#include "widgets/wdginspector.h"
#include "soawidget.h"
#include <QVBoxLayout>



WdgInspector::WdgInspector(QWidget *_parent, SoaWg *_wg) :
	QTreeWidget(_parent), row(0), delegate(new WdgInspectorDelegate(this, _wg)) {

	setHeaderLabels(QStringList() << "Property" << "Value");
	setAlternatingRowColors(true);
	setRootIsDecorated(false);
	setItemDelegate(delegate);
}

const WdgInspectorDelegate *WdgInspector::Delegate() const {
	return delegate;
}

void WdgInspector::SetRow(SoaRw *_row) {

	delegate->SetRow(row = _row);
	clear();

	if (row) {
		SoaTb *tb = row->Tb();

		QTreeWidgetItem *item;
		int index = SKIP_COLUMNS;
		const SoaClVector &columns = tb->Columns();
		for (SoaClVector::const_iterator cl = columns.begin() += SKIP_COLUMNS; cl != columns.end(); ++cl) {

			item = new QTreeWidgetItem(this, QStringList() <<
									   QString::fromUtf8((cl->DisplayName() + (cl->IsMandatory() ? "*" : "")).c_str()) <<
									   QString::fromUtf8(row->Ce(index++)->ToString().c_str()));

			item->setFlags(item->flags() | Qt::ItemIsEditable);
			item->setSizeHint(0, QSize(10, 24));
		}

		index = 0;
		const SoaClVector &auxColumns = tb->AuxColumns();
		for (SoaClVector::const_iterator cl = auxColumns.begin(); cl != auxColumns.end(); ++cl) {

			item = new QTreeWidgetItem(this, QStringList() <<
									   QString::fromUtf8(cl->DisplayName().c_str()) <<
									   QString::fromUtf8(row->AuxCe(index++)->ToString().c_str()));

			item->setForeground(0, QBrush(QColor(Qt::darkGray)));
			item->setForeground(1, QBrush(QColor(Qt::darkGray)));
			item->setSizeHint(0, QSize(10, 24));
		}
	}
}

//
// container
//

WdgInspectorContainer::WdgInspectorContainer(SoaWg *_wg, IViewable *_iv) :
	IView(_wg), iv(_iv),
	inspector(new WdgInspector(this, _wg)),
	browser(new QTextBrowser(this)),
	splitter(new QSplitter(this)) {

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(splitter);

	splitter->setOrientation(Qt::Vertical);
	splitter->addWidget(inspector);
	splitter->addWidget(browser);
	splitter->setStretchFactor(0, 3);
	splitter->setStretchFactor(1, 1);
}

// Gets the last selected row from the model,
// and displays it in the inspector widget.
QString WdgInspectorContainer::Refresh() {
	InItem inspectee = iv->Inspectee();

	inspector->SetRow(inspectee.row);

	// compose & set browser text
	if (inspectee.row)
		browser->setText(QString("<b>") + QString::fromUtf8(inspectee.row->Name().c_str()) + "</b> (" +
						 QString::fromUtf8(inspectee.row->Tb()->Name().c_str()) + ")<br/><br/>" +
						 "<i>" + QString::fromUtf8(inspectee.row->Ce(DES_COL_INDEX)->Txt().c_str()) + "</i>");
	else
		browser->clear();

	return windowTitle();
}
