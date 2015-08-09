#ifndef soawidget_h
#define soawidget_h

#include "database/soadatabase.h"
#include "interfaces/iview.h"
#include <QMainWindow>
#include <QHash>



class IView;
class SoaGtwy;
class WdgTreeContainer;
class WdgTabContainer;
class WdgInspectorContainer;
class MainWindow;
class QActionGroup;
struct HiFilter;



class SoaWg : public QMainWindow
{
	Q_OBJECT

public:
	SoaWg(MainWindow *, const QString &, const QString &, bool);
	~SoaWg();

	bool IsReadyToClose();

	bool operator==(const QString &) const;

	SoaDb &Db();
	const QString &Path() const;
	FilterMode Mode() const;

	void Open();
	void LoadSQL(const QString &);
	void LoadCentraSiteReportXML(const QString &);
	bool LoadPortfolio(const QString &);
	void DumpSQL();
	void Save();

	void RefreshViews();
	void AddGraph(int);
	void AddGraph(int, FilterMode);
	void AddGraph(int, const HiFilter &);
	void AddTable(unsigned int);
	void OpenRegistryWidget(SoaGtwy *);

	void UpdateTitle();

	// actions
	QAction *GetAction(const QString &);

protected:
	QString path, name;
	SoaDb db;
	WdgTabContainer *tab;
	QDockWidget *treeDock, *inspectorDock;
	WdgTreeContainer *tree;
	WdgInspectorContainer *inspector;
	IViewList views;
	unsigned int changeId;
	FilterMode mode;
	QToolBar *toolbar;
	QActionGroup *group;
	QAction *infrastructureAction;
	QMenu *domainsMenu;
	QAction *manageIntermediaries, *manageRegistries;

	// add asset actions
	QHash<QString, QAction *> actionsHash;

	void AddView(IView *);

	// actions
	void CreateActions();
	void RefreshActions();
	void RefreshDomainsMenu();
	QAction *AddAction(const QString &, const char *);

signals:
	void retitleSignal(QWidget *, const QString &);

public slots:
	void ModeButtonTriggeredSlot(QAction *);
	void InfrastructureButtonClickedSlot();

	void AddUseSlot();
	void DeleteUseSlot();
	void AddOrganizationSlot();
	void AddBusinessUnitSlot();
	void AddServiceDomainSlot();
	void AddApplicationSlot();
	void AddApplicationVersionSlot();
	void AddServiceSlot();
	void AddServiceVersionSlot();

	void DeleteOrganizationSlot();
	void DeleteBusinessUnitSlot();
	void DeleteApplicationSlot();
	void DeleteApplicationVersionSlot();
	void DeleteDeployedApplicationSlot();
	void DeleteServiceSlot();
	void DeleteServiceVersionSlot();
	void DeleteDeployedServiceSlot();
	void DeleteEndpointSlot();
	void DeleteAsset(SoaTb *);

	void DomainsMenuSlot(QAction *);
	void ManageIntermediariesSlot();
	void ManageRegistriesSlot();
};

#endif
