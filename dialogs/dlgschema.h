#ifndef dlgschema_h
#define dlgschema_h

#include "ui_dlgschema.h"
#include <QDialog>



class SoaWg;

class DlgSchema : public QDialog
{
	Q_OBJECT

public:
	Ui::dlgSchema ui;

	DlgSchema(SoaWg *, QString &);

private:
	SoaWg *wg;
	QString &schemaPath;

	void Load(const QString &);

private slots:
	void SchemaButtonSlot();
	void OkButtonSlot();
};

#endif
