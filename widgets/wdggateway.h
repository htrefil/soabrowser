#ifndef wdggateway_h
#define wdggateway_h

#include "database/soarow.h"
#include "interfaces/iview.h"
#include "gateways/gateway.h"
#include <QTreeWidget>



enum MergeWidgetMode { mwmTree = 0, mwmList };

class SoaWg;
class SoaDb;

class WdgGatewayTree : public QTreeWidget
{
	Q_OBJECT

public:
	SoaWg *wg;
	GtDomain *registry, *conflation;
	MergeWidgetMode mode;
	bool guard;

	WdgGatewayTree(QWidget *, SoaWg *, GtDomain *);

	void Refresh();

	void mouseDoubleClickEvent(QMouseEvent *);
	void mousePressEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);

private:
	int travel;

signals:
	void DeployServiceSignal(const GtService &);
	void DeployEndpointSignal(const GtService &, const GtEndpoint &);
	void SelectServiceSignal(const GtService &);
	void SelectEndpointSignal(const GtEndpoint &);
	void BrowseEndpointIndexSignal(const GtEndpoint &);

public slots:
	void itemSelectionChangedSlot();
};



class SoaWg;
class SoaTb;
class QComboBox;
class QCheckBox;

class WdgGateway : public IView
{
	Q_OBJECT

public:
	WdgGateway(SoaWg *, SoaGtwy *);

	QString Refresh();
	int IsSingleton() { return ivsGateway; }

	void Conflate();


private:
	SoaWg *wg;
	GtDomains gtDomains;	// registry entities
	GtDomain *domain;		// currently selected domain (registry entities only)
	GtDomain conflation;	// merged entites (registry & database)
	WdgGatewayTree *tree;
	QComboBox *cb;
	QCheckBox *existingCheckBox;
	MergeWidgetMode mode, prevMode;
	bool showExisting;

	SoaRw *NewService(SoaTb *, const std::string &);
	SoaRw *NewServiceVersion(SoaTb *, const SoaRwList *, SoaRw *);

private slots:
	void ModeChangedSlot(int);
	void ExistingToggledSlot(bool);
	void DeployServiceSlot(const GtService &);
	void DeployEndpointSlot(const GtService &, const GtEndpoint &);
	void SelectServiceSlot(const GtService &);
	void SelectEndpointSlot(const GtEndpoint &);
	void BrowseEndpointIndexSlot(const GtEndpoint &);
};

#endif
