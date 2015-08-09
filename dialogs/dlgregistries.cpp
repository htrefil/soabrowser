#include "dialogs/dlgregistries.h"
#include "dialogs/dlgnewasset.h"
#include "soawidget.h"



DlgRegistries::DlgRegistries(SoaWg *_wg) :
	QDialog(_wg),
	wg(_wg),
	guard(false) {

	ui.setupUi(this);
	setWindowTitle("Run-time registries");

	ui.tree->setHeaderLabels(QStringList() << "Name");
	ui.intermediaryTree->setHeaderLabels(QStringList() << "Name");

	connect(ui.addButton, SIGNAL(clicked()), this, SLOT(AddSlot()));
	connect(ui.editButton, SIGNAL(clicked()), this, SLOT(EditSlot()));
	connect(ui.removeButton, SIGNAL(clicked()), this, SLOT(RemoveSlot()));
	connect(ui.closeButton, SIGNAL(clicked()), this, SLOT(CloseSlot()));
	connect(ui.tree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(StateSlot()));
	connect(ui.tree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(EditSlot()));
	connect(ui.intermediaryTree, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(IntermediaryItemChangedSlot(QTreeWidgetItem*)));

	LoadIntermediaries();
	Refresh();
}

void DlgRegistries::Refresh() {

	guard = true;

	MgRegistryMap m = wg->Db().RegistryMap();

	ui.tree->clear();

	for (MgRegistryMap::iterator r = m.begin(); r != m.end(); ++r) {

		QTreeWidgetItem *it = new QTreeWidgetItem(ui.tree, QStringList() << QString::fromStdString(r->second.name));
		it->setData(0, Qt::UserRole, r->first);
	}

	if (ui.tree->topLevelItemCount())
		ui.tree->setCurrentItem(ui.tree->topLevelItem(0));

	guard = false;

	StateSlot();
}

//
// intermediaries
//

void DlgRegistries::LoadIntermediaries() {

	guard = true;

	ui.intermediaryTree->clear();

	MgIntermediaryMap mp = wg->Db().IntermediaryMap(-1);
	for (MgIntermediaryMap::iterator ip = mp.begin(); ip != mp.end(); ++ip) {

		QTreeWidgetItem *i = new QTreeWidgetItem(ui.intermediaryTree,
												 QStringList() <<
												 QString::fromStdString(ip->second.name) <<
												 QString::fromStdString(ip->second.sdName));

		i->setData(0, Qt::UserRole, ip->first);
	}

	guard = false;
}

void DlgRegistries::UpdateIntermediaries() {

	guard = true;

	int regId = ui.tree->currentItem() ? (int)ui.tree->currentItem()->data(0, Qt::UserRole).toUInt() : -1;

	MgIntermediaryMap mp = wg->Db().IntermediaryMap(-1);

	for (int i = 0; i < ui.intermediaryTree->topLevelItemCount(); ++i) {

		QTreeWidgetItem *item = ui.intermediaryTree->topLevelItem(i);

		MgIntermediaryMap::iterator ip = mp.find(item->data(0, Qt::UserRole).toUInt());
		if (ip != mp.end()) {

			item->setData(0, Qt::UserRole + 1, ip->second.registry);

			if (regId == -1) {
				item->setDisabled(true);
				item->setCheckState(0, Qt::Unchecked);
			}
			else {

				if (ip->second.registry == -1) {
					item->setDisabled(false);
					item->setCheckState(0, Qt::Unchecked);
				}
				else if (ip->second.registry == regId) {
					item->setDisabled(false);
					item->setCheckState(0, Qt::Checked);
				}
				else {
					item->setDisabled(true);
					item->setCheckState(0, Qt::Unchecked);
				}
			}
		}
	}

	guard = false;
}

//
// slots
//

void DlgRegistries::IntermediaryItemChangedSlot(QTreeWidgetItem *i) {

	if (guard)
		return;

	if (i) {

		SoaRw *intm = wg->Db().Tb("Intermediary")->RwByViewId(i->data(0, Qt::UserRole).toUInt());
		if (intm) {

			if (i->checkState(0) == Qt::Checked) {

				QTreeWidgetItem *ri = ui.tree->currentItem();
				if (ri) {

					SoaRw *reg = wg->Db().Tb("RuntimeRegistry")->RwByViewId(ri->data(0, Qt::UserRole).toUInt());
					if (reg)
						intm->SetPr("RuntimeRegistry", reg);
				}
			}
			else
				intm->SetPr("RuntimeRegistry", 0);

			UpdateIntermediaries();
		}
	}
}

void DlgRegistries::StateSlot() {

	bool selected = ui.tree->currentItem() != 0;

	ui.editButton->setEnabled(selected);
	ui.removeButton->setEnabled(selected);

	UpdateIntermediaries();
}

void DlgRegistries::AddSlot() {

	SoaTb *tb = wg->Db().Tb("RuntimeRegistry");
	SoaRw *rw = tb->AllocRw(-1, true);
	DlgNewAsset dlg(this, wg, tb, rw);
	if (dlg.exec()) {
		tb->AddRw(dlg.Row());
		Refresh();
	}
	else
		delete rw;
}

void DlgRegistries::EditSlot() {

	SoaTb *tb = wg->Db().Tb("RuntimeRegistry");
	DlgNewAsset dlg(this, wg, tb, tb->RwByViewId(ui.tree->currentItem()->data(0, Qt::UserRole).toUInt()));
	if (dlg.exec())
		Refresh();
}


void DlgRegistries::RemoveSlot() {

	if (ui.tree->currentItem()) {
		wg->Db().Tb("RuntimeRegistry")->DeleteRowByViewId(ui.tree->currentItem()->data(0, Qt::UserRole).toUInt());
		Refresh();
	}
}

void DlgRegistries::CloseSlot() {
	done(0);
}

