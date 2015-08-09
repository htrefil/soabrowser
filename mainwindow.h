#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "helpers/recents.h"
#include <QMainWindow>



namespace Ui {
	class MainWindow;
}



class ITab;
class SoaWg;
class WdgWelcome;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private:
	Ui::MainWindow *ui;
	QString repositoryPath, reportPath, portfolioPath, schemaPath;
	Recents recents;

	void ShowRepository(const QString &, bool);
	bool FocusRepository(const QString &);

	void ReadSettings();
	void WriteSettings();
	SoaWg *CurrRepo();

	void closeEvent(QCloseEvent *);

public slots:
	void NewRepositorySlot();
	void LoadRepositorySlot();
	void SaveRepositorySlot();
	void CloseRepositorySlot();
	void SelectLoggingDirectorySlot();
	void QuitSlot();

	void LoadPortfolioSlot();
	void LoadSchemaSlot();
	void LoadCentraSiteReportSlot();
	void LoadSQLSlot();
	void DumpSQLSlot();

	void Add2DGraphSlot();
	void Add25DGraphSlot();
	void Add3DGraphSlot();

	void WelcomeSlot();

	void RetitleTabSlot(QWidget *, const QString &);
};

#endif
