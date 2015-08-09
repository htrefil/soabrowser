#include "widgets/wdggateway.h"
#include "soawidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QCheckBox>

using std::pair;
using std::map;



WdgGateway::WdgGateway(SoaWg *_wg, SoaGtwy *_gateway) : IView(_wg),
	wg(_wg),
	gtDomains(_gateway->ConstructTree()),
	tree(new WdgGatewayTree(this, _wg, &conflation)),
	cb(new QComboBox()),
	existingCheckBox(new QCheckBox("Show existing endpoints")),
	mode(mwmTree),
	prevMode(mwmList),
	showExisting(true) {

	setWindowTitle(QS("Run-time registry \"" + gtDomains.registryName + "\""));

	cb->addItem("Services & endpoints", mwmTree);
	cb->addItem("Endpoints only", mwmList);
	connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(ModeChangedSlot(int)));

	existingCheckBox->setChecked(showExisting);
	connect(existingCheckBox, SIGNAL(toggled(bool)), this, SLOT(ExistingToggledSlot(bool)));

	QHBoxLayout *toolbar = new QHBoxLayout();
	toolbar->addWidget(cb);
	toolbar->addWidget(existingCheckBox);
	toolbar->setContentsMargins(0, 0, 0, 0);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addLayout(toolbar);
	layout->addWidget(tree);

	tree->setHeaderLabels(QStringList() <<
						  "Name" <<
						  "Status" <<
						  "Resolution path" <<
						  "Protected end-point" <<
						  "Registry folder" <<
						  "WSDL" <<
						  "Intermediary version" <<
						  "Policy version" <<
						  "Intermediary ID" <<
						  "SOAP" <<
						  "Enabled");

	connect(tree, SIGNAL(DeployServiceSignal(GtService)), this, SLOT(DeployServiceSlot(GtService)));
	connect(tree, SIGNAL(DeployEndpointSignal(GtService, GtEndpoint)), this, SLOT(DeployEndpointSlot(GtService, GtEndpoint)));
	connect(tree, SIGNAL(SelectServiceSignal(GtService)), this, SLOT(SelectServiceSlot(GtService)));
	connect(tree, SIGNAL(SelectEndpointSignal(GtEndpoint)), this, SLOT(SelectEndpointSlot(GtEndpoint)));
	connect(tree, SIGNAL(BrowseEndpointIndexSignal(GtEndpoint)), this, SLOT(BrowseEndpointIndexSlot(GtEndpoint)));
}

QString WdgGateway::Refresh() {

	// conflate db & gt data for the selected service domain

	tree->mode = mode;
	tree->registry = domain = gtDomains.Find(wg->Db().SelectedDomainName());
	if (domain == 0)
		return windowTitle();

	conflation = *domain;
	Conflate();

	// update header

	if (mode != prevMode) {

		prevMode = mode;

		if (mode == mwmTree) {

			tree->setHeaderLabels(QStringList() <<
								  "Name" <<
								  "Status" <<
								  "Resolution path" <<
								  "Protected end-point" <<
								  "Registry folder" <<
								  "WSDL" <<
								  "Intermediary version" <<
								  "Policy version" <<
								  "Intermediary ID" <<
								  "SOAP" <<
								  "Enabled");
		}
		else {

			tree->setHeaderLabels(QStringList() <<
								  "Service name" <<
								  "Name" <<
								  "Status" <<
								  "Resolution path" <<
								  "Protected end-point" <<
								  "Registry folder" <<
								  "WSDL" <<
								  "Intermediary version" <<
								  "Policy version" <<
								  "Intermediary ID" <<
								  "SOAP" <<
								  "Enabled");
		}
	}

	// list conflated data

	tree->guard = true;
	bool virginTree = false;

	if (mode == mwmTree) {

		// create origin map

		map<unsigned, pair<GtService *, map<unsigned, GtEndpoint *> > > sm;
		for (GtDomain::iterator s = conflation.begin(); s != conflation.end(); ++s) {

			map<unsigned, GtEndpoint *> em;
			for (GtService::iterator e = s->begin(); e != s->end(); ++e)
				em.insert(pair<unsigned, GtEndpoint *>(e->viewId, &(*e)));

			sm.insert(pair<unsigned, pair<GtService *, map<unsigned, GtEndpoint *> > >(s->viewId, pair<GtService *, map<unsigned, GtEndpoint *> >(&(*s), em)));
		}

		// cross-reference origin map and the tree

		for (int i = 0; i < tree->topLevelItemCount();) {

			QTreeWidgetItem *serviceItem = tree->topLevelItem(i);

			map<unsigned, pair<GtService *, map<unsigned, GtEndpoint *> > >::iterator s = sm.find(serviceItem->data(0, Qt::UserRole + 2).toUInt());
			if (s != sm.end()) {

				// service found

				serviceItem->setSelected(s->second.first->selected);
				serviceItem->setHidden(!showExisting && s->second.first->status == gesOld);
				virginTree = false;

				// update endpoints

				for (int j = 0; j < serviceItem->childCount();) {

					QTreeWidgetItem *endpointItem = serviceItem->child(j);

					map<unsigned, GtEndpoint *> &em = s->second.second;

					map<unsigned, GtEndpoint *>::iterator e = em.find(endpointItem->data(0, Qt::UserRole + 2).toUInt());
					if (e != em.end()) {

						// endpoint found

						endpointItem->setSelected(e->second->selected);
						endpointItem->setHidden(!showExisting && e->second->status == gesOld);

						em.erase(e);
						++j;
					}
					else {

						// endpoint deleted

						delete endpointItem;
					}
				}

				sm.erase(s);
				++i;
			}
			else {

				// service deleted

				delete serviceItem;
			}
		}

		// add remaining origin items to the tree

		for (map<unsigned, pair<GtService *, map<unsigned, GtEndpoint *> > >::iterator s = sm.begin(); s != sm.end(); ++s) {

			GtService *service = s->second.first;

			QTreeWidgetItem *serviceItem = new QTreeWidgetItem(tree, QStringList() << QS(service->name) << QS(service->StatusToString()));
			serviceItem->setData(0, Qt::UserRole, service->sId);
			serviceItem->setData(0, Qt::UserRole + 1, service->eId);
			serviceItem->setData(0, Qt::UserRole + 2, service->viewId);
			serviceItem->setSelected(service->selected);
			serviceItem->setHidden(!showExisting && service->status == gesOld);

			map<unsigned, GtEndpoint *> &em = s->second.second;

			for (map<unsigned, GtEndpoint *>::iterator e = em.begin(); e != em.end(); ++e) {

				GtEndpoint *endpoint = e->second;

				QTreeWidgetItem *endpointItem = new QTreeWidgetItem(serviceItem, QStringList() <<
															 QS(endpoint->intGivenName) <<
															 QS(endpoint->StatusToString()) <<
															 QS(endpoint->resolutionPath) <<
															 QS(endpoint->protectedEndpoint) <<
															 QS(endpoint->urlRegistryFolder) <<
															 QS(endpoint->urlWSDL) <<
															 QS(endpoint->intGivenVersion) <<
															 QS(endpoint->policyVersion) <<
															 QS(endpoint->intGivenId) <<
															 QS(endpoint->soap) <<
															 QS(endpoint->enabled));
				endpointItem->setData(0, Qt::UserRole, endpoint->sId);
				endpointItem->setData(0, Qt::UserRole + 1, endpoint->eId);
				endpointItem->setData(0, Qt::UserRole + 2, endpoint->viewId);
				endpointItem->setSelected(endpoint->selected);
				endpointItem->setHidden(!showExisting && endpoint->status == gesOld);
			}
		}
	}
	else {

		map<unsigned, GtEndpoint *> em;

		for (GtDomain::iterator s = conflation.begin(); s != conflation.end(); ++s)
			for (GtService::iterator e = s->begin(); e != s->end(); ++e)
				em.insert(pair<unsigned, GtEndpoint *>(e->viewId, &(*e)));

		for (int i = 0; i < tree->topLevelItemCount();) {

			QTreeWidgetItem *item = tree->topLevelItem(i);

			map<unsigned, GtEndpoint *>::iterator f = em.find(item->data(0, Qt::UserRole + 2).toUInt());
			if (f != em.end()) {

				item->setSelected(f->second->selected);
				item->setHidden(!showExisting && f->second->status == gesOld);

				em.erase(f);
				++i;
			}
			else
				delete item;
		}

		for (map<unsigned, GtEndpoint *>::iterator e = em.begin(); e != em.end(); ++e) {

			GtEndpoint *endpoint = e->second;

			QTreeWidgetItem *endpointItem = new QTreeWidgetItem(tree, QStringList() <<
														 QS(endpoint->intGivenName) <<
														 QS(endpoint->StatusToString()) <<
														 QS(endpoint->resolutionPath) <<
														 QS(endpoint->protectedEndpoint) <<
														 QS(endpoint->urlRegistryFolder) <<
														 QS(endpoint->urlWSDL) <<
														 QS(endpoint->intGivenVersion) <<
														 QS(endpoint->policyVersion) <<
														 QS(endpoint->intGivenId) <<
														 QS(endpoint->soap) <<
														 QS(endpoint->enabled));
			endpointItem->setData(0, Qt::UserRole, endpoint->sId);
			endpointItem->setData(0, Qt::UserRole + 1, endpoint->eId);
			endpointItem->setData(0, Qt::UserRole + 2, endpoint->viewId);
			endpointItem->setSelected(endpoint->selected);
			endpointItem->setHidden(!showExisting && endpoint->status == gesOld);
		}
	}

	tree->guard = false;

	if (!virginTree)
		tree->expandAll();

	return windowTitle();
}
