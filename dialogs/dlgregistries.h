#ifndef dlgregistries_h
#define dlgregistries_h

#include "ui_dlgregistries.h"
#include <QDialog>



class SoaWg;
class IViewable;

class DlgRegistries : public QDialog
{
	Q_OBJECT

public:
	DlgRegistries(SoaWg *);

private:
	Ui_dlgRegistries ui;
	SoaWg *wg;
	bool guard;

	void Refresh();
	void LoadIntermediaries();
	void UpdateIntermediaries();

private slots:
	void AddSlot();
	void EditSlot();
	void RemoveSlot();
	void CloseSlot();
	void StateSlot();
	void IntermediaryItemChangedSlot(QTreeWidgetItem *);
};

#endif
