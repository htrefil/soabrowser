#ifndef dlgintermediaries_h
#define dlgintermediaries_h

#include "ui_dlgintermediaries.h"
#include <QDialog>



class SoaWg;
class SoaTb;
class IViewable;

class DlgIntermediaries : public QDialog
{
	Q_OBJECT

public:
	DlgIntermediaries(SoaWg *, SoaTb *, IViewable *);

private:
	Ui::dlgIntermediaries ui;
	SoaWg *wg;
	SoaTb *tb;
	IViewable *iv;

	void Refresh();

private slots:
	void AddSlot();
	void EditSlot();
	void RemoveSlot();
	void CloseSlot();
	void StateSlot();
};

#endif
