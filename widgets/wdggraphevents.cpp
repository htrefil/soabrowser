#include "wdggraph.h"
#include <QWheelEvent>



void WdgGraph::initializeGL() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glPointSize(4.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);

	glEnable(GL_FOG);
	float FogCol[3] = { 1.0f, 1.0f, 1.0f };
	glFogfv(GL_FOG_COLOR, FogCol);
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogf(GL_FOG_START, 50.0f);
	glFogf(GL_FOG_END, 180.0f);
}

void WdgGraph::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glFrustum(
				0.001 * (view.x - width() * 0.5) * view.z,
				0.001 * (view.x + width() * 0.5) * view.z,
				0.001 * (view.y - height() * 0.5) * view.z,
				0.001 * (view.y + height() * 0.5) * view.z,
				1.0, 200.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -100.0f);
	float scale = bounds.Deviation();
	glScalef(scale, scale, scale);
	glRotatef(rotation.x, 1.0f, 0.0f, 0.0f);
	glRotatef(rotation.y, 0.0f, 0.0f, 1.0f);
	SoaPoint center = bounds.Center();
	glTranslatef(-center.x, -center.y, -center.z);

	for (WdgGraphNodeList::iterator node = nodes.begin(); node != nodes.end(); ++node) {
		glPushMatrix();
		glLoadIdentity();

		glTranslatef(0.0f, 0.0f, -100.0f);
		glScalef(scale, scale, scale);
		glRotatef(rotation.x, 1.0f, 0.0f, 0.0f);
		glRotatef(rotation.y, 0.0f, 0.0f, 1.0f);
		glTranslatef(-center.x, -center.y, -center.z);

		glTranslatef(node->p[0], node->p[1], node->p[2]);
		glRotatef(-rotation.y, 0.0f, 0.0f, 1.0f);
		glRotatef(-rotation.x, 1.0f, 0.0f, 0.0f);

		DrawRectangle(8, 6, 0, 0, 0);
		DrawRectangle(2, 1.5f, -8, 2.5, 1);
		DrawRectangle(2, 1.5f, -8, -2.5, 1);

		glPopMatrix();
	}

/*	for (WdgGraphNodeList::iterator node = secNodes.begin(); node != secNodes.end(); ++node) {
		glPushMatrix();
		glLoadIdentity();

		glTranslatef(0.0f, 0.0f, -100.0f);
		glScalef(scale, scale, scale);
		glRotatef(rotation.x, 1.0f, 0.0f, 0.0f);
		glRotatef(rotation.y, 0.0f, 0.0f, 1.0f);
		glTranslatef(-center.x, -center.y, -center.z);

		glTranslatef(node->p[0], node->p[1], node->p[2]);
		glRotatef(-rotation.y, 0.0f, 0.0f, 1.0f);
		glRotatef(-rotation.x, 1.0f, 0.0f, 0.0f);

		DrawEllipse(4, 3);

		glPopMatrix();
	}*/

	glBegin(GL_LINES);
	for (WdgGraphLinkList::iterator link = links.begin(); link != links.end(); ++link) {
		glColor4ub(0, 0, 0, 160);
		glVertex3fv(nodes[link->prIn].p);
		glColor4ub(180, 180, 180, 160);
		glVertex3fv(nodes[link->chIn].p);
	}
	glEnd();
}

void WdgGraph::resizeGL(int width, int height) {
	glViewport(0, 0, width, height);
}

void WdgGraph::mousePressEvent(QMouseEvent *event) {
	mousePos.Set(event->x(), event->y());

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

	mousePos.Set(event->x(), event->y());

	QGLWidget::mouseMoveEvent(event);
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

//
// drawing utilities
//

void DrawRectangle(float width, float height, float xOffset, float yOffset, float zOffset) {

	glColor4ub(180, 210, 255, 255);
	glBegin(GL_QUADS);
	glVertex3f(xOffset - width, yOffset - height, zOffset);
	glVertex3f(xOffset - width, yOffset + height, zOffset);
	glVertex3f(xOffset + width, yOffset + height, zOffset);
	glVertex3f(xOffset + width, yOffset - height, zOffset);
	glEnd();

	glColor4ub(0, 0, 0, 255);
	glBegin(GL_LINE_LOOP);
	glVertex3f(xOffset - width, yOffset - height, zOffset + 0.5f);
	glVertex3f(xOffset - width, yOffset + height, zOffset + 0.5f);
	glVertex3f(xOffset + width, yOffset + height, zOffset + 0.5f);
	glVertex3f(xOffset + width, yOffset - height, zOffset + 0.5f);
	glEnd();
}

void DrawEllipse(float width, float height) {
	glBegin(GL_LINE_LOOP);
	float angleStep = 3.1416f / 6.0f;
	for (int i = 0; i <= 6; ++i)
		glVertex2f(cos(i * angleStep) * height - width, sin(i * angleStep) * height);
	for (int i = 6; i <= 12; ++i)
		glVertex2f(cos(i * angleStep) * height + width, sin(i * angleStep) * height);
	glEnd();
}
