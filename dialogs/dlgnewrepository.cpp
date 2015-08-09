#include "dlgnewrepository.h"
#include "database/soaenums.h"
#include <QFileDialog>
#include <QDateTime>
#include <QDir>



DlgNewRepository::DlgNewRepository(QWidget *_parent, const QString &_location) : QDialog(_parent), location(_location) {
	ui.setupUi(this);

	setWindowTitle("New repository");

	ui.repositoryNameEdit->setText(name = ""/*QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz")*/);
	ui.repositoryNameEdit->setFocus();

	Check();

	QObject::connect(ui.okButton, SIGNAL(clicked()), this, SLOT(OkSlot()));
	QObject::connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(CancelSlot()));
	QObject::connect(ui.repositoryNameEdit, SIGNAL(textChanged(QString)), this, SLOT(RepositoryNameEditSlot(QString)));
	QObject::connect(ui.locationButton, SIGNAL(clicked()), this, SLOT(RepositoryLocationButtonSlot()));
}

const QString &DlgNewRepository::Path() const {
	return path;
}

const QString &DlgNewRepository::Name() const {
	return name;
}

const QString &DlgNewRepository::Location() const {
	return location;
}

//
// slots
//

void DlgNewRepository::Check() {
	if (name != "") {
		QDir dir(location);
		if (dir.exists()) {
			path = dir.filePath(name + DB_EXTENSION);
			if (!QFileInfo(path).exists()) {
				ui.outputLabel->setText(path);
				ui.okButton->setEnabled(true);
				return;
			}
			else
				ui.outputLabel->setText(path + " already exists");
		}
		else
			ui.outputLabel->setText(dir.absolutePath() + " does not exist");
	}
	else
		ui.outputLabel->setText("Repository name must be specified");

	ui.okButton->setEnabled(false);
}

void DlgNewRepository::RepositoryNameEditSlot(const QString &_name) {
	name = _name;

	Check();
}

void DlgNewRepository::RepositoryLocationButtonSlot() {
	QString _location = QFileDialog::getExistingDirectory(this, "Repository location", location);
	if (!_location.isEmpty()) {
		location = _location;
		Check();
	}
}

void DlgNewRepository::CancelSlot() {
	done(0);
}

void DlgNewRepository::OkSlot() {
	done(1);
}

