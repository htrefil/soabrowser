#ifndef dlgchooseasset_h
#define dlgchooseasset_h

#include "database/soarow.h"
#include "ui_dlgchooseasset.h"
#include <QDialog>



class DlgChooseAsset : public QDialog
{
	Q_OBJECT

public:
	DlgChooseAsset(QWidget *, const SoaRwList *, const QString &);

	unsigned int SelectedId() const;
	SoaRw *SelectedRow() const;

private:
	Ui::dlgChooseAsset ui;
	const SoaRwList *list;

private slots:
	void OkSlot();
	void CancelSlot();
	void SelectionChangedSlot();
};

#endif
