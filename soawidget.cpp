#include "mainwindow.h"
#include "soawidget.h"
#include "database/soapattern.h"
#include "database/soalogger.h"
#include "widgets/wdgtab.h"
#include "widgets/wdgtree.h"
#include "widgets/wdggraph.h"
#include "widgets/wdgtable.h"
#include "widgets/wdginspector.h"
#include "widgets/wdggateway.h"
#include <QWidgetAction>
#include <QMessageBox>
#include <QTextStream>
#include <QDockWidget>
#include <QComboBox>
#include <QTextEdit>
#include <QToolBar>
#include <QFile>

#define OUTLINE_STRING	"Outline"
#define REALIZATION_STRING	"Realization"
#define INFRASTRUCTURE_STRING	"Infrastructure"



SoaWg::SoaWg(MainWindow *_parent, const QString &_path, const QString &_name, bool open) :
	QMainWindow(_parent), path(_path), name(_name),
	tab(new WdgTabContainer(this)),
	treeDock(new QDockWidget("Tree", this)),
	inspectorDock(new QDockWidget("Properties", this)),
	tree(new WdgTreeContainer(this, &db)),
	inspector(new WdgInspectorContainer(this, &db)),
	changeId(0), mode(fmRealization),
	domainsMenu(new QMenu(this)),
	manageIntermediaries(new QAction("Manage intermediaries...", this)),
	manageRegistries(new QAction("Manage registries...", this)) {

	CreateActions();

	// toolbar

	toolbar = new QToolBar("Repository toolbar", this);
	QFont f = toolbar->font();
	f.setBold(true);
	toolbar->setFont(f);
	toolbar->setIconSize(QSize(80, 60));
	toolbar->setMovable(false);
	toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

	QAction *a;
	group = new QActionGroup(this);
	a = group->addAction(QIcon(":/images/resources/outline.png"), OUTLINE_STRING);
	a->setCheckable(true);
	a = group->addAction(QIcon(":/images/resources/realization.png"), REALIZATION_STRING);
	a->setCheckable(true);
	a->setChecked(true);
	infrastructureAction = toolbar->addAction(QIcon(":/images/resources/infrastructure.png"), INFRASTRUCTURE_STRING);

	QWidget *spacer = new QWidget(toolbar);
	spacer->setMinimumHeight(20);
	toolbar->addWidget(spacer);
	toolbar->insertActions(infrastructureAction, group->actions());
	addToolBar(Qt::LeftToolBarArea, toolbar);

	// tree dock

	treeDock->setWindowTitle("Assets");
	treeDock->setWidget(tree);
	views.push_back(tree);

	inspectorDock->setWidget(inspector);
	views.push_back(inspector);

	// docks

	addDockWidget(Qt::LeftDockWidgetArea, treeDock);
	addDockWidget(Qt::LeftDockWidgetArea, inspectorDock);

	// tabs

	setCentralWidget(tab);
	views.push_back(tab);
	AddGraph(svm25D, fmOutline);
	AddGraph(svm25D, fmRealization);
	AddGraph(svm25D, fmRuntime);

	// load from file

	if (open)
		Open();

	connect(group, SIGNAL(triggered(QAction*)), this, SLOT(ModeButtonTriggeredSlot(QAction*)));
	connect(infrastructureAction, SIGNAL(triggered()), this, SLOT(InfrastructureButtonClickedSlot()));
	connect(this, SIGNAL(retitleSignal(QWidget *, QString)), _parent, SLOT(RetitleTabSlot(QWidget *, QString)));
	connect(domainsMenu, SIGNAL(triggered(QAction*)), this, SLOT(DomainsMenuSlot(QAction*)));
	connect(manageIntermediaries, SIGNAL(triggered()), this, SLOT(ManageIntermediariesSlot()));
	connect(manageRegistries, SIGNAL(triggered()), this, SLOT(ManageRegistriesSlot()));
}

SoaWg::~SoaWg() {}

bool SoaWg::IsReadyToClose() {

	if (changeId != db.ChangeId()) {

		int res = QMessageBox::information(this,
			 "Close repository \"" + name + "\"",
			 "There are unsaved changes in this repository,\ndo you want to save them?",
			 QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);

		if (res == QMessageBox::Yes)
			db.ToDB(path.toStdString());

		return res != QMessageBox::Cancel;
	}

	return true;
}

bool SoaWg::operator==(const QString &_path) const {
	return _path == path;
}

SoaDb &SoaWg::Db() {
	return db;
}

const QString &SoaWg::Path() const {
	return path;
}

FilterMode SoaWg::Mode() const {
	return mode;
}

//
// loading
//

// Loads SQLite db.
void SoaWg::Open() {

	if (db.FromDB(path.toStdString())) {
		changeId = db.ChangeId();
		RefreshViews();
	}
	else
		QMessageBox::warning(this, "Load repository", "Error loading repository from \"" + path + "\".", QMessageBox::Ok, QMessageBox::Ok);
}

// Interprets SQL source into a temporary SQLite db,
// then loads the db.
void SoaWg::LoadSQL(const QString &path) {
	sl::log << "loading sql " << path.toStdString() << sl::end;

	QFile file(path);
	if (file.open(QFile::ReadOnly))
		db.FromSQL(QTextStream(&file).readAll().toStdString());
	else
		sl::log << "could not open " << path.toStdString() << sl::end;

	RefreshViews();
}

// Parse CentraSite XML report.
void SoaWg::LoadCentraSiteReportXML(const QString &path) {
	sl::log << "loading xml " << path.toStdString() << sl::end;

	db.FromXML(path.toStdString());

	RefreshViews();
}

bool SoaWg::LoadPortfolio(const QString &path) {
	sl::log << "loading portfolio " << path.toStdString() << sl::end;

	if (!db.Tb("Application")->IsSchemaValid())
		return false;

	db.Tb("Application")->LoadPortfolio(path.toStdString());
	RefreshViews();

	return true;
}

//
// serializing
//

void SoaWg::DumpSQL() {
	sl::log << "dumping db to sql" << sl::end;

	QFile file(path + ".sql");
	if (file.open(QFile::WriteOnly | QFile::Truncate)) {
		std::string sql;
		QTextStream(&file) << db.ToSQL(sql, sptSQLite).c_str();
	}
	else
		sl::log << "could not open " << path.toStdString() << sl::end;
}

void SoaWg::Save() {
	sl::log << "dumping db to db" << sl::end;

	db.ToDB(path.toStdString());
	changeId = db.ChangeId();
	UpdateTitle();
}

//
// views
//

void SoaWg::RefreshViews() {

	// refresh contained views

	for (IViewList::iterator view = views.begin(); view != views.end(); ++view)
		(*view)->Refresh();

	// refresh runtime buttons

	IgDomainMap dm = db.DomainMap();
	QList<QAction *> actns = group->actions();

	std::vector<QAction *> acs;
	for (QList<QAction *>::iterator a = actns.begin(); a != actns.end(); ++a)
		acs.push_back(*a);

	for (std::vector<QAction *>::iterator a = acs.begin(); a != acs.end();) {

		if ((*a)->text() != OUTLINE_STRING && (*a)->text() != REALIZATION_STRING) {

			IgDomainMap::iterator i = dm.find((*a)->data().toUInt());

			if (i == dm.end()) {
				toolbar->removeAction(*a);
				group->removeAction(*a);
				a = acs.erase(a);
			}
			else {
				dm.erase(i);
				++a;
			}
		}
		else
			++a;
	}

	for (IgDomainMap::iterator i = dm.begin(); i != dm.end(); ++i) {
		QAction *a = new QAction(QIcon(":/images/resources/runtime.png"), QS(i->second.text), this);
		a->setData(i->first);
		a->setCheckable(true);
		toolbar->insertAction(infrastructureAction, a);
		group->addAction(a);
	}

	RefreshActions();
	RefreshDomainsMenu();

	// update database widget title

	UpdateTitle();
}

void SoaWg::AddGraph(int solverMode) {
	WdgGraphContainer *graph = new WdgGraphContainer(this, &db, mode, (SolverMode)solverMode, HiFilter(), GetAction("Add use"), GetAction("Delete use"));
	tab->AddTab(graph, mode);

	RefreshViews();
}

void SoaWg::AddGraph(int solverMode, FilterMode fm) {
	WdgGraphContainer *graph = new WdgGraphContainer(this, &db, fm, (SolverMode)solverMode, HiFilter(), GetAction("Add use"), GetAction("Delete use"));
	tab->AddTab(graph, fm);

	RefreshViews();
}

void SoaWg::AddGraph(int solverMode, const HiFilter &hifi) {
	WdgGraphContainer *graph = new WdgGraphContainer(this, &db, mode, (SolverMode)solverMode, hifi, GetAction("Add use"), GetAction("Delete use"));
	tab->AddTab(graph, mode);

	RefreshViews();
}

void SoaWg::AddTable(unsigned int tbIndex) {
	WdgTableContainer *table = new WdgTableContainer(this, &db, tbIndex);
	tab->AddTab(table, mode);

	RefreshViews();
}

void SoaWg::OpenRegistryWidget(SoaGtwy *reg) {
	if (!tab->ContainsSpecialTab(ivsGateway))
		tab->AddTab(new WdgGateway(this, reg), mode);
	else {
		// set reg!!!
	}

	RefreshViews();
}

void SoaWg::UpdateTitle() {
	emit retitleSignal(this, (changeId == db.ChangeId()) ? name : name + "*");
}

//
// slots
//

void SoaWg::ModeButtonTriggeredSlot(QAction *action) {

	if (action->text() == OUTLINE_STRING)
		mode = fmOutline;
	else if (action->text() == REALIZATION_STRING)
		mode = fmRealization;
	else {
		db.SelectDomain(action->data().toUInt());
		mode = fmRuntime;
	}

	RefreshViews();
}

void SoaWg::InfrastructureButtonClickedSlot() {

	domainsMenu->exec(QCursor::pos());
}
