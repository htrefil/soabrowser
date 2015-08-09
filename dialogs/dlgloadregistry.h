#ifndef dlglayer7_h
#define dlglayer7_h

#include "gateways/layer7loader.h"
#include "ui_dlglayer7.h"
#include <QDialog>



class SoaWg;

class DlgLoadRegistry : public QDialog
{
	Q_OBJECT

public:
	DlgLoadRegistry(SoaWg *, unsigned int);

	SoaGtwy *Gateway();

private:
	Ui::dlgLayer7 ui;
	SoaWg *wg;
	unsigned int regId;
	Layer7 loader;
	bool loading;

private slots:
	void LoadSlot();
	void UpdateSlot(const QString &);
	void DoneSlot(const QString &);
	void MergeSlot();
};

#endif
