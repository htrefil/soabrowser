#include "widgets/wdggateway.h"
#include "dialogs/dlgchooseasset.h"
#include "dialogs/dlgnewasset.h"
#include "soawidget.h"
#include "database/soalogger.h"
#include <QMessageBox>
#include <QDesktopServices>
#include <QComboBox>
#include <QCheckBox>



void WdgGateway::ModeChangedSlot(int i) {
	mode = (MergeWidgetMode)cb->itemData(i).toInt();
	Refresh();
}

void WdgGateway::ExistingToggledSlot(bool b) {
	showExisting = b;
	Refresh();
}

void WdgGateway::DeployServiceSlot(const GtService &gtS) {

	SoaDb &db = wg->Db();

	SoaRw *service = 0, *serviceVersion = 0;

	// find service with matching name

	service = db.Tb("Service")->Search().Wt("name", gtS.name.c_str()).Get();

	// if service not found

	bool newServiceVersion = false;
	if (service == 0) {

		if (QMessageBox::information(this,
								 "Registry service deployment",
								 "No service with matching name found.\n\nDo you want to create a new service?\n",
								 QMessageBox::Ok | QMessageBox::Cancel) != QMessageBox::Ok)
			return;

		service = NewService(db.Tb("Service"), gtS.name);

		if (service == 0)
			return;
		else {

			const SoaRwList *applicationVersions = service->Ce("Application")->Row()->Children("ApplicationVersion");

			serviceVersion = NewServiceVersion(db.Tb("ServiceVersion"), applicationVersions, service);
			newServiceVersion = true;

			if (serviceVersion == 0)
				return;
		}
	}

	// if service found

	else {

		const SoaRwList *versions = service->Children("ServiceVersion");
		if (versions->size()) {

			// pick service version

			DlgChooseAsset dlg(this, versions, "Choose service version");
			if (dlg.exec())
				serviceVersion = dlg.SelectedRow();
			else
				return;
		}
		else {

			if (QMessageBox::information(this,
									 "Registry service deployment",
									 "No service versions with matching name were found.\n\nDo you want to create a new service version?\n",
									 QMessageBox::Ok | QMessageBox::Cancel) != QMessageBox::Ok)
					return;

			SoaTb *applicationVersions = db.Tb("ApplicationVersion");

			serviceVersion = NewServiceVersion(db.Tb("ServiceVersion"), applicationVersions, service);
			newServiceVersion = true;
		}
	}

	if (service == 0 || serviceVersion == 0)
		return;

	// find run-time registry & service domain

	SoaRw *runtimeReg = db.Tb("RuntimeRegistry")->RwByViewId(gtDomains.registryId);
	SoaRw *servDomain = db.Tb("ServiceDomain")->RwByViewId(db.SelectedDomain());

	// find intermediary - the one with both runtime registry
	// and service domains matching the selected ones

	SoaTb *intermediaries = db.Tb("Intermediary");
	SoaRw *intermediary = intermediaries->Search().
			Wt("RuntimeRegistry", runtimeReg).
			Wt("ServiceDomain", servDomain).
			Get();
	SoaRw *defIntermediary = intermediaries->Search().
			Wt("name", DEFAULT_INTERMEDIARY_NAME).
			Wt("ServiceDomain", servDomain).
			Get();

	// application version

	SoaRw *appVersion = serviceVersion->Ce("ApplicationVersion")->Row();

	// find deployed application

	SoaTb *deployedApps = db.Tb("DeployedApplication");
	SoaRw *depApp = deployedApps->Search().
			Wt("ServiceDomain", servDomain).
			Wt("ApplicationVersion", appVersion).
			Get();

	SoaTb *deployedServices = db.Tb("DeployedService");
	SoaTb *endpoints = db.Tb("Endpoint");

	// if not found, deploy application, and then deploy all its services on the default intermediary

	if (depApp == 0) {

		depApp = deployedApps->Search().
						Wt("ServiceDomain", servDomain).
						Wt("ApplicationVersion", appVersion).
						Get(deployedApps, true);

		const SoaRwList *servicesToDeploy = appVersion->Children("ServiceVersion");
		for (SoaRwList::const_iterator s = servicesToDeploy->begin(); s != servicesToDeploy->end(); ++s) {

			SoaRw *ds = deployedServices->Search().
					Wt("ServiceVersion", *s).
					Wt("DeployedApplication", depApp).
					Get(deployedServices, true);

			endpoints->Search().
					Wt("intermediaryGivenName", DEFAULT_ENDPOINT_NAME).
					Wt("DeployedService", ds).
					Wt("Intermediary", defIntermediary).
					Wt("ServiceDomain", servDomain).
					Get(endpoints, true);
		}
	}

	// find or create the deployed service

	SoaRw *deployedService = deployedServices->Search().
			Wt("ServiceVersion", serviceVersion).
			Wt("DeployedApplication", depApp).
			Get();

	if (deployedService == 0) {

		deployedService = deployedServices->Search().
						Wt("ServiceVersion", serviceVersion).
						Wt("DeployedApplication", depApp).
						Get(deployedServices, true);

		endpoints->Search().
				Wt("intermediaryGivenName", DEFAULT_ENDPOINT_NAME).
				Wt("DeployedService", deployedService).
				Wt("Intermediary", defIntermediary).
				Wt("ServiceDomain", servDomain).
				Get(endpoints, true);
	}

	// fill service with new data

	deployedService->SetCe("targetNameSpace", gtS.targetNamespace);
	deployedService->SetCe("definitionName", gtS.definitionName);

	// create its new endpoints

	for (GtService::const_iterator e = gtS.begin(); e != gtS.end(); ++e) {

		endpoints->Search().
				Wt("ServiceDomain", servDomain).
				Wt("DeployedService", deployedService).
				Wt("Intermediary", intermediary).
				Wt("intermediaryGivenName", e->intGivenName.c_str()).
				Wt("intermediaryGivenVersion", e->intGivenVersion.c_str()).
				Wt("intermediaryGivenId", e->intGivenId.c_str()).
				Wt("policyVersion", e->policyVersion.c_str()).
				Wt("resolutionPath", e->resolutionPath.c_str()).
				Wt("enabled", e->enabled.c_str()).
				Wt("soap", e->soap.c_str()).
				Wt("urlWSDL", e->urlWSDL.c_str()).
				Wt("urlRegistryFolder", e->urlRegistryFolder.c_str()).
				Get(endpoints, true);
	}

	// if a new service version was created, deploy the
	// service on all domains the deployed application is
	// deployed on

	if (newServiceVersion) {

		const SoaRwList *depApps = appVersion->Children("DeployedApplication");
		for (SoaRwList::const_iterator a = depApps->begin(); a != depApps->end(); ++a) {

			if (*a != depApp) {	// skip the app on the selected domain

				// find or create the deployed service

				SoaRw *deployedService = deployedServices->Search().
						Wt("ServiceVersion", serviceVersion).
						Wt("DeployedApplication", *a).
						Get();

				if (deployedService == 0) {

					deployedService = deployedServices->Search().
									Wt("ServiceVersion", serviceVersion).
									Wt("DeployedApplication", *a).
									Get(deployedServices, true);

					SoaRw *servDom = (*a)->Ce("ServiceDomain")->Row();

					SoaRw *defInter = intermediaries->Search().
							Wt("name", DEFAULT_INTERMEDIARY_NAME).
							Wt("ServiceDomain", servDom).
							Get();

					endpoints->Search().
							Wt("intermediaryGivenName", DEFAULT_ENDPOINT_NAME).
							Wt("DeployedService", deployedService).
							Wt("Intermediary", defInter).
							Wt("ServiceDomain", servDom).
							Get(endpoints, true);
				}
			}
		}
	}

	wg->Db().DeselectAll();
	wg->Db().InvertRowSelection(deployedService->ViewId());
	wg->RefreshViews();
}

SoaRw *WdgGateway::NewService(SoaTb *services, const std::string &name) {

	SoaRw *service = services->AllocRw(-1, false);

	service->SetCe("name", SoaCe(name.c_str()));

	DlgNewAsset dlg(this, wg, services, service);
	if (dlg.exec()) {
		services->AddRw(service);
		return service;
	}
	else
		delete service;

	return 0;
}

SoaRw *WdgGateway::NewServiceVersion(SoaTb *serviceVersions, const SoaRwList *applicationVersions, SoaRw *service) {

	DlgChooseAsset chooser(this, applicationVersions, "Choose application version");
	if (chooser.exec()) {

		SoaRw *serviceVersion = serviceVersions->AllocRw(-1, false);

		serviceVersion->SetCe("Service", SoaCe(service));
		serviceVersion->SetCe("ApplicationVersion", SoaCe(chooser.SelectedRow()));

		DlgNewAsset dlg(this, wg, serviceVersions, serviceVersion);
		if (dlg.exec()) {
			serviceVersions->AddRw(serviceVersion);
			return serviceVersion;
		}
		else
			delete serviceVersion;
	}

	return 0;
}

//
// deploying endpoints
//

void WdgGateway::DeployEndpointSlot(const GtService &gtS, const GtEndpoint &gtE) {

	SoaDb &db = wg->Db();

	SoaRw *deployedService = db.Tb("DeployedService")->RwByViewId(gtS.viewId);
	if (deployedService == 0) {
		sl::log << "WdgGateway::DeployEndpointSlot() error: no deployed service with matching id found" << sl::end;
		return;
	}

	SoaTb *endpoints = db.Tb("Endpoint");

	// find run-time registry & service domain

	SoaRw *runtimeReg = db.Tb("RuntimeRegistry")->RwByViewId(gtDomains.registryId);
	SoaRw *servDomain = db.Tb("ServiceDomain")->RwByViewId(db.SelectedDomain());

	// find intermediary - the one with both runtime registry
	// and service domains matching the selected ones

	SoaTb *intermediaries = db.Tb("Intermediary");
	SoaRw *intermediary = intermediaries->Search().
			Wt("RuntimeRegistry", runtimeReg).
			Wt("ServiceDomain", servDomain).
			Get();

	endpoints->Search().
			Wt("ServiceDomain", servDomain).
			Wt("DeployedService", deployedService).
			Wt("Intermediary", intermediary).
			Wt("intermediaryGivenName", gtE.intGivenName.c_str()).
			Wt("intermediaryGivenVersion", gtE.intGivenVersion.c_str()).
			Wt("intermediaryGivenId", gtE.intGivenId.c_str()).
			Wt("policyVersion", gtE.policyVersion.c_str()).
			Wt("resolutionPath", gtE.resolutionPath.c_str()).
			Wt("enabled", gtE.enabled.c_str()).
			Wt("soap", gtE.soap.c_str()).
			Wt("urlWSDL", gtE.urlWSDL.c_str()).
			Wt("urlRegistryFolder", gtE.urlRegistryFolder.c_str()).
			Get(endpoints, true);
}

//
// iterators
//

void WdgGateway::SelectServiceSlot(const GtService &s) {

	wg->Db().DeselectAll();
	wg->Db().InvertRowSelection(s.viewId);
	wg->RefreshViews();
}

void WdgGateway::SelectEndpointSlot(const GtEndpoint &e) {

	wg->Db().DeselectAll();
	wg->Db().InvertRowSelection(e.viewId);
	wg->RefreshViews();
}

void WdgGateway::BrowseEndpointIndexSlot(const GtEndpoint &e) {

	QDesktopServices::openUrl(QUrl(QS(e.urlRegistryFolder + "index.html")));
}
