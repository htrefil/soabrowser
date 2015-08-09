#ifndef dlgnewasset_h
#define dlgnewasset_h

#include "ui_dlgnewasset.h"
#include <QDialog>



class SoaWg;
class SoaTb;
class SoaRw;
class WdgInspector;

class DlgNewAsset : public QDialog
{
	Q_OBJECT

public:
	Ui_dlgNewAsset ui;

	DlgNewAsset(QWidget *, SoaWg *, SoaTb *, SoaRw *);

	SoaRw *Row();

private:
	SoaWg *wg;
	WdgInspector *ins;
	SoaTb *tb;
	SoaRw *row;

	void Check();

public slots:
	void OkSlot();
	void CancelSlot();
	void ModifiedSlot();
};

#endif
