#include "widgets/wdgtree.h"
#include "soawidget.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QComboBox>



WdgTree::WdgTree(SoaWg *_wg, IViewable *_iv) : QTreeWidget(_wg),
	wg(_wg), iv(_iv),
	selectionGuard(false), inspecteeId(0), travel(1000) {

	setContextMenuPolicy(Qt::CustomContextMenu);
	setMouseTracking(true);
	setAcceptDrops(true);

	// tree columns

	setHeaderLabels(QStringList("Asset"));
	setHeaderHidden(true);
	setMinimumSize(200, 100);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::ExtendedSelection);

	// signals & slots

	QObject::connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(ItemSelectionChangedSlot()));
	QObject::connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(CustomContextMenuRequestedSlot(QPoint)));
}

//
// refresh tree
//

void WdgTree::Refresh() {
	ItItemMap itemMap = iv->ItemMap(wg->Mode(), iv->SelectedDomain());
	inspecteeId = itemMap.in;
	RefreshItems(invisibleRootItem(), itemMap);
}

bool WdgTree::RefreshItems(QTreeWidgetItem *parentIt, std::map<unsigned int, ItItem> &itemMap) {

	bool expandParent = false;

	int index = 0;
	while (index < parentIt->childCount()) {

		WdgTreeItem *childIt = (WdgTreeItem *)parentIt->child(index);
		ItItemMap::iterator itemIt = itemMap.find(childIt->id);

		if (itemIt != itemMap.end()) {

			selectionGuard = true;

			if (itemIt->first == inspecteeId) {
				setCurrentItem(childIt);
				scrollToItem(childIt);
			}

			childIt->setSelected(itemIt->second.selected);
			if (itemIt->second.selected)
				expandParent = true;

			selectionGuard = false;

			SetIcon(childIt, itemIt->second.icon);

			childIt->setText(0, QString::fromUtf8(itemIt->second.text.c_str()));

			if (childIt->ts != itemIt->second.ts)
				childIt->ts = itemIt->second.ts;

			childIt->en = itemIt->second.enabled;

			if (RefreshItems(childIt, itemIt->second.children)) {
				expandItem(childIt);
				expandParent = true;
			}

			itemMap.erase(itemIt);
			index++;
		}
		else {
			selectionGuard = true;
			delete childIt;
			selectionGuard = false;
		}
	}

	for (ItItemMap::iterator itemIt = itemMap.begin(); itemIt != itemMap.end(); ++itemIt) {

		WdgTreeItem *item = new WdgTreeItem(parentIt,
											QStringList(QString::fromUtf8(itemIt->second.text.c_str())),
											itemIt->second.sortby);
		item->id = itemIt->first;
		item->ts = itemIt->second.ts;
		item->en = itemIt->second.enabled;
		SetIcon(item, itemIt->second.icon);

		RefreshItems(item, itemIt->second.children);
	}

	return expandParent;
}

void WdgTree::SetIcon(WdgTreeItem *i, int c) {

	QString icon = "";
	switch (c) {
	case iiiOrg:	icon = ":/images/resources/org.png"; break;
	case iiiOrg2:	icon = ":/images/resources/org2.png"; break;
	case iiiUnit:	icon = ":/images/resources/unit.png"; break;
	case iiiUnit2:	icon = ":/images/resources/unit2.png"; break;
	case iiiApp:	icon = ":/images/resources/app.png"; break;
	case iiiApp1:	icon = ":/images/resources/app1.png"; break;
	case iiiApp2:	icon = ":/images/resources/app2.png"; break;
	case iiiServ:	icon = ":/images/resources/service.png"; break;
	case iiiServ1:	icon = ":/images/resources/service1.png"; break;
	case iiiServ2:	icon = ":/images/resources/service2.png"; break;
	case iiiEnd:	icon = ":/images/resources/end.png"; break;
	case iiiEnd2:	icon = ":/images/resources/end2.png"; break;
	default:;
	}

	if (icon != "")
		i->setIcon(0, QIcon(icon));
}

//
// container
//

WdgTreeContainer::WdgTreeContainer(SoaWg *wg, IViewable *iv) : IView(wg), tree(new WdgTree(wg, iv)) {
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(tree);
}

QString WdgTreeContainer::Refresh() {
	tree->Refresh();
	return "";
}
