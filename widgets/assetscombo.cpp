#include "widgets/assetscombo.h"
#include "interfaces/iview.h"
#include "dialogs/dlgnewasset.h"
#include "soawidget.h"
//#include <QMessageBox>
//#include <iostream>



AssetsCombo::AssetsCombo(QWidget *_p, SoaWg *_wg, SoaTb *_tb) : QComboBox(_p),
	wg(_wg),
	tb(_tb) {

	connect(this, SIGNAL(activated(int)), this, SLOT(Slot()));
}

void AssetsCombo::Refresh(SoaRw *currentRow) {

	clear();

	addItem("<Add new...>", (unsigned int)IV_NULL_VIEWID);

	int i = 1;
	for (SoaTb::iterator rw = tb->begin(); rw != tb->end(); ++rw) {

		addItem(QString::fromUtf8((*rw)->Name().c_str()), (*rw)->ViewId());

		if (currentRow == *rw)
			setCurrentIndex(i);

		++i;
	}
}

void AssetsCombo::Slot() {

	if (currentIndex() == -1)
		return;

	unsigned int id = itemData(currentIndex()).toUInt();

	if (id == IV_NULL_VIEWID) {

		SoaRw *rw = tb->AllocRw(-1, true);
		DlgNewAsset dlg(this, wg, tb, rw);
		if (dlg.exec()) {
			tb->AddRw(dlg.Row());
			Refresh(rw);
		}
		else
			delete rw;
	}
}
