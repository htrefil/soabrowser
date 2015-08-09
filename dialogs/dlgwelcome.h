#ifndef dlgwelcome_h
#define dlgwelcome_h

#include "ui_dlgwelcome.h"
#include <QDialog>

#define WELCOME_CLOSE	0
#define WELCOME_OPEN	1
#define WELCOME_NEW		2
#define WELCOME_RECENT	3



class Recents;

class DlgWelcome : public QDialog
{
	Q_OBJECT

public:
	Ui_dlgWelcome ui;
	QString recent;

	DlgWelcome(QWidget *, Recents &);

private:
	Recents &recents;

private slots:
	void OpenSlot();
	void NewSlot();
	void CloseSlot();
	void RecentSlot();
	void RecentDoubleClickedSlot(QListWidgetItem *);
};

#endif
