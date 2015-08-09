#include "dialogs/dlgnewasset.h"
#include "widgets/wdginspector.h"
#include "database/soatable.h"
#include <QVBoxLayout>



DlgNewAsset::DlgNewAsset(QWidget *_parent, SoaWg *_wg, SoaTb *_tb, SoaRw *_rw) : QDialog(_parent),
	wg(_wg),
	ins(new WdgInspector(this, 0)),
	tb(_tb),
	row(_rw) {

	ins->SetRow(row);

	ui.setupUi(this);
	setWindowTitle(QString::fromStdString(tb->Name()));

	QVBoxLayout *layout = new QVBoxLayout(ui.inspectorWidget);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(ins);

	QObject::connect(ui.okButton, SIGNAL(clicked()), this, SLOT(OkSlot()));
	QObject::connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(CancelSlot()));
	QObject::connect(ins->Delegate(), SIGNAL(modifiedSignal()), this, SLOT(ModifiedSlot()));

	Check();
}

SoaRw *DlgNewAsset::Row() {
	return row;
}

void DlgNewAsset::Check() {
	ui.okButton->setEnabled(tb->IsRwValid(row));
}

//
// slots
//

void DlgNewAsset::ModifiedSlot() {
	Check();
}

void DlgNewAsset::OkSlot() {
	done(1);
}

void DlgNewAsset::CancelSlot() {
	done(0);
}
