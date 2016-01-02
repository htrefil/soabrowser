#include "database/soadatabase.h"
#include "interfaces/iview.h"

#include <iostream>

//
// graph
//

IgNodeMap SoaDb::NodeMap(int filter, unsigned int domainId) {
	IgNodeMap nodeMap;


	SoaRw *org = Tb("Organization")->FrontRw();

	if (filter == fmOutline) {
		SoaTb *nodes = Tb("Application");
		for (SoaRwList::iterator row = nodes->begin(); row != nodes->end(); ++row)
			nodeMap.insert(IgNodePair((*row)->viewId, IgNode(
				(*row)->viewTs,
				(*row)->selId == selId,
				(*row)->Ce("BusinessUnit")->Row()->Ce("Organization")->Row() != org,
				(*row)->Ce("singleUser")->Num() == 0,
				(*row)->Name()
			)));
	}
	else if (filter == fmRealization) {
		SoaTb *nodes = Tb("ApplicationVersion");
		for (SoaRwList::iterator row = nodes->begin(); row != nodes->end(); ++row)
			nodeMap.insert(IgNodePair((*row)->viewId, IgNode(
				(*row)->viewTs,
				(*row)->selId == selId,
				(*row)->Ce("Application")->Row()->Ce("BusinessUnit")->Row()->Ce("Organization")->Row() != org,
				(*row)->Ce("Application")->Row()->Ce("singleUser")->Num() == 0,
				(*row)->Name()
			)));
	}
	else if (filter == fmRuntime) {
		SoaTb *nodes = Tb("DeployedApplication");
		for (SoaRwList::iterator row = nodes->begin(); row != nodes->end(); ++row) {
			SoaRw *domain = (*row)->Ce("ServiceDomain")->Row();
			if (domain && domain->ViewId() == domainId) {

				nodeMap.insert(IgNodePair((*row)->viewId, IgNode(
					(*row)->viewTs,
					(*row)->selId == selId,
					(*row)->Ce("ApplicationVersion")->Row()->Ce("Application")->Row()->Ce("external")->Num() != 0,
					(*row)->Ce("ApplicationVersion")->Row()->Ce("Application")->Row()->Ce("singleUser")->Num() == 0,
					(*row)->Name()
				)));
			}
		}
	}

	return nodeMap;
}

IgSubNodeMap SoaDb::SubNodeMap(int filter, unsigned int domainId) {
	IgSubNodeMap subNodeMap;

	SoaRw *org = Tb("Organization")->FrontRw();

	if (filter == fmOutline) {
		SoaTb *subNodes = Tb("Service");
		for (SoaRwList::iterator row = subNodes->begin(); row != subNodes->end(); ++row)
			subNodeMap.insert(IgSubNodePair((*row)->viewId, IgSubNode(
				(*row)->viewTs,
				(*row)->selId == selId,
				(*row)->Ce("Application")->Row()->Ce("BusinessUnit")->Row()->Ce("Organization")->Row() != org,
				(*row)->Name(),
				(*row)->Ce("Application")->Row()->viewId
			)));
	}
	else if (filter == fmRealization) {
		SoaTb *subNodes = Tb("ServiceVersion");
		for (SoaRwList::iterator row = subNodes->begin(); row != subNodes->end(); ++row)
			subNodeMap.insert(IgSubNodePair((*row)->viewId, IgSubNode(
				(*row)->viewTs,
				(*row)->selId == selId,
				(*row)->Ce("ApplicationVersion")->Row()->Ce("Application")->Row()->Ce("BusinessUnit")->Row()->Ce("Organization")->Row() != org,
				(*row)->Name(),
				(*row)->Ce("ApplicationVersion")->Row()->viewId
			)));
	}
	else if (filter == fmRuntime) {

		SoaTb *subNodes = Tb("DeployedService");
		for (SoaRwList::iterator row = subNodes->begin(); row != subNodes->end(); ++row) {

			SoaRw *domain = 0;

			const SoaRwList *endpoints = (*row)->Children("Endpoint");
			for (SoaRwList::const_iterator e = endpoints->begin(); e != endpoints->end(); ++e) {
				domain = (*e)->Ce("ServiceDomain")->Row();
				if (domain)
					break;
			}

			if (domain && domain->ViewId() == domainId)

				subNodeMap.insert(IgSubNodePair((*row)->viewId, IgSubNode(
					(*row)->viewTs,
					(*row)->selId == selId,
					(*row)->Ce("ServiceVersion")->Row()->Ce("Service")->Row()->Ce("external")->Num() != 0,
					(*row)->Name(),
					(*row)->Ce("DeployedApplication")->Row()->viewId
				)));
		}
	}

	return subNodeMap;
}

IgLinkMap SoaDb::LinkMap(int filter, unsigned int domainId) {
	IgLinkMap linkMap;

	if (filter == fmOutline) {
		SoaTb *links = Tb("DesignTimeServiceUse");
		for (SoaRwList::iterator row = links->begin(); row != links->end(); ++row)
			linkMap.insert(IgLinkPair((*row)->viewId, IgLink(
				(*row)->viewTs,
				(*row)->selId == selId,
				(*row)->Ce("ApplicationVersion")->Row()->Ce("Application")->Row()->viewId,
				(*row)->Ce("ServiceVersion")->Row()->Ce("Service")->Row()->Ce("Application")->Row()->viewId,
				(*row)->Ce("ServiceVersion")->Row()->Ce("Service")->Row()->viewId
			)));
	}
	else if (filter == fmRealization) {
		SoaTb *links = Tb("DesignTimeServiceUse");
		for (SoaRwList::iterator row = links->begin(); row != links->end(); ++row)
			linkMap.insert(IgLinkPair((*row)->viewId, IgLink(
				(*row)->viewTs,
				(*row)->selId == selId,
				(*row)->Ce("ApplicationVersion")->Row()->viewId,
				(*row)->Ce("ServiceVersion")->Row()->Ce("ApplicationVersion")->Row()->viewId,
				(*row)->Ce("ServiceVersion")->Row()->viewId
			)));
	}
	else if (filter == fmRuntime) {
		SoaTb *links = Tb("RuntimeServiceUse");
		for (SoaRwList::iterator row = links->begin(); row != links->end(); ++row) {
			SoaRw *domain = (*row)->Ce("DeployedApplication")->Row()->Ce("ServiceDomain")->Row();
			if (domain && domain->ViewId() == domainId)
				linkMap.insert(IgLinkPair((*row)->viewId, IgLink(
					(*row)->viewTs,
					(*row)->selId == selId,
					(*row)->Ce("DeployedApplication")->Row()->viewId,
					(*row)->Ce("Endpoint")->Row()->Ce("DeployedService")->Row()->Ce("DeployedApplication")->Row()->viewId,
					(*row)->Ce("Endpoint")->Row()->Ce("DeployedService")->Row()->viewId
				)));
		}
	}

	return linkMap;
}

IgDomainMap SoaDb::DomainMap() {
	IgDomainMap domainMap;

	SoaTb *domains = Tb("ServiceDomain");
	for (SoaRwList::iterator row = domains->begin(); row != domains->end(); ++row)
		domainMap.insert(IgDomainPair((*row)->ViewId(), IgDomain((*row)->Name(), (*row)->selId == selId)));

	return domainMap;
}

//
// tree
//

ItItemMap SoaDb::ItemMap(int filter, unsigned int domainId) {

	SoaRw *selRw = selTb ? selTb->FocusRw() : 0;
	ItItemMap itemMap(selRw ? selRw->ViewId() : SDB_NULL_INSPECTEE);

	bool externalOrg = false;

	int i = 0;
	SoaTb *orgs = Tb("Organization");
	for (SoaRwList::iterator org = orgs->begin(); org != orgs->end(); ++org) {
		ItItemMap::iterator orgIt = itemMap.insert(ItItemPair((*org)->viewId, ItItem(
			(*org)->viewTs,
			(*org)->selId == selId,
			(*org)->Ce("name")->Txt(),
			true, externalOrg ? iiiOrg2 : iiiOrg, i++
		))).first;

		const SoaRwList *depts = (*org)->Children("BusinessUnit");
		for (SoaRwList::const_iterator dept = depts->begin(); dept != depts->end(); ++dept) {
			ItItemMap::iterator deptIt = orgIt->second.children.insert(ItItemPair((*dept)->viewId, ItItem(
				(*dept)->viewTs,
				(*dept)->selId == selId,
				(*dept)->Ce("name")->Txt(),
				true, externalOrg ? iiiUnit2 : iiiUnit
			))).first;

			const SoaRwList *apps = (*dept)->Children("Application");
			for (SoaRwList::const_iterator app = apps->begin(); app != apps->end(); ++app) {
				ItItemMap::iterator appIt = deptIt->second.children.insert(ItItemPair((*app)->viewId, ItItem(
					(*app)->viewTs,
					(*app)->selId == selId,
					(*app)->Ce("name")->Txt(),
					true, (filter != fmOutline)? iiiApp1 : (externalOrg? iiiApp2 : iiiApp)
				))).first;

				if (filter == fmOutline) {

					const SoaRwList *services = (*app)->Children("Service");
					for (SoaRwList::const_iterator service = services->begin(); service != services->end(); ++service) {
						appIt->second.children.insert(ItItemPair((*service)->viewId, ItItem(
							(*service)->viewTs,
							(*service)->selId == selId,
							(*service)->Ce("name")->Txt(),
							true, externalOrg ? iiiServ2 : iiiServ
						)));
					}
				}
				else if (filter == fmRealization) {

					const SoaRwList *appVers = (*app)->Children("ApplicationVersion");
					for (SoaRwList::const_iterator appVer = appVers->begin(); appVer != appVers->end(); ++appVer) {
						ItItemMap::iterator appVerIt = appIt->second.children.insert(ItItemPair((*appVer)->viewId, ItItem(
							(*appVer)->viewTs,
							(*appVer)->selId == selId,
							(*appVer)->Ce("version")->Txt(),
							true, externalOrg ? iiiApp2 : iiiApp
						))).first;

						const SoaRwList *servVers = (*appVer)->Children("ServiceVersion");
						for (SoaRwList::const_iterator servVer = servVers->begin(); servVer != servVers->end(); ++servVer) {
							appVerIt->second.children.insert(ItItemPair((*servVer)->viewId, ItItem(
								(*servVer)->viewTs,
								(*servVer)->selId == selId,
								(*servVer)->Name(),
								true, externalOrg ? iiiServ2 : iiiServ
							)));
						}
					}
				}
				else if (filter == fmRuntime) {

					const SoaRwList *appVers = (*app)->Children("ApplicationVersion");
					for (SoaRwList::const_iterator appVer = appVers->begin(); appVer != appVers->end(); ++appVer) {
						ItItemMap::iterator appVerIt = appIt->second.children.insert(ItItemPair((*appVer)->viewId, ItItem(
							(*appVer)->viewTs,
							(*appVer)->selId == selId,
							(*appVer)->Name(),
							true, externalOrg ? iiiApp1 : iiiApp1
						))).first;

						const SoaRwList *depApps = (*appVer)->Children("DeployedApplication");
						for (SoaRwList::const_iterator depApp = depApps->begin(); depApp != depApps->end(); ++depApp) {

							if ((*depApp)->Ce("ServiceDomain")->Row()->ViewId() == domainId) {

								ItItemMap::iterator depAppIt = appVerIt->second.children.insert(ItItemPair((*depApp)->viewId, ItItem(
									(*depApp)->viewTs,
									(*depApp)->selId == selId,
									(*depApp)->Name(),
									true, externalOrg ? iiiApp2 : iiiApp
								))).first;

								const SoaRwList *dServices = (*depApp)->Children("DeployedService");
								for (SoaRwList::const_iterator dService = dServices->begin(); dService != dServices->end(); ++dService) {

									SoaRwList filteredEndpoints;

									const SoaRwList *endpoints = (*dService)->Children("Endpoint");
									for (SoaRwList::const_iterator e = endpoints->begin(); e != endpoints->end(); ++e) {

										SoaRw *fDomain = (*e)->Ce("ServiceDomain")->Row();
										if (fDomain && fDomain->ViewId() == domainId)
											filteredEndpoints.push_back(*e);
									}

									if (filteredEndpoints.size()) {

										ItItemMap::iterator dServiceIt = depAppIt->second.children.insert(ItItemPair((*dService)->viewId, ItItem(
											(*dService)->viewTs,
											(*dService)->selId == selId,
											(*dService)->Name(),
											true, externalOrg ? iiiServ2 : iiiServ
										))).first;

										for (SoaRwList::iterator e = filteredEndpoints.begin(); e != filteredEndpoints.end(); ++e) {

											dServiceIt->second.children.insert(ItItemPair((*e)->viewId, ItItem(
												(*e)->viewTs,
												(*e)->selId == selId,
												(*e)->Name(),
												true, externalOrg ? iiiEnd2 : iiiEnd
											)));
										}
									}
								}
							}
						}
					}
				}
			}
		}

		externalOrg = true;
	}

	return itemMap;
}

//
// table
//

ItTable SoaDb::ItemTable(int tbIndex) {
	ItTable table;

	if ((unsigned int)tbIndex < size()) {

		SoaTb *tb = at(tbIndex);
		table.name = tb->Name();

		if (tb->tableColumnHints.size()) {

			table.styles = tb->ClStyles();
			table.columns = tb->ClNames();

			for (SoaRwList::iterator row = tb->begin(); row != tb->end(); ++row) {

				table.rows.insert(ItRowPair((*row)->viewId, ItRow(
					(*row)->viewTs,
					(*row)->selId == selId,
					(*row)->ToStrings(tb->tableColumnHints)
				)));
			}
		}
		else {

			for (SoaClVector::const_iterator cl = ++++tb->Columns().begin(); cl != tb->Columns().end(); ++cl)
				table.columns.push_back(cl->DisplayName());

			for (SoaClVector::const_iterator cl = tb->AuxColumns().begin(); cl != tb->AuxColumns().end(); ++cl)
				table.columns.push_back(cl->DisplayName());

			table.styles = std::vector<int>(table.columns.size(), tcsDefault);

			for (SoaRwList::iterator row = tb->begin(); row != tb->end(); ++row) {

				table.rows.insert(ItRowPair((*row)->viewId, ItRow(
					(*row)->viewTs,
					(*row)->selId == selId,
					(*row)->ToStrings(tb->tableColumnHints)
				)));
			}
		}
	}

	return table;
}

//
// inspector
//

InItem SoaDb::Inspectee() {
	if (selTb == 0)
		return InItem(0, "Properties");

	SoaRw *row = selTb->FocusRw();
	if (row)
		return InItem(row, "Properties [" + row->tb->Name() + "]");
	else
		return InItem(0, "Properties");
}

//
// intermediary
//

GtDomain SoaDb::Services(unsigned int domainId) {

	GtDomain services;

	SoaTb *deplServs = Tb("DeployedService");
	for (SoaRwList::iterator s = deplServs->begin(); s != deplServs->end(); ++s) {

		GtService gs((*s)->ViewId(), (*s)->selId == selId, (*s)->Ce("ServiceVersion")->Row()->Ce("Service")->Row()->Ce("name")->Txt());

		const SoaRwList *endpnts = (*s)->Children("Endpoint");
		for (SoaRwList::const_iterator e = endpnts->begin(); e != endpnts->end(); ++e) {

			if ((*e)->Name() != DEFAULT_ENDPOINT_NAME && (*e)->Ce("Intermediary")->Row()) {

				SoaRw *dom = (*e)->Ce("ServiceDomain")->Row();
				if (dom && dom->ViewId() == domainId) {

					gs.push_back(GtEndpoint((*e)->ViewId(), (*e)->selId == selId, (*e)->Ce("intermediaryGivenId")->Txt()));
				}
			}
		}

		if (gs.size())
			services.push_back(gs);
	}

	return services;
}

//
// selected domain
//

unsigned int SoaDb::SelectedDomain() {

	SoaTb *dom = Tb("ServiceDomain");
	if (dom->size() == 0)
		return IV_NULL_DOMAIN;

	SoaRw *row = dom->FocusRw();
	if (row)
		return row->ViewId();

	dom->InvertRowSelection(dom->front()->ViewId(), selId);

	return dom->front()->ViewId();
}

std::string SoaDb::SelectedDomainName() {

	SoaTb *dom = Tb("ServiceDomain");
	SoaRw *d = dom->RwByViewId(SelectedDomain());

	return d ? d->Ce("name")->Txt() : "";
}

void SoaDb::SelectDomain(unsigned int viewId) {

	SoaTb *dom = Tb("ServiceDomain");

	DeselectAll();
	dom->InvertRowSelection(viewId, selId);
}

//
// other
//

std::string SoaDb::TbNameFromViewId(unsigned int viewId) {
	int tbIndex = viewId % 100;
	return (tbIndex < (int)size()) ? at(tbIndex)->name : "";
}

SoaTb *SoaDb::TbFromViewId(unsigned int viewId) {
	int tbIndex = viewId % 100;
	return (tbIndex < (int)size()) ? at(tbIndex) : 0;
}

MgIntermediaryMap SoaDb::IntermediaryMap(int domainId) {

	MgIntermediaryMap m;

	if (domainId != -1) {

		SoaRw *dom = Tb("ServiceDomain")->RwByViewId(domainId);
		if (dom) {

			const SoaRwList *ints = dom->Children("Intermediary");
			if (ints) {

				for (SoaRwList::const_iterator i = ints->begin(); i != ints->end(); ++i)
					if ((*i)->Name() != DEFAULT_INTERMEDIARY_NAME)
						m.insert(MgIntermediaryPair((*i)->ViewId(), MgIntermediary((*i)->Ce("name")->Txt())));
			}
		}
	}
	else {

		SoaTb *ints = Tb("Intermediary");
		for (SoaTb::iterator i = ints->begin(); i != ints->end(); ++i) {

			if ((*i)->Name() != DEFAULT_INTERMEDIARY_NAME) {

				SoaRw *reg = (*i)->Ce("RuntimeRegistry")->Row();

				m.insert(MgIntermediaryPair((*i)->ViewId(),
											MgIntermediary((*i)->Ce("name")->Txt(),
														   (*i)->Ce("ServiceDomain")->Row()->Ce("name")->Txt(),
														   reg ? reg->ViewId() : -1)));
			}
		}
	}

	return m;
}

MgRegistryMap SoaDb::RegistryMap() {

	MgRegistryMap m;

	SoaTb *regs = Tb("RuntimeRegistry");
	for (SoaTb::iterator r = regs->begin(); r != regs->end(); ++r) {

		MgRegistry rg((*r)->Ce("name")->Txt());

		const SoaRwList *ints = (*r)->Children("Intermediary");
		if (ints) {

			for (SoaRwList::const_iterator i = ints->begin(); i != ints->end(); ++i)
				rg.intermediaries.insert(MgIntermediaryPair((*i)->ViewId(), MgIntermediary((*i)->Ce("name")->Txt())));
		}

		m.insert(MgRegistryPair((*r)->ViewId(), rg));
	}

	return m;
}
