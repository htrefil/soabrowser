#include "widgets/wdgtree.h"
#include "soawidget.h"
#include <QMouseEvent>
#include <QDataStream>
#include <QMimeData>
#include <QDrag>
#include <iostream>
#include <limits>



void WdgTree::mousePressEvent(QMouseEvent *e) {

	QTreeWidget::mousePressEvent(e);

	if (e->button() == Qt::LeftButton) {

        WdgTreeItem *i = (WdgTreeItem *)itemAt(e->pos());
		if (i) {

            if (!i->en)
				return;

            assetId = i->id;

            if (iv->TbNameFromViewId(assetId) == "ApplicationVersion")
				travel = 0;
		}
		else
			assetId = std::numeric_limits<unsigned int>::max();
	}
}

void WdgTree::mouseReleaseEvent(QMouseEvent *e) {
	QTreeWidget::mouseReleaseEvent(e);
}

void WdgTree::mouseMoveEvent(QMouseEvent *e) {

	QTreeWidget::mouseMoveEvent(e);

	if (e->buttons() == Qt::LeftButton) {

		if (travel++ == 5) {

			QByteArray ba;
			QDataStream ds(&ba, QIODevice::WriteOnly);
			ds << assetId;

			QMimeData *mimeData = new QMimeData;
			mimeData->setData("application/applicationversionid", ba);
			mimeData->setText("ApplicationVersion");

			QDrag *drag = new QDrag(this);
			drag->setMimeData(mimeData);
			drag->exec();
		}
	}
}
