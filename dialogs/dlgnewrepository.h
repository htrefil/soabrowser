#ifndef dlgnewrepository_h
#define dlgnewrepository_h

#include "ui_dlgnewrepository.h"
#include <QDialog>



class DlgNewRepository : public QDialog
{
	Q_OBJECT

public:
	DlgNewRepository(QWidget *, const QString &);

	const QString &Path() const;
	const QString &Name() const;
	const QString &Location() const;

private:
	Ui::dlgNewRepository ui;
	QString path, name, location;

	void Check();

private slots:
	void CancelSlot();
	void OkSlot();
	void RepositoryNameEditSlot(const QString &);
	void RepositoryLocationButtonSlot();
};

#endif
