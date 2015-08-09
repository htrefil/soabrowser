#include "dialogs/dlgchooseasset.h"
#include "database/soarow.h"
#include "qs.h"



DlgChooseAsset::DlgChooseAsset(QWidget *_p, const SoaRwList *_l, const QString &title) : QDialog(_p),
	list(_l) {

	ui.setupUi(this);
	ui.tree->setHeaderHidden(true);

	setWindowTitle(title);

	for (SoaRwList::const_iterator r = list->begin(); r != list->end(); ++r) {

		QTreeWidgetItem *i = new QTreeWidgetItem(ui.tree, QStringList() << QS((*r)->Name()));
		i->setData(0, Qt::UserRole, (*r)->ViewId());
		i->setData(0, Qt::UserRole + 1, (qulonglong)(*r));
	}

	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(OkSlot()));
	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(CancelSlot()));
	connect(ui.tree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(SelectionChangedSlot()));

	if (ui.tree->topLevelItemCount())
		ui.tree->setCurrentItem(ui.tree->topLevelItem(0));
}

unsigned int DlgChooseAsset::SelectedId() const {
	return (ui.tree->selectedItems().count() > 0) ? ui.tree->selectedItems()[0]->data(0, Qt::UserRole).toUInt() : 10000000;
}

SoaRw *DlgChooseAsset::SelectedRow() const {
	return ui.tree->selectedItems().count() ? (SoaRw *)ui.tree->selectedItems()[0]->data(0, Qt::UserRole + 1).toULongLong() : 0;
}

void DlgChooseAsset::OkSlot() {
	return done(1);
}

void DlgChooseAsset::CancelSlot() {
	return done(0);
}

void DlgChooseAsset::SelectionChangedSlot() {
	ui.okButton->setEnabled(ui.tree->currentItem() != 0);
}
