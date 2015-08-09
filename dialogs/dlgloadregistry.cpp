#include "dialogs/dlgloadregistry.h"
#include "soawidget.h"



DlgLoadRegistry::DlgLoadRegistry(SoaWg *_wg, unsigned int _regId) : QDialog(_wg),
	wg(_wg),
	regId(_regId),
	loading(false) {

	ui.setupUi(this);
	setWindowTitle("Load run-time registry");

	ui.mergeButton->setEnabled(false);

	connect(ui.loadButton, SIGNAL(clicked()), this, SLOT(LoadSlot()));
	connect(&loader, SIGNAL(DoneSignal(QString)), this, SLOT(DoneSlot(QString)));
	connect(&loader, SIGNAL(UpdateSignal(QString)), this, SLOT(UpdateSlot(QString)));
	connect(ui.mergeButton, SIGNAL(clicked()), this, SLOT(MergeSlot()));
}

SoaGtwy *DlgLoadRegistry::Gateway() {
	return &loader;
}

//
// slots
//

void DlgLoadRegistry::LoadSlot() {

	if (!loading) {

		ui.textBrowser->clear();

		// create domain names list

		QStringList domainNames;

		SoaTb *doms = wg->Db().Tb("ServiceDomain");
		for (SoaTb::iterator d = doms->begin(); d != doms->end(); ++d)
			domainNames << QString::fromStdString((*d)->Ce("name")->Txt());

		SoaRw *reg = wg->Db().Tb("RuntimeRegistry")->RwByViewId(regId);
		if (reg) {

			loader.Load(
						reg->ViewId(),
						QS(reg->Name()),
						QS(reg->Ce("url")->Txt()),
						QS(reg->Ce("username")->Txt()),
						ui.passwordEdit->text(),
						QS(reg->Ce("securityDomain")->Txt()),
						domainNames);

			loading = true;

			ui.passwordEdit->setEnabled(false);
			ui.mergeButton->setEnabled(false);
			ui.loadButton->setText("Cancel");
		}
	}
	else
		loader.Cancel();
}

void DlgLoadRegistry::UpdateSlot(const QString &text) {
	ui.textBrowser->append(text);
}

void DlgLoadRegistry::DoneSlot(const QString &text) {

	ui.textBrowser->append(text);

	loading = false;

	ui.passwordEdit->setEnabled(true);
	ui.mergeButton->setEnabled(loader.Loaded());

	ui.loadButton->setText("Load");
}

void DlgLoadRegistry::MergeSlot() {
	done(1);
}
