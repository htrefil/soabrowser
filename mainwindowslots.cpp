#include "mainwindow.h"
#include "soawidget.h"
#include "dialogs/dlgnewrepository.h"
#include "dialogs/dlgschema.h"
#include "dialogs/dlgwelcome.h"
#include "logger/logger.h"
#include <QFileDialog>
#include <QMessageBox>

//
// file menu
//

void MainWindow::NewRepositorySlot() {

	DlgNewRepository dialog(this, repositoryPath);
	if (dialog.exec())
		ShowRepository(dialog.Path(), false);
}

void MainWindow::LoadRepositorySlot() {

	QString path = QFileDialog::getOpenFileName(this, "Load repository", repositoryPath, QString("Repository files (*") + DB_EXTENSION + ")");
	if (path != "")
		ShowRepository(path, true);
}

void MainWindow::SaveRepositorySlot() {
	if (CurrRepo()) {
		CurrRepo()->Save();
		recents.Add(CurrRepo()->Path());
		WriteSettings();
	}
}

void MainWindow::CloseRepositorySlot() {
	if (CurrRepo())
		if (CurrRepo()->IsReadyToClose())
			delete CurrRepo();
}

void MainWindow::SelectLoggingDirectorySlot() {

	QString location = QFileDialog::getExistingDirectory(this, "Select logging directory", Logger::ins.Location());

	if (location.isEmpty())
		return;

	Logger::ins.SetLocation(location);
}

void MainWindow::QuitSlot() {

	bool quit = true;
	for (int i = 0; i < ui->tabWidget->count(); ++i)
		if (!((SoaWg *)ui->tabWidget->widget(i))->IsReadyToClose()) {
			quit = false;
			break;
		}

	if (quit) {
		WriteSettings();
		QApplication::exit();
	}
}

//
//	repository menu
//

void MainWindow::LoadPortfolioSlot() {

	if (CurrRepo()) {
		SoaWg *widget = CurrRepo();

		int dlgResult = 1;
		if (!widget->Db().Tb("Application")->IsSchemaValid()) {

			if (QMessageBox::warning(this,
									"Portfolio",
									"Cannot load portfolio without a valid schema.\nDo you want to load the schema now?",
									QMessageBox::Yes | QMessageBox::No,
									QMessageBox::Yes) == QMessageBox::Yes) {

				DlgSchema dialog(widget, schemaPath);
				dlgResult = dialog.exec();
				WriteSettings();
			}
		}

		if (dlgResult && widget->Db().Tb("Application")->IsSchemaValid()) {

			QString path = QFileDialog::getOpenFileName(this, "Load portfolio", portfolioPath, "Portfolio files (*.xml)");
			if (!path.isEmpty()) {
				portfolioPath = path;
				WriteSettings();

				widget->LoadPortfolio(portfolioPath);
			}
		}
	}
}

void MainWindow::LoadSchemaSlot() {
	if (CurrRepo()) {
		DlgSchema dialog(CurrRepo(), schemaPath);
		dialog.exec();
		WriteSettings();
	}
}

void MainWindow::LoadCentraSiteReportSlot() {
	if (CurrRepo()) {
		QString fileName = QFileDialog::getOpenFileName(this, "Load CentraSite report XML", reportPath, "XML files (*.xml)");
		if (!fileName.isEmpty()) {
			reportPath = fileName;
			WriteSettings();

			CurrRepo()->LoadCentraSiteReportXML(fileName);
		}
	}
}

void MainWindow::LoadSQLSlot() {
	if (CurrRepo()) {
		QString fileName = QFileDialog::getOpenFileName(this, "Load repository SQL", "./", "SQL files (*.sql)");
		if (!fileName.isEmpty())
			CurrRepo()->LoadSQL(fileName);
	}
}

void MainWindow::DumpSQLSlot() {
	if (CurrRepo())
		CurrRepo()->DumpSQL();
}

//
// views menu
//

void MainWindow::Add2DGraphSlot() {
	if (CurrRepo())
		CurrRepo()->AddGraph(0);
}

void MainWindow::Add25DGraphSlot() {
	if (CurrRepo())
		CurrRepo()->AddGraph(1);
}

void MainWindow::Add3DGraphSlot() {
	if (CurrRepo())
		CurrRepo()->AddGraph(2);
}

//
// help menu
//

void MainWindow::WelcomeSlot() {

	DlgWelcome dialog(this, recents);
	int res = dialog.exec();

	switch (res) {
	case WELCOME_NEW:
		NewRepositorySlot();
		break;
	case WELCOME_OPEN:
		LoadRepositorySlot();
		break;
	case WELCOME_RECENT:
		ShowRepository(dialog.recent, true);
		break;
	default:;
	}
}

//
// general
//

void MainWindow::RetitleTabSlot(QWidget *_widget, const QString &_title) {
	int index = ui->tabWidget->indexOf(_widget);
	if (index != -1)
		ui->tabWidget->setTabText(index, _title);
}
