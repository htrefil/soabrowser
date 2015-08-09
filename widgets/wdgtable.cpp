#include "widgets/wdgtable.h"
#include "interfaces/iviewable.h"
#include "soawidget.h"
#include <QMenu>
#include <QVBoxLayout>
#include <QHeaderView>
#include <set>



WdgTable::WdgTable(QWidget *_parent, SoaWg *_wg, IViewable *_iv, int _tbIndex) :
	QTableWidget(_parent), wg(_wg), iv(_iv), tbIndex(_tbIndex),
	selectionGuard(false), itemClickedGuard(false) {

	setWordWrap(true);
	setContextMenuPolicy(Qt::CustomContextMenu);
	setSortingEnabled(true);

	QObject::connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(ItemSelectionChangedSlot()));
	QObject::connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(ContextMenuRequestedSlot(QPoint)));
}

QString WdgTable::Refresh() {

	ItTable table = iv->ItemTable(tbIndex);
	styles = table.styles;

	int nRows = rowCount();
	bool virginTable = rowCount() == 0;

	setColumnCount(table.columns.size());
	setRowCount(table.rows.size());
	setHorizontalHeaderLabels(ToStringList(table.columns));

	ItRowMap &itemMap = table.rows;

	int row = 0;
	while (row < nRows) {
		QTableWidgetItem *it = item(row, 0);

		ItRowMap::iterator itemIt = itemMap.find(it->data(Qt::UserRole + 1).toUInt());

		if (itemIt != itemMap.end()) {

			if (!itemClickedGuard) {
				selectionGuard = true;
				it->setSelected(itemIt->second.selected);
				selectionGuard = false;
			}

			if (it->data(Qt::UserRole + 2) != itemIt->second.ts) {
				it->setData(Qt::UserRole + 2, itemIt->second.ts);

				for (unsigned int col = 0; col < itemIt->second.texts.size(); ++col) {

					QTableWidgetItem *i = new QTableWidgetItem(QString::fromUtf8(itemIt->second.texts[col].c_str()));
					i->setData(Qt::UserRole + 1, itemIt->first);
					i->setData(Qt::UserRole + 2, itemIt->second.ts);

					setItem(row, col, i);
				}
			}

			itemMap.erase(itemIt);
			++row;
		}
		else {
			selectionGuard = true;
			removeRow(row);
			selectionGuard = false;
		}
	}

	for (ItRowMap::iterator itemIt = itemMap.begin(); itemIt != itemMap.end(); ++itemIt) {

		int col = 0;
		for (std::vector<std::string>::const_iterator str = itemIt->second.texts.begin(); str != itemIt->second.texts.end(); ++str) {

			QTableWidgetItem *i = new QTableWidgetItem(QString::fromUtf8(str->c_str()));
			i->setData(Qt::UserRole + 1, itemIt->first);
			i->setData(Qt::UserRole + 2, itemIt->second.ts);

			setItem(row, col++, i);
		}

		++row;
	}

	if (virginTable)
		FormatColumnsSlot();

	return QString::fromUtf8(table.name.c_str());
}

void WdgTable::FormatCellsSlot() {

	for (int col = 0; col < columnCount(); ++col)
		if (styles[col] == tcsFixedWide)
			setColumnWidth(col, 280);
		else
			resizeColumnToContents(col);

	for (int row = 0; row < rowCount(); ++row)
		resizeRowToContents(row);
}

void WdgTable::FormatColumnsSlot() {

	for (int col = 0; col < columnCount(); ++col)
		if (styles[col] == tcsFixedWide)
			setColumnWidth(col, 280);
		else
			resizeColumnToContents(col);
}

QStringList WdgTable::ToStringList(const std::vector<std::string> &stringVec) {
	QStringList stringList;

	for (std::vector<std::string>::const_iterator str = stringVec.begin(); str != stringVec.end(); ++str)
		stringList << QString::fromUtf8(str->c_str());

	return stringList;
}

//
// slots
//

void WdgTable::ContextMenuRequestedSlot(const QPoint &pos) {
	QMenu menu;
	menu.addAction("Fit cells to contents", this, SLOT(FormatCellsSlot()));
	menu.exec(viewport()->mapToGlobal(pos));
}

void WdgTable::ItemSelectionChangedSlot() {

	if (!selectionGuard) {
		itemClickedGuard = true;
		iv->DeselectAll();

		std::set<unsigned int> ids;
		QList<QTableWidgetItem *> items = selectedItems();
		for (QList<QTableWidgetItem *>::iterator it = items.begin(); it != items.end(); ++it)
			ids.insert((*it)->data(Qt::UserRole + 1).toUInt());

		for (std::set<unsigned int>::iterator id = ids.begin(); id != ids.end(); ++id)
			iv->InvertRowSelection(*id);

		wg->RefreshViews();
		itemClickedGuard = false;
	}
}

//
// container
//

WdgTableContainer::WdgTableContainer(SoaWg *_wg, IViewable *_iv, int _tbIndex) :
	IView(_wg), table(new WdgTable(this, _wg, _iv, _tbIndex)) {

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(table);
}

QString WdgTableContainer::Refresh() {
	return table->Refresh();
}
