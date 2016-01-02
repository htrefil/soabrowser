#include "widgets/wdggraph.h"
#include "soawidget.h"
#include <QWheelEvent>
#include <QMimeData>
#include <iostream>



void WdgGraph::mouseDoubleClickEvent(QMouseEvent *) {}

void WdgGraph::mousePressEvent(QMouseEvent *event) {

	mousePos.Set(event->x(), event->y());
	ht1 = ht2 = mouseTravel = 0;

	QGLWidget::mousePressEvent(event);
}

void WdgGraph::mouseMoveEvent(QMouseEvent *event) {

	if (event->buttons() == Qt::RightButton) {
		view.x -= event->x() - mousePos.x;
		view.y += event->y() - mousePos.y;

		updateGL();
	}
	else if (event->buttons() == Qt::LeftButton) {
		rotation.x += (event->y() - mousePos.y) * 0.2f;
		rotation.y += (event->x() - mousePos.x) * 0.2f;

		updateGL();
	}
//	else
//		hover.restart();

	mousePos.Set(event->x(), event->y());
	++mouseTravel;

	QGLWidget::mouseMoveEvent(event);
}

void WdgGraph::mouseReleaseEvent(QMouseEvent *event) {

	if (event->button() == Qt::LeftButton) {
        if (mouseTravel < 5) {
            setFocus();
			selectGL(event->x(), event->y(), event->modifiers() & Qt::ControlModifier, true);
        }
	}
	else if (event->button() == Qt::RightButton)
		if (mouseTravel < 5)
			menu->exec(mapToGlobal(event->pos()));
}

void WdgGraph::wheelEvent(QWheelEvent *event) {

	if (event->delta() > 0) {
		view.z /= 1.1f;
		view.x *= 1.1f;
		view.y *= 1.1f;
	}
	else {
		view.z *= 1.1f;
		view.x /= 1.1f;
		view.y /= 1.1f;
	}

	updateGL();
	QGLWidget::wheelEvent(event);
}

void WdgGraph::timerEvent(QTimerEvent *) {

	if (ht1 == mouseTravel && ht1 != ht2 && mousePos.x > 0 && mousePos.y > 0) {
		selectGL(mousePos.x, mousePos.y, false, false);
		updateGL();
	}

	ht2 = ht1;
	ht1 = mouseTravel;
}

void WdgGraph::dragEnterEvent(QDragEnterEvent *e) {

	if (e->mimeData()->hasFormat("application/applicationversionid"))
		e->acceptProposedAction();
	else
		e->ignore();
}

void WdgGraph::dropEvent(QDropEvent *e) {

	if (e->mimeData()->hasFormat("application/applicationversionid")) {

		QByteArray ba = e->mimeData()->data("application/applicationversionid");
		QDataStream ds(&ba, QIODevice::ReadOnly);
		unsigned int appId;
		ds >> appId;

		wg->Db().DeployApplicationVersion(appId, domainId, true);
		wg->RefreshViews();

		e->setDropAction(Qt::CopyAction);
		e->accept();
	}
	else
		e->acceptProposedAction();
}
