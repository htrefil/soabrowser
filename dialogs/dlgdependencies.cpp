#include "dialogs/dlgdependencies.h"
#include "database/soatable.h"
#include "qs.h"
#include <QTreeWidget>



DlgDependencies::DlgDependencies(QWidget *p, SoaRw *r) : QDialog(p) {

	ui.setupUi(this);
	setWindowTitle("Dependencies");

	ui.tree->setHeaderLabels(QStringList() << "Item" << "Type");

	AddDependencyItems(new QTreeWidgetItem(ui.tree), r);

	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(OkSlot()));
	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(CancelSlot()));
}

void DlgDependencies::AddDependencyItems(QTreeWidgetItem *i, SoaRw *r) {

	i->setText(0, QS(r->Name()));
	i->setText(1, QS(r->Tb()->Name()));

	SoaRwList l = r->GetDependencies();
	for (SoaRwList::iterator d = l.begin(); d != l.end(); ++d)
		AddDependencyItems(new QTreeWidgetItem(i), *d);
}

void DlgDependencies::OkSlot() {
	done(1);
}

void DlgDependencies::CancelSlot() {
	done(0);
}
