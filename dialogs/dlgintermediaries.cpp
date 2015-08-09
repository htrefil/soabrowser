#include "dialogs/dlgintermediaries.h"
#include "dialogs/dlgnewasset.h"
#include "interfaces/iviewable.h"
#include "database/soatable.h"
#include "soawidget.h"



DlgIntermediaries::DlgIntermediaries(SoaWg *_wg, SoaTb *_tb, IViewable *_iv) :
	QDialog(_wg),
	wg(_wg), tb(_tb), iv(_iv) {

	ui.setupUi(this);
	setWindowTitle("Intermediaries");

	ui.tree->setHeaderLabels(QStringList() << "Name");

	connect(ui.addButton, SIGNAL(clicked()), this, SLOT(AddSlot()));
	connect(ui.editButton, SIGNAL(clicked()), this, SLOT(EditSlot()));
	connect(ui.removeButton, SIGNAL(clicked()), this, SLOT(RemoveSlot()));
	connect(ui.closeButton, SIGNAL(clicked()), this, SLOT(CloseSlot()));
	connect(ui.tree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(StateSlot()));
	connect(ui.tree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(EditSlot()));

	Refresh();
}

void DlgIntermediaries::Refresh() {

	MgIntermediaryMap m = iv->IntermediaryMap(iv->SelectedDomain());

	ui.tree->clear();

	for (MgIntermediaryMap::iterator i = m.begin(); i != m.end(); ++i) {

		QTreeWidgetItem *it = new QTreeWidgetItem(ui.tree, QStringList() << QString::fromStdString(i->second.name));
		it->setData(0, Qt::UserRole, i->first);
	}

	if (ui.tree->topLevelItemCount())
		ui.tree->setCurrentItem(ui.tree->topLevelItem(0));

	StateSlot();
}

void DlgIntermediaries::StateSlot() {

	bool selected = ui.tree->currentItem() != 0;

	ui.editButton->setEnabled(selected);
	ui.removeButton->setEnabled(selected);
}

void DlgIntermediaries::AddSlot() {

	SoaRw *rw = tb->AllocRw(-1, true);
	DlgNewAsset dlg(this, wg, tb, rw);
	if (dlg.exec()) {
		tb->AddRw(dlg.Row());
		Refresh();
	}
	else
		delete rw;
}

void DlgIntermediaries::EditSlot() {

	DlgNewAsset dlg(this, wg, tb, tb->RwByViewId(ui.tree->currentItem()->data(0, Qt::UserRole).toUInt()));
	if (dlg.exec())
		Refresh();
}

void DlgIntermediaries::RemoveSlot() {

	if (ui.tree->currentItem()) {
		tb->DeleteRowByViewId(ui.tree->currentItem()->data(0, Qt::UserRole).toUInt());
		Refresh();
	}
}

void DlgIntermediaries::CloseSlot() {
	done(0);
}
