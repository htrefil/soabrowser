#include "soawidget.h"
#include "dialogs/dlgnewasset.h"
#include "dialogs/dlgintermediaries.h"
#include "dialogs/dlgregistries.h"
#include "dialogs/dlgloadregistry.h"
#include "dialogs/dlgchooseasset.h"
#include "dialogs/dlgdependencies.h"
#include <QAction>
#include <QMenu>



void SoaWg::CreateActions() {

	AddAction("Add use", SLOT(AddUseSlot()));
	AddAction("Delete use", SLOT(DeleteUseSlot()));
	AddAction("Add organization", SLOT(AddOrganizationSlot()));
	AddAction("Add business unit", SLOT(AddBusinessUnitSlot()));
	AddAction("Add service domain", SLOT(AddServiceDomainSlot()));
	AddAction("Add application", SLOT(AddApplicationSlot()));
	AddAction("Add application version", SLOT(AddApplicationVersionSlot()));
	AddAction("Add service", SLOT(AddServiceSlot()));
	AddAction("Add service version", SLOT(AddServiceVersionSlot()));

	AddAction("Delete organization", SLOT(DeleteOrganizationSlot()));
	AddAction("Delete business unit", SLOT(DeleteBusinessUnitSlot()));
	AddAction("Delete service domain", SLOT(DeleteServiceDomainSlot()));
	AddAction("Delete application", SLOT(DeleteApplicationSlot()));
	AddAction("Delete application version", SLOT(DeleteApplicationVersionSlot()));
	AddAction("Delete deployed application", SLOT(DeleteDeployedApplicationSlot()));
	AddAction("Delete service", SLOT(DeleteServiceSlot()));
	AddAction("Delete service version", SLOT(DeleteServiceVersionSlot()));
	AddAction("Delete deployed service", SLOT(DeleteDeployedServiceSlot()));
	AddAction("Delete endpoint", SLOT(DeleteEndpointSlot()));
}

void SoaWg::RefreshActions() {

	bool isOrganizationSeld = db.Tb("Organization")->FocusRw() != 0;
	bool isBusinessUnitSeld = db.Tb("BusinessUnit")->FocusRw() != 0;
	bool isApplicationSeld = db.Tb("Application")->FocusRw() != 0;
	bool isUseSelected = false;
	bool enableAddUse = false;

	// actions sensitive to mode

	if (mode == fmOutline) {
	}
	else if (mode == fmRealization) {

		SoaRw *app = db.Tb("ApplicationVersion")->FocusRw();
		SoaRw *ser = db.Tb("ServiceVersion")->FocusRw();

		if (app && ser && ser->Ce("ApplicationVersion")->Row() != app) {

			enableAddUse = true;

			const SoaRwList *uses = app->Children("DesignTimeServiceUse");
			for (SoaRwList::const_iterator use = uses->begin(); use != uses->end(); ++use) {

				if ((*use)->Ce("ServiceVersion")->Row() == ser) {
					enableAddUse = false;
					break;
				}
			}
		}
		else
			GetAction("Add use")->setEnabled(false);

		isUseSelected = db.Tb("DesignTimeServiceUse")->FocusRw() != 0;
	}
	else if (mode == fmRuntime) {

		SoaRw *app = db.Tb("DeployedApplication")->FocusRw();
		SoaRw *ser = db.Tb("DeployedService")->FocusRw();

		// if both are focused (selected) and application doesn't implement that specific service,
		// and the servie has one or more endpoints

		if (app && ser && ser->Ce("DeployedApplication")->Row() != app && ser->Children("Endpoint")->size()) {

			enableAddUse = true;

			// check that the application doesn't already use the deployed service

			const SoaRwList *uses = app->Children("RuntimeServiceUse");
			for (SoaRwList::const_iterator use = uses->begin(); use != uses->end(); ++use) {

				SoaRw *endp = (*use)->Ce("Endpoint")->Row();
				if (endp && endp->Ce("DeployedService")->Row() == ser) {
					enableAddUse = false;
					break;
				}
			}
		}

		isUseSelected = db.Tb("RuntimeServiceUse")->FocusRw() != 0;
	}

	// actions insensitive to mode

	GetAction("Add use")->setEnabled(enableAddUse);
	GetAction("Delete use")->setEnabled(isUseSelected);
	GetAction("Add business unit")->setEnabled(isOrganizationSeld);
	GetAction("Add service domain")->setEnabled(isOrganizationSeld);
	GetAction("Add application")->setEnabled(isBusinessUnitSeld);
	GetAction("Add application version")->setEnabled(isApplicationSeld);
	infrastructureAction->setEnabled(mode == fmRuntime);
}

QAction *SoaWg::AddAction(const QString &text, const char *slot) {

	QAction *a = new QAction(text, this);

	addAction(a);
	connect(a, SIGNAL(triggered()), this, slot);
	actionsHash.insert(text, a);

	return a;
}

QAction *SoaWg::GetAction(const QString &text) {
	return actionsHash[text];
}

void SoaWg::RefreshDomainsMenu() {

	domainsMenu->clear();

	MgRegistryMap rm = db.RegistryMap();
	for (MgRegistryMap::iterator r = rm.begin(); r != rm.end(); ++r) {

		QAction *ra = domainsMenu->addAction("Load registry \"" + QString::fromStdString(r->second.name) + "\"");
		ra->setData(r->first);
	}

	domainsMenu->addSeparator();
	domainsMenu->addAction(manageIntermediaries);
	domainsMenu->addAction(manageRegistries);
}

//
// slots
//

void SoaWg::AddUseSlot() {

	if (mode == fmRealization) {

		SoaRw *app = db.Tb("ApplicationVersion")->FocusRw();
		SoaRw *ser = db.Tb("ServiceVersion")->FocusRw();

		if (app && ser) {

			SoaTb *tb = db.Tb("DesignTimeServiceUse");
			SoaRw *rw = tb->AllocRw(-1, true);
			DlgNewAsset dlg(this, this, tb, rw);
			if (dlg.exec()) {
				tb->AddRw(dlg.Row());
				RefreshViews();
			}
			else
				delete rw;
		}
	}
	else if (mode == fmRuntime) {

		SoaRw *app = db.Tb("DeployedApplication")->FocusRw();
		SoaRw *ser = db.Tb("DeployedService")->FocusRw();

		if (app && ser) {

			SoaRw *endpoint = 0;
			const SoaRwList *endpoints = ser->Children("Endpoint");
			if (endpoints->size() == 1)
				endpoint = endpoints->front();
			else if (endpoints->size() > 1) {

				DlgChooseAsset dlg(this, endpoints, "Choose endpoint");
				if (dlg.exec())
					endpoint = dlg.SelectedRow();
			}

			if (endpoint) {

				SoaTb *rsus = db.Tb("RuntimeServiceUse");
				SoaRw *rsu = rsus->AllocRw(-1, false);
				rsu->SetCe("Endpoint", endpoint);
				rsu->SetCe("DeployedApplication", app);
				DlgNewAsset useDlg(this, this, rsus, rsu);
				if (useDlg.exec()) {

					rsus->AddRw(useDlg.Row());

					// add design time service use also

					SoaTb *dsus = db.Tb("DesignTimeServiceUse");
					dsus->Search().
							Wt("ServiceVersion", ser->Ce("ServiceVersion")->Row()).
							Wt("ApplicationVersion", app->Ce("ApplicationVersion")->Row()).
							Get(dsus, true);

					RefreshViews();
				}
				else
					delete rsu;
			}
		}
	}
}

void SoaWg::DeleteUseSlot() {

	if (mode == fmRealization) {

		SoaRw *use = db.Tb("DesignTimeServiceUse")->FocusRw();
		if (use) {
			db.Tb("DesignTimeServiceUse")->DeleteRowByViewId(use->ViewId());
			RefreshViews();
		}
	}
	else if (mode == fmRuntime) {

		SoaRw *use = db.Tb("RuntimeServiceUse")->FocusRw();
		if (use) {
			db.Tb("RuntimeServiceUse")->DeleteRowByViewId(use->ViewId());
			RefreshViews();
		}
	}
}

void SoaWg::AddOrganizationSlot() {

	SoaTb *tb = db.Tb("Organization");
	SoaRw *rw = tb->AllocRw(-1, true);
	DlgNewAsset dlg(this, this, tb, rw);
	if (dlg.exec()) {
		tb->AddRw(dlg.Row());
		RefreshViews();
	}
	else
		delete rw;
}

void SoaWg::AddBusinessUnitSlot() {

	SoaTb *tb = db.Tb("BusinessUnit");
	SoaRw *rw = tb->AllocRw(-1, true);
	DlgNewAsset dlg(this, this, tb, rw);
	if (dlg.exec()) {
		tb->AddRw(dlg.Row());
		RefreshViews();
	}
	else
		delete rw;
}

void SoaWg::AddServiceDomainSlot() {

	SoaTb *tb = db.Tb("ServiceDomain");
	SoaRw *rw = tb->AllocRw(-1, true);
	DlgNewAsset dlg(this, this, tb, rw);
	if (dlg.exec()) {

		tb->AddRw(dlg.Row());

		SoaTb *intermediaries = db.Tb("Intermediary");
		intermediaries->Search().
				Wt("name", DEFAULT_INTERMEDIARY_NAME).
				Wt("ServiceDomain", dlg.Row()).
				Get(intermediaries, true);

		RefreshViews();
	}
	else
		delete rw;
}

void SoaWg::AddApplicationSlot() {

	SoaTb *tb = db.Tb("Application");
	SoaRw *rw = tb->AllocRw(-1, true);
	DlgNewAsset dlg(this, this, tb, rw);
	if (dlg.exec()) {
		tb->AddRw(dlg.Row());
		RefreshViews();
	}
	else
		delete rw;
}

void SoaWg::AddApplicationVersionSlot() {

	SoaTb *tb = db.Tb("ApplicationVersion");
	SoaRw *rw = tb->AllocRw(-1, true);
	DlgNewAsset dlg(this, this, tb, rw);
	if (dlg.exec()) {
		tb->AddRw(dlg.Row());
		RefreshViews();
	}
	else
		delete rw;
}

void SoaWg::AddServiceSlot() {

	SoaTb *tb = db.Tb("Service");
	SoaRw *rw = tb->AllocRw(-1, true);
	DlgNewAsset dlg(this, this, tb, rw);
	if (dlg.exec()) {
		tb->AddRw(dlg.Row());
		RefreshViews();
	}
	else
		delete rw;
}

// Assume this has been called with an
// application version selected.
void SoaWg::AddServiceVersionSlot() {

	SoaRw *appVer = db.Tb("ApplicationVersion")->FocusRw();
	if (appVer == 0)
		return;

	SoaRw *app = appVer->Ce("Application")->Row();
	if (app == 0)
		return;

	DlgChooseAsset picker(this, app->Children("Service"), "Pick service");
	if (picker.exec()) {

		db.Tb("Service")->SetFocusedRwByViewId(picker.SelectedRow()->ViewId());

		SoaTb *tb = db.Tb("ServiceVersion");
		SoaRw *rw = tb->AllocRw(-1, true);
		DlgNewAsset dlg(this, this, tb, rw);
		if (dlg.exec()) {
			tb->AddRw(dlg.Row());
			RefreshViews();
		}
		else
			delete rw;
	}
}

void SoaWg::DeleteOrganizationSlot() {

	SoaRw *rw = db.Tb("Organization")->FocusRw();
	if (rw == 0 || rw == db.Tb("Organization")->front())
		return;

	db.Tb("Organization")->DeleteRowByViewId(rw->ViewId());
	RefreshViews();
}

void SoaWg::DeleteBusinessUnitSlot() {

	SoaRw *rw = db.Tb("BusinessUnit")->FocusRw();
	if (rw == 0)
		return;

	db.Tb("BusinessUnit")->DeleteRowByViewId(rw->ViewId());
	RefreshViews();
}

void SoaWg::DeleteApplicationSlot() {
	DeleteAsset(db.Tb("Application"));
}

void SoaWg::DeleteApplicationVersionSlot() {
	DeleteAsset(db.Tb("ApplicationVersion"));
}

void SoaWg::DeleteDeployedApplicationSlot() {
	DeleteAsset(db.Tb("DeployedApplication"));
}

void SoaWg::DeleteServiceSlot() {
	DeleteAsset(db.Tb("Service"));
}

void SoaWg::DeleteServiceVersionSlot() {
	DeleteAsset(db.Tb("ServiceVersion"));
}

void SoaWg::DeleteDeployedServiceSlot() {
	DeleteAsset(db.Tb("DeployedService"));
}

void SoaWg::DeleteEndpointSlot() {
	DeleteAsset(db.Tb("Endpoint"));
}

void SoaWg::DeleteAsset(SoaTb *tb) {

	SoaRw *rw = tb->FocusRw();
	if (rw == 0)
		return;

	DlgDependencies dlg(this, rw);
	if (dlg.exec()) {
		tb->DeleteRowByViewId(rw->ViewId());
		RefreshViews();
	}
}

void SoaWg::DomainsMenuSlot(QAction *a) {

	unsigned int viewId = a->data().toUInt();
	std::string tbName = db.TbNameFromViewId(viewId);

	if (tbName == "RuntimeRegistry") {

		DlgLoadRegistry dlg(this, viewId);
		if (dlg.exec())
			OpenRegistryWidget(dlg.Gateway());
	}
}

void SoaWg::ManageIntermediariesSlot() {
	DlgIntermediaries dlg(this, db.Tb("Intermediary"), &db);
	dlg.exec();
	RefreshViews();
}

void SoaWg::ManageRegistriesSlot() {
	DlgRegistries dlg(this);
	dlg.exec();
	RefreshViews();
}
