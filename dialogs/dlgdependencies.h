#ifndef dlgdependencies_h
#define dlgdependencies_h

#include "ui_dlgdependencies.h"
#include <QDialog>



class SoaRw;

class DlgDependencies : public QDialog
{
	Q_OBJECT

public:
	DlgDependencies(QWidget *, SoaRw *);

private:
	Ui_dlgDependencies ui;

	void AddDependencyItems(QTreeWidgetItem *, SoaRw *);

private slots:
	void OkSlot();
	void CancelSlot();
};

#endif
