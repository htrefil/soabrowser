#include "widgets/wdggateway.h"
#include "soawidget.h"
#include <QMouseEvent>
#include <iostream>



WdgGatewayTree::WdgGatewayTree(QWidget *_p, SoaWg *_w, GtDomain *_c) : QTreeWidget(_p),
	wg(_w),
	conflation(_c),
	guard(false),
	travel(100) {

	setMouseTracking(true);
	setExpandsOnDoubleClick(false);
	setSortingEnabled(true);
	setSelectionMode(QAbstractItemView::ExtendedSelection);

	connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelectionChangedSlot()));
}

void WdgGatewayTree::mouseDoubleClickEvent(QMouseEvent *e) {

	QTreeWidget::mouseDoubleClickEvent(e);

	if (e->button() == Qt::LeftButton) {

		// try to resolve the conflict

		QTreeWidgetItem *i = itemAt(e->pos());
		if (i) {

			int sId = i->data(0, Qt::UserRole).toInt();
			int eId = i->data(0, Qt::UserRole + 1).toInt();

			if (sId < (int)conflation->size()) {

				GtService &gtS = conflation->at(sId);

				if (eId < 0) {	// service item

					if (gtS.status == gesNew)
						emit DeployServiceSignal(gtS);
				}
				else if (eId < (int)gtS.size()) {

					if (gtS.status == gesOld && gtS[eId].status == gesNew) {
						emit DeployEndpointSignal(gtS, gtS[eId]);
					}
					else
						emit BrowseEndpointIndexSignal(gtS[eId]);
				}
			}
		}
	}
}

void WdgGatewayTree::mousePressEvent(QMouseEvent *e) {
	QTreeWidget::mousePressEvent(e);
}

void WdgGatewayTree::mouseMoveEvent(QMouseEvent *e) {
	QTreeWidget::mouseMoveEvent(e);
}

void WdgGatewayTree::mouseReleaseEvent(QMouseEvent *e) {
	QTreeWidget::mouseReleaseEvent(e);
}

void WdgGatewayTree::itemSelectionChangedSlot() {

	if (guard)
		return;

	wg->Db().DeselectAll();
	registry->ClearSelection();

	if (mode == mwmTree) {

		for (int i = 0; i < topLevelItemCount(); ++i) {

			QTreeWidgetItem *serviceItem = topLevelItem(i);

			unsigned serviceId = serviceItem->data(0, Qt::UserRole + 2).toUInt();

			if (serviceItem->isSelected()) {
				if (serviceId % 100 == REPO_TB_IDX)
					registry->Select(serviceId);
				else
					wg->Db().InvertRowSelection(serviceId);
			}

			for (int j = 0; j < serviceItem->childCount(); ++j) {

				QTreeWidgetItem *endpointItem = serviceItem->child(j);

				unsigned endpointId = endpointItem->data(0, Qt::UserRole + 2).toUInt();

				if (endpointItem->isSelected()) {
					if (endpointId % 100 == REPO_TB_IDX)
						registry->SelectEndpoint(serviceId, endpointId);
					else
						wg->Db().InvertRowSelection(endpointId);
				}
			}
		}
	}
	else {

		for (int i = 0; i < topLevelItemCount(); ++i) {

			QTreeWidgetItem *endpointItem = topLevelItem(i);

			unsigned endpointId = endpointItem->data(0, Qt::UserRole + 2).toUInt();

			if (endpointItem->isSelected()) {
				if (endpointId % 100 == REPO_TB_IDX)
					registry->SelectEndpoint(endpointId);
				else
					wg->Db().InvertRowSelection(endpointId);
			}
		}
	}

	wg->RefreshViews();
}
