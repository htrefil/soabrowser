#include "widgets/wdggraph.h"
#include "soawidget.h"

#define WG_ANGLE_STEP	0.314159f
#define WG_PICK_HALF	10
#define WG_PICK_SIZE	WG_PICK_HALF * 2
#define WG_SEL_LINE_W	2.0f
#define TT_MARGIN		3



unsigned char WdgGraph::alpha = 255;

void WdgGraph::initializeGL() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glPointSize(4.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
//	glEnable(GL_FOG);

	float fogCol[3] = { 1.0f, 1.0f, 1.0f };
	glFogfv(GL_FOG_COLOR, fogCol);
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogf(GL_FOG_START, 50.f);
	glFogf(GL_FOG_END, 150.f);

	fnt.Load();
}

void WdgGraph::setupGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// projection transformation

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

	// model transformation

	float scale = bounds.Deviation();
	SoaPoint center = bounds.Center();

	glTranslatef(0.0f, 0.0f, -100.0f);
	glScalef(scale, scale, scale);
	glRotatef(rotation.x, 1.0f, 0.0f, 0.0f);
	glRotatef(rotation.y, 0.0f, 0.0f, 1.0f);
	glTranslatef(-center.x, -center.y, -center.z);

	glGetDoublev(GL_MODELVIEW_MATRIX, model);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);
}



void WdgGraph::paintGL() {

	setupGL();

	// nodes

	for (WgNodeVector::iterator node = nodes.begin(); node != nodes.end(); ++node) {
		if (hilitNodes == 0 || node->hi.dir) {
			glPushMatrix();
			glTranslatef(node->p[0], node->p[1], node->p[2]);
			glRotatef(-rotation.y, 0.0f, 0.0f, 1.0f);
			glRotatef(-rotation.x, 1.0f, 0.0f, 0.0f);
            WdgGraph::SetNodeColor(255, node->selected, node->external, node->multiuser);
            if (node->multiuser)
                WdgGraph::DrawNode(6.0f, 4.0f, 2.0f, 0, 0, 3);
            else {
                WdgGraph::DrawMultiuserNode(6, 4, 2, 1);
                WdgGraph::DrawMultiuserNode(6, 4, 2, 2);
            }
			glPopMatrix();
		}
	}

	if (showSubNodes) {

		// sub nodes

		for (WgSubNodeVector::iterator subNode = subNodes.begin(); subNode != subNodes.end(); ++subNode) {
			if (hilitNodes == 0 || subNode->hilit) {
				glPushMatrix();
				glTranslatef(subNode->p[0], subNode->p[1], subNode->p[2]);
				glRotatef(-rotation.y, 0.0f, 0.0f, 1.0f);
				glRotatef(-rotation.x, 1.0f, 0.0f, 0.0f);
				WdgGraph::SetSubNodeColor(255, subNode->selected, subNode->external);
				WdgGraph::DrawSubNode(1.5f, 1.5f, true);
				glPopMatrix();
			}
		}

		// implementation links

		glLineWidth(2.0f);
		glColor4ub(0, 0, 0, 160);
		glBegin(GL_LINES);
		for (WgSubNodeVector::iterator subNode = subNodes.begin(); subNode != subNodes.end(); ++subNode) {
			if (hilitNodes == 0 || subNode->hilit) {
				glVertex3fv(subNode->p);
				glVertex3fv(nodes[subNode->im.in].p);
			}
		}
		glEnd();
		glLineWidth(1.0f);

		// use links

		for (WgLinkList::iterator link = links.begin(); link != links.end(); ++link) {
			if (hilitNodes == 0 || link->hilit) {
				glLineWidth(link->selected ? WG_SEL_LINE_W : 1.0f);
				glColor4ub(0, 0, 0, link->selected ? 255 : 100);
				glBegin(GL_LINES);
				glVertex3fv(nodes[link->us.in].p);
				glVertex3fv(subNodes[link->sb.in].p);
				glEnd();
			}
		}
	}
	else {

		// direct links

		for (WgLinkList::iterator link = links.begin(); link != links.end(); ++link) {
			if (hilitNodes == 0 || link->hilit) {
				glLineWidth(link->selected ? WG_SEL_LINE_W : 1.0f);
				glBegin(GL_LINES);
				glColor4ub(0, 0, 0, link->selected ? 255 : 100);
				glVertex3fv(nodes[link->us.in].p);
				glColor4ub(180, 180, 180, link->selected ? 255 : 100);
				glVertex3fv(nodes[link->im.in].p);
				glEnd();
			}
		}
	}

	glLineWidth(1.0f);

	// transparent stuff

	glDepthMask(false);

	// transparent nodes

	for (WgNodeVector::iterator node = nodes.begin(); node != nodes.end(); ++node) {
		if (hilitNodes && node->hi.dir == 0) {
			glPushMatrix();
			glTranslatef(node->p[0], node->p[1], node->p[2]);
			glRotatef(-rotation.y, 0.0f, 0.0f, 1.0f);
			glRotatef(-rotation.x, 1.0f, 0.0f, 0.0f);
            WdgGraph::SetNodeColor(40, node->selected, node->external, node->multiuser);
            if (node->multiuser)
                WdgGraph::DrawNode(6.0f, 4.0f, 2.0f, 0, 0, 3);
            else {
                WdgGraph::DrawMultiuserNode(6, 4, 2, 1);
                WdgGraph::DrawMultiuserNode(6, 4, 2, 2);
            }
			glPopMatrix();
		}
	}

	if (showSubNodes) {

		// transparent sub nodes

		for (WgSubNodeVector::iterator subNode = subNodes.begin(); subNode != subNodes.end(); ++subNode) {
			if (hilitNodes && !subNode->hilit) {
				glPushMatrix();
				glTranslatef(subNode->p[0], subNode->p[1], subNode->p[2]);
				glRotatef(-rotation.y, 0.0f, 0.0f, 1.0f);
				glRotatef(-rotation.x, 1.0f, 0.0f, 0.0f);
				WdgGraph::SetSubNodeColor(40, subNode->selected, subNode->external);
				WdgGraph::DrawSubNode(1.5f, 1.5f, true);
				glPopMatrix();
			}
		}

		// transparent implementation links

		glLineWidth(2.0f);
		glColor4ub(0, 0, 0, 10);
		glBegin(GL_LINES);
		for (WgSubNodeVector::iterator subNode = subNodes.begin(); subNode != subNodes.end(); ++subNode) {
			if (hilitNodes && !subNode->hilit) {
				glVertex3fv(subNode->p);
				glVertex3fv(nodes[subNode->im.in].p);
			}
		}
		glEnd();
		glLineWidth(1.0f);

		// transparent use links

		glColor4ub(0, 0, 0, 10);
		glBegin(GL_LINES);
		for (WgLinkList::iterator link = links.begin(); link != links.end(); ++link) {
			if (hilitNodes && !link->hilit) {
				glVertex3fv(nodes[link->us.in].p);
				glVertex3fv(subNodes[link->sb.in].p);
			}
		}
		glEnd();
	}
	else {

		// transparent direct links

		glBegin(GL_LINES);
		for (WgLinkList::iterator link = links.begin(); link != links.end(); ++link) {
			if (hilitNodes && !link->hilit) {
				glColor4ub(0, 0, 0, 10);
				glVertex3fv(nodes[link->us.in].p);
				glColor4ub(180, 180, 180, 10);
				glVertex3fv(nodes[link->im.in].p);
			}
		}
		glEnd();
	}

	// walls

	if (solver.Mode() == svm25D) {
		const SoaRect &walls = solver.Walls();
		glColor4ub(0, 0, 0, 60);
		DrawRectangle(GL_LINE_LOOP, walls.o.x, walls.o.y, 0.0f, walls.hW, walls.hH);
		DrawRectangle(GL_LINE_LOOP, walls.o.x, walls.o.y, walls.o.z, walls.hW, walls.hH);
		glColor4ub(0, 0, 0, 10);
		DrawRectangle(GL_QUADS, walls.o.x, walls.o.y, 0.0f, walls.hW, walls.hH);
		DrawRectangle(GL_QUADS, walls.o.x, walls.o.y, walls.o.z, walls.hW, walls.hH);
	}

	//
	// 2D
	//

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, width(), 0.0, height(), -10, 10);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);

	double x, y, z;
	for (WgNodeVector::iterator node = nodes.begin(); node != nodes.end(); ++node) {
		if (showToolTips || node->selected || node->hi.dir != hdNone || node->id == hoverNode) {
			gluProject(node->p[0], node->p[1], node->p[2], model, projection, viewport, &x, &y, &z);
			DrawTooltip(node->title, x, y);
			glLoadIdentity();
		}
	}

	for (WgSubNodeVector::iterator subNode = subNodes.begin(); subNode != subNodes.end(); ++subNode) {
		if (showToolTips || subNode->selected || subNode->id == hoverNode) {
			gluProject(subNode->p[0], subNode->p[1], subNode->p[2], model, projection, viewport, &x, &y, &z);
			DrawTooltip(subNode->title, x, y);
			glLoadIdentity();
		}
	}

	if (showSubNodes) {

		for (WgLinkList::iterator link = links.begin(); link != links.end(); ++link) {
			if (link->selected) {

				WgNode &n1 = nodes[link->us.in], &n2 = subNodes[link->sb.in];

				float _x = (n1.p[0] + n2.p[0]) * 0.5f;
				float _y = (n1.p[1] + n2.p[1]) * 0.5f;
				float _z = (n1.p[2] + n2.p[2]) * 0.5f;

				gluProject(_x, _y, _z, model, projection, viewport, &x, &y, &z);
				DrawTooltip(n1.title + " - " + n2.title, x, y);
				glLoadIdentity();
			}
		}
	}
	else {

		for (WgLinkList::iterator link = links.begin(); link != links.end(); ++link) {
			if (link->selected) {

				WgNode &n1 = nodes[link->us.in], &n2 = nodes[link->im.in];

				float _x = (n1.p[0] + n2.p[0]) * 0.5f;
				float _y = (n1.p[1] + n2.p[1]) * 0.5f;
				float _z = (n1.p[2] + n2.p[2]) * 0.5f;

				gluProject(_x, _y, _z, model, projection, viewport, &x, &y, &z);
				DrawTooltip(n1.title + " - " + n2.title, x, y);
				glLoadIdentity();
			}
		}
	}

	glEnable(GL_DEPTH_TEST);

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glDepthMask(true);
}

void WdgGraph::selectGL(int scrX, int scrY, bool mod, bool commit) {

	setupGL();

	bool selectionChanged = false;

	if (!mod && commit) {
		iv->DeselectAll();
		selectionChanged = true;
	}

	glDisable(GL_BLEND);
	glDisable(GL_LINE_SMOOTH);

	// nodes

	unsigned char nodeIndex = 0;
	for (WgNodeVector::iterator node = nodes.begin(); node != nodes.end(); ++node) {

		glPushMatrix();
		glTranslatef(node->p[0], node->p[1], node->p[2]);
		glRotatef(-rotation.y, 0.0f, 0.0f, 1.0f);
		glRotatef(-rotation.x, 1.0f, 0.0f, 0.0f);
		glColor4ub(13, 0, nodeIndex++, 255);
		if (node->multiuser)
            WdgGraph::DrawNode(6.0f, 4.0f, 2.0f, 0, 0, 1);
		else
            WdgGraph::DrawMultiuserNode(6, 4, 2, 1);
		glPopMatrix();
	}

	if (showSubNodes) {

		// sub nodes

		unsigned char subNodeIndex = 0;
		for (WgSubNodeVector::iterator subNode = subNodes.begin(); subNode != subNodes.end(); ++subNode) {

			glPushMatrix();
			glTranslatef(subNode->p[0], subNode->p[1], subNode->p[2]);
			glRotatef(-rotation.y, 0.0f, 0.0f, 1.0f);
			glRotatef(-rotation.x, 1.0f, 0.0f, 0.0f);
			glColor4ub(13, 1, subNodeIndex++, 255);
			WdgGraph::DrawSubNode(1.5f, 1.5f, false);
			glPopMatrix();
		}

		// use links

		unsigned char useLinkIndex = 0;
		glBegin(GL_LINES);
		for (WgLinkList::iterator link = links.begin(); link != links.end(); ++link) {
			if (hilitNodes == 0 || link->hilit) {
				glColor4ub(13, 2, useLinkIndex++, 255);
				glVertex3fv(nodes[link->us.in].p);
				glVertex3fv(subNodes[link->sb.in].p);
			}
		}
		glEnd();
	}

	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);

	unsigned char pixels[WG_PICK_SIZE * WG_PICK_SIZE * 4];
	glReadPixels(scrX - WG_PICK_HALF, height() - scrY - WG_PICK_HALF, WG_PICK_SIZE, WG_PICK_SIZE, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	// extract best hit

	int p = 0, h = -1;
	float d = 1000.0f;
	for (int x = 0; x < WG_PICK_SIZE; ++x) {
		for (int y = 0; y < WG_PICK_SIZE; ++y) {

			if (pixels[p * 4] == 13) {	// hit

				float _x = WG_PICK_HALF - x;
				float _y = WG_PICK_HALF - y;
				float _d = _x * _x + _y * _y;

				if (_d < d) {
					h = p;
					d = _d;
				}
			}

			++p;
		}
	}

	h *= 4;

	// apply best hit

	if (commit) {
		if (pixels[h + 0] == 13) {
			if (pixels[h + 1] == 0)
				iv->InvertRowSelection(nodes[pixels[h + 2]].id);
			else if (pixels[h + 1] == 1)
				iv->InvertRowSelection(subNodes[pixels[h + 2]].id);
			else if (pixels[h + 1] == 2)
				iv->InvertRowSelection(links[pixels[h + 2]].id);

			selectionChanged = true;
		}

		if (selectionChanged)
			wg->RefreshViews();
	}
	else {
		hoverNode = 13021980;
		if (pixels[h + 0] == 13) {
			switch (pixels[h + 1]) {
			case 0:
				if (pixels[h + 2] < nodes.size())
					hoverNode = nodes[pixels[h + 2]].id;
				break;
			case 1:
				if (pixels[h + 2] < subNodes.size())
					hoverNode = subNodes[pixels[h + 2]].id;
				break;
			case 2:
				if (pixels[h + 2] < links.size())
					hoverNode = links[pixels[h + 2]].id;
				break;
			default:;
			}
		}
	}
}

void WdgGraph::resizeGL(int width, int height) {
	glViewport(0, 0, width, height);
}

//
// static
//

void WdgGraph::SetNodeColor(unsigned char _alpha, bool selected, bool external, bool multiuser) {
	alpha = _alpha;
	if (selected)		glColor4ub(250, 230, 0, _alpha);
	else if (external)	glColor4ub(124, 183, 193, _alpha);
    else if (multiuser)	glColor4ub(240, 0, 0, _alpha);
    else				glColor4ub(234, 94, 33, _alpha);
}

void WdgGraph::SetSubNodeColor(unsigned char _alpha, bool selected, bool external) {
	alpha = _alpha;
	if (selected)		glColor4ub(250, 230, 0, _alpha);
	else if (external)	glColor4ub(124, 183, 193, _alpha);
	else				glColor4ub(20, 140, 255, _alpha);
}

void WdgGraph::DrawRectangle(unsigned int mode, float x, float y, float z, float w, float h) {
	glBegin(mode);
	glVertex3f(x - w, y - h, z);
	glVertex3f(x - w, y + h, z);
	glVertex3f(x + w, y + h, z);
	glVertex3f(x + w, y - h, z);
	glEnd();
}

void WdgGraph::DrawEllipse(unsigned int mode, float z, float w, float h) {
	glBegin(mode);
	for (int i = -5; i <= 5; ++i)
		glVertex3f(cos(i * WG_ANGLE_STEP) * h + w, sin(i * WG_ANGLE_STEP) * h, z);
	for (int i = 5; i <= 15; ++i)
		glVertex3f(cos(i * WG_ANGLE_STEP) * h - w, sin(i * WG_ANGLE_STEP) * h, z);
	glEnd();
}

void WdgGraph::DrawNode(float w, float h, float z, float dx, float dy, int mode) {
	if (mode & 1) {
		WdgGraph::DrawRectangle(GL_QUADS, dx, dy, z, w, h);
		WdgGraph::DrawRectangle(GL_QUADS, -w + dx, h * 0.4f + dy, z + 0.4f, w * 0.2f, h * 0.2f);
		WdgGraph::DrawRectangle(GL_QUADS, -w + dx, -h * 0.4f + dy, z + 0.4f, w * 0.2f, h * 0.2f);
	}
	if (mode & 2) {
		z += 0.02f;
        glColor4ub(255, 255, 255, alpha);
		WdgGraph::DrawRectangle(GL_LINE_LOOP, dx, dy, z, w, h);
		WdgGraph::DrawRectangle(GL_LINE_LOOP, -w + dx, h * 0.4f + dy, z + 0.4f, w * 0.2f, h * 0.2f);
		WdgGraph::DrawRectangle(GL_LINE_LOOP, -w + dx, -h * 0.4f + dy, z + 0.4f, w * 0.2f, h * 0.2f);
	}

}

void WdgGraph::DrawMultiuserNode(float w, float h, float z, int mode) {
	DrawNode(w, h, z, -1.5, -1.5, mode);
	DrawNode(w, h, z - 0.5, 0, 0, mode);
	DrawNode(w, h, z - 1, 1.5, 1.5, mode);
}

void WdgGraph::DrawSubNode(float w, float h, bool edges) {
	WdgGraph::DrawEllipse(GL_POLYGON, 0.0f, w, h);
	if (edges) {
		glColor4ub(0, 0, 0, alpha);
		WdgGraph::DrawEllipse(GL_LINE_LOOP, 0.02f, w, h);
	}
}

void WdgGraph::DrawTooltip(const QString &text, double x, double y) {

	glTranslated(x + TT_MARGIN, y + TT_MARGIN, 0.0);

	int h = fnt.h, w = fnt.w * text.size();

	glColor4ub(250, 230, 200, 200);
	glBegin(GL_QUADS);
	glVertex2i(-TT_MARGIN, -TT_MARGIN);
	glVertex2i(-TT_MARGIN, h + TT_MARGIN);
	glVertex2i(w + TT_MARGIN, h + TT_MARGIN);
	glVertex2i(w + TT_MARGIN, -TT_MARGIN);
	glEnd();

	glColor4ub(0, 0, 0, 255);
	glBegin(GL_LINE_LOOP);
	glVertex2i(-TT_MARGIN, -TT_MARGIN);
	glVertex2i(-TT_MARGIN, h + TT_MARGIN);
	glVertex2i(w + TT_MARGIN, h + TT_MARGIN);
	glVertex2i(w + TT_MARGIN, -TT_MARGIN);
	glEnd();

	glColor4ub(0, 0, 0, 255);
	fnt.PaintText(text.toStdString(), 0.0, 0.0);
}
