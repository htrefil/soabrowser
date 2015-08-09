#include "dlgwelcome.h"
#include "helpers/recents.h"



DlgWelcome::DlgWelcome(QWidget *_parent, Recents &_recents) : QDialog(_parent), recents(_recents) {

	ui.setupUi(this);
	setWindowTitle("Welcome");

	// populate recents list

	for (Recents::iterator r = recents.begin(); r != recents.end(); ++r)
		ui.recentsList->addItem(*r);

	if (ui.recentsList->count())
		ui.recentsList->setCurrentItem(ui.recentsList->item(0));

	ui.recentButton->setEnabled(ui.recentsList->count() > 0);

	// signals & slots

	QObject::connect(ui.openButton, SIGNAL(clicked()), this, SLOT(OpenSlot()));
	QObject::connect(ui.newButton, SIGNAL(clicked()), this, SLOT(NewSlot()));
	QObject::connect(ui.closeButton, SIGNAL(clicked()), this, SLOT(CloseSlot()));
	QObject::connect(ui.recentButton, SIGNAL(clicked()), this, SLOT(RecentSlot()));
	QObject::connect(ui.recentsList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(RecentDoubleClickedSlot(QListWidgetItem *)));
}

//
// slots
//

void DlgWelcome::OpenSlot() {
	done(WELCOME_OPEN);
}

void DlgWelcome::NewSlot() {
	done(WELCOME_NEW);
}

void DlgWelcome::CloseSlot() {
	done(WELCOME_CLOSE);
}

void DlgWelcome::RecentSlot() {
	recent = ui.recentsList->currentItem()->text();
	done(WELCOME_RECENT);
}

void DlgWelcome::RecentDoubleClickedSlot(QListWidgetItem *item) {
	recent = item->text();
	done(WELCOME_RECENT);
}
