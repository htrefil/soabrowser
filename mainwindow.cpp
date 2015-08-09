#include "mainwindow.h"
#include "soawidget.h"
#include "logger/logger.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QSettings>
#include <QFileInfo>
#include <QDir>



MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {

	ui->setupUi(this);
	setWindowTitle("SoaBrowser");

	connect(ui->actionNew_repository, SIGNAL(triggered()), this, SLOT(NewRepositorySlot()));
	connect(ui->actionLoad_repository, SIGNAL(triggered()), this, SLOT(LoadRepositorySlot()));
	connect(ui->actionSave_repository, SIGNAL(triggered()), this, SLOT(SaveRepositorySlot()));
	connect(ui->actionClose_repository, SIGNAL(triggered()), this, SLOT(CloseRepositorySlot()));
	connect(ui->actionSelect_logging_directory, SIGNAL(triggered()), this, SLOT(SelectLoggingDirectorySlot()));
	connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(QuitSlot()));

	connect(ui->actionImport_CentraSite_report, SIGNAL(triggered()), this, SLOT(LoadCentraSiteReportSlot()));
	connect(ui->actionImport_portfolio, SIGNAL(triggered()), this, SLOT(LoadPortfolioSlot()));
	connect(ui->actionImport_scheme, SIGNAL(triggered()), this, SLOT(LoadSchemaSlot()));
//	connect(ui->actionLoad_SQL, SIGNAL(triggered()), this, SLOT(LoadSQLSlot()));
//	connect(ui->actionDump_SQL, SIGNAL(triggered()), this, SLOT(DumpSQLSlot()));

	connect(ui->actionAdd_2D_graph, SIGNAL(triggered()), this, SLOT(Add2DGraphSlot()));
	connect(ui->actionAdd_2_5D_graph, SIGNAL(triggered()), this, SLOT(Add25DGraphSlot()));
	connect(ui->actionAdd_3D_graph, SIGNAL(triggered()), this, SLOT(Add3DGraphSlot()));

	connect(ui->actionWelcome_to_SoaScape, SIGNAL(triggered()), this, SLOT(WelcomeSlot()));

	ReadSettings();
}

MainWindow::~MainWindow() {
	delete ui;
}

void MainWindow::ShowRepository(const QString &path, bool load) {

	if (!FocusRepository(path)) {

		QFileInfo info(path);
		if (load && !recents.Add(path))
			return;

		repositoryPath = info.path();
		WriteSettings();

		ui->tabWidget->setCurrentIndex(ui->tabWidget->addTab(new SoaWg(this, path, info.baseName(), load), info.baseName()));
	}
}

bool MainWindow::FocusRepository(const QString &path) {

	for (int i = 0; i < ui->tabWidget->count(); ++i) {
		if (*((SoaWg *)ui->tabWidget->widget(i)) == path) {

			ui->tabWidget->setCurrentIndex(i);
			recents.Add(path);

			return true;
		}
	}

	return false;
}

//
// events
//

void MainWindow::closeEvent(QCloseEvent *event) {
	WriteSettings();
	event->accept();
}

//
// settings
//

void MainWindow::ReadSettings() {

	QSettings settings(QApplication::applicationDirPath() + "/soabrowser.ini", QSettings::IniFormat);

	settings.beginGroup("MainWindow");
	restoreGeometry(settings.value("geometry").toByteArray());
	restoreState(settings.value("windowState").toByteArray());
	settings.endGroup();

	settings.beginGroup("Paths");
	repositoryPath = settings.value("repositoryPath", QApplication::applicationDirPath()).toString();
	reportPath = settings.value("reportPath", QApplication::applicationDirPath()).toString();
	portfolioPath = settings.value("portfolioPath", QApplication::applicationDirPath()).toString();
	schemaPath = settings.value("schemaPath", QApplication::applicationDirPath()).toString();
	Logger::ins.SetLocation(settings.value("logPath", QDir::tempPath()).toString());
	settings.endGroup();

	recents.Deserialize(settings);
}

void MainWindow::WriteSettings() {

	QSettings settings(QApplication::applicationDirPath() + "/soabrowser.ini", QSettings::IniFormat);

	settings.beginGroup("MainWindow");
	settings.setValue("geometry", saveGeometry());
	settings.setValue("windowState", saveState());
	settings.endGroup();

	settings.beginGroup("Paths");
	settings.setValue("repositoryPath", repositoryPath);
	settings.setValue("reportPath", reportPath);
	settings.setValue("portfolioPath", portfolioPath);
	settings.setValue("schemaPath", schemaPath);
	settings.setValue("logPath", Logger::ins.Location());
	settings.endGroup();

	recents.Serialize(settings);
}

SoaWg *MainWindow::CurrRepo() {
	return (SoaWg *)ui->tabWidget->currentWidget();
}
