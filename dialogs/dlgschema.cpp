#include "dialogs/dlgschema.h"
#include "soawidget.h"
#include <QFileDialog>



DlgSchema::DlgSchema(SoaWg *_wg, QString &_schemaPath) :
	QDialog(_wg), wg(_wg), schemaPath(_schemaPath) {

	ui.setupUi(this);

	setWindowTitle("Schema");

	ui.schemaTree->setColumnWidth(0, 200);
	ui.schemaTree->setRootIsDecorated(false);

	QObject::connect(ui.schemaButton, SIGNAL(clicked()), this, SLOT(SchemaButtonSlot()));
	QObject::connect(ui.okButton, SIGNAL(clicked()), this, SLOT(OkButtonSlot()));

	Load("Schema not loaded.");
}

void DlgSchema::Load(const QString &errorText) {

	ui.schemaTree->clear();
	SoaTb *tb = wg->Db().Tb("Application");

	QTreeWidgetItem *item;
	for (SoaClVector::const_iterator cl = tb->Columns().begin(); cl != tb->Columns().end(); ++cl) {
		item = new QTreeWidgetItem(ui.schemaTree, QStringList() <<
								   QString::fromUtf8((cl->DisplayName() + (cl->Schema().critical ? "*" : "")).c_str()) <<
								   QString::fromUtf8(cl->Schema().schema.c_str()));

		if (cl->Schema().critical && !cl->Schema().valid)
			item->setForeground(0, QBrush(QColor(Qt::red)));
	}

	for (SoaClVector::const_iterator cl = tb->AuxColumns().begin(); cl != tb->AuxColumns().end(); ++cl) {
		item = new QTreeWidgetItem(ui.schemaTree, QStringList() <<
							QString::fromUtf8(cl->DisplayName().c_str()) <<
							"");

		item->setForeground(0, QBrush(QColor(Qt::darkGray)));
	}

	QPalette p = ui.label->palette();
	p.setColor(QPalette::Foreground, tb->IsSchemaValid() ? QColor(0, 0, 0) : QColor(255, 0, 0));

	ui.label->setText(tb->IsSchemaValid() ? "Schema OK." : errorText);
	ui.label->setPalette(p);
}

//
// slots
//

void DlgSchema::SchemaButtonSlot() {
	QString path = QFileDialog::getOpenFileName(this, "Load schema", schemaPath, "Schema files (*.xsd)");
	if (!path.isEmpty()) {
		schemaPath = path;
		wg->Db().Tb("Application")->LoadSchema(path.toStdString());
		wg->UpdateTitle();
		Load("Error loading schema.");
	}
}

void DlgSchema::OkButtonSlot() {
	done(1);
}
