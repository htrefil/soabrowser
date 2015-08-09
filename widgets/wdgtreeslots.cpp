#include "dialogs/dlgnewasset.h"
#include "widgets/wdgtree.h"
#include "soawidget.h"
#include <QMenu>



void WdgTree::ItemSelectionChangedSlot() {

	if (!selectionGuard) {
		iv->DeselectAll();

		QList<QTreeWidgetItem *> items = selectedItems();
		for (QList<QTreeWidgetItem *>::iterator itemIt = items.begin(); itemIt != items.end(); ++itemIt) {

            WdgTreeItem *item = (WdgTreeItem *)*itemIt;
            iv->InvertRowSelection(item->id);
		}

		wg->RefreshViews();
	}
}

void WdgTree::CustomContextMenuRequestedSlot(const QPoint &pos) {

    WdgTreeItem *item = (WdgTreeItem *)itemAt(pos);

	if (item) {
        menuItemId = item->id;
		menuItemArg = item->text(0);

		QMenu *menu = new QMenu(this);
		menu->addAction("Show table", this, SLOT(ShowTableSlot()));
        menu->addAction("Expand selected applications", this, SLOT(ExpandSlot()));

		// asset specific menu items

		std::string tbName = iv->TbNameFromViewId(menuItemId);

		if (tbName == "Organization") {
			menu->addSeparator();
			menu->addAction(wg->GetAction("Add business unit"));
			menu->addAction(wg->GetAction("Add service domain"));
			menu->addAction(wg->GetAction("Delete organization"));
		}
		else if (tbName == "BusinessUnit") {
			menu->addSeparator();
			menu->addAction(wg->GetAction("Add application"));
			menu->addAction(wg->GetAction("Delete business unit"));
		}
		else if (tbName == "Application") {
			menu->addSeparator();
			menu->addAction(wg->GetAction("Add application version"));
			menu->addAction(wg->GetAction("Add service"));
			menu->addAction(wg->GetAction("Delete application"));
		}
		else if (tbName == "ApplicationVersion") {
			menu->addSeparator();
			menu->addAction(wg->GetAction("Add service version"));
			menu->addAction(wg->GetAction("Delete application version"));
		}
		else if (tbName == "DeployedApplication") {
			menu->addSeparator();
			menu->addAction(wg->GetAction("Delete deployed application"));
		}
		else if (tbName == "Service") {
			menu->addSeparator();
			menu->addAction(wg->GetAction("Delete service"));
		}
		else if (tbName == "ServiceVersion") {
			menu->addSeparator();
			menu->addAction(wg->GetAction("Delete service version"));
		}
		else if (tbName == "DeployedService") {
			menu->addSeparator();
			menu->addAction(wg->GetAction("Delete deployed service"));
		}
		else if (tbName == "Endpoint") {
			menu->addSeparator();
			menu->addAction(wg->GetAction("Delete endpoint"));
		}

		menu->exec(mapToGlobal(pos));
		delete menu;
	}
	else {
		QMenu *menu = new QMenu(this);
		menu->addAction(wg->GetAction("Add organization"));
		menu->exec(mapToGlobal(pos));
		delete menu;
	}
}

void WdgTree::ShowTableSlot() {
	wg->AddTable(menuItemId % 100);
}

void expandToLevel(QTreeWidget *tree, QTreeWidgetItem *item, int level) {
    tree->expandItem(item);
    if (--level == 0)
        return;
    for (int i = 0; i < item->childCount(); ++i)
        expandToLevel(tree, item->child(i), level);
}

void WdgTree::ExpandSlot() {
    unsigned mode = wg->Mode();

    QList<QTreeWidgetItem*> seldItems = selectedItems();
    for (int i = 0; i < seldItems.count(); ++i) {

        WdgTreeItem *item = (WdgTreeItem *)seldItems[i];
        std::string tbName = iv->TbNameFromViewId(item->id);

        if (mode == fmOutline) {
            if (tbName == "Organization")
                expandToLevel(this, item, 2);
            else if (tbName == "BusinessUnit")
                expandToLevel(this, item, 1);
            else
                expandItem(item);
        }
        else if (mode == fmRealization) {
            if (tbName == "Organization")
                expandToLevel(this, item, 3);
            else if (tbName == "BusinessUnit")
                expandToLevel(this, item, 2);
            else if (tbName == "Application")
                expandToLevel(this, item, 1);
            else
                expandItem(item);
        }
        else if (mode == fmRuntime) {
            if (tbName == "Organization")
                expandToLevel(this, item, 4);
            else if (tbName == "BusinessUnit")
                expandToLevel(this, item, 3);
            else if (tbName == "Application")
                expandToLevel(this, item, 2);
            else if (tbName == "ApplicationVersion")
                expandToLevel(this, item, 1);
            else
                expandItem(item);
        }
    }
}
