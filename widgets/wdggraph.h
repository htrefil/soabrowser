#ifndef wdggraph_h
#define wdggraph_h

#include "interfaces/iviewable.h"
#include "interfaces/iview.h"
#include "solver/solver.h"
#include "soamath.h"
#include "fonts/font.h"
#include <QGLWidget>
#include <QThread>
#include <QTime>
#include <QMenu>
#include <GL/glu.h>
#include <map>
#include <set>



enum HiDir { hdNone = 0, hdImp = 1, hdUsr = 2, hdAll = 3 };

struct IdInPair	// asset id/index pair (id fixed at creation, index updated for solve & drawing)
{
	unsigned int id;
	int in;
	IdInPair(unsigned int _id) : id(_id), in(-1) {}
};

struct HiData
{
	HiDir dir;
	bool iEnd, uEnd;
	HiData() : dir(hdNone), iEnd(false), uEnd(false) {}
};

struct HiFilter
{
	std::set<unsigned int> nodes, subNodes, links;

	bool Active() const {
		return nodes.size() || subNodes.size() || links.size();
	}

	bool ContainsNode(unsigned int id) const {
		return nodes.find(id) != nodes.end();
	}

	bool ContainsSubNode(unsigned int id) const {
		return subNodes.find(id) != subNodes.end();
	}

	bool ContainsLink(unsigned int id) const {
		return links.find(id) != links.end();
	}
};



class WgNode	// application asset
{
public:
	float p[3];
	unsigned int id, ts;
	bool selected, external, multiuser;
	QString title;
	HiData hi;

	WgNode(unsigned int _id, unsigned int _ts, bool _selected, bool _external, bool _multiuser, const QString &_title) :
		id(_id), ts(_ts), selected(_selected), external(_external), multiuser(_multiuser), title(_title) {

		p[0] = rand() * 100.0f / RAND_MAX;
		p[1] = rand() * 100.0f / RAND_MAX;
		p[2] = rand() * 100.0f / RAND_MAX;
	}

	void Set(const float *_p) {
		p[0] = _p[0]; p[1] = _p[1]; p[2] = _p[2];
	}

	bool operator!=(const IgNode &ig) const {
		return ts != ig.ts;
	}
};

class WgSubNode : public WgNode	// service asset
{
public:
	IdInPair im;
	bool hilit;

	WgSubNode(unsigned _id, unsigned _ts, bool _selected, bool _external, const QString &_title, unsigned int _imId) :
		WgNode(_id, _ts, _selected, _external, false, _title), im(_imId), hilit(false) {}

	bool operator!=(const IgSubNode &ig) const {
		return ts != ig.ts || im.id != ig.im;
	}
};

class WgLink	// use asset
{
public:
	unsigned int id, ts;
	bool selected, hilit;
	IdInPair im, us, sb;	// implementator, user, subject

	WgLink(unsigned int _id, unsigned int _ts, bool _selected, unsigned int _usId, unsigned int _imId, unsigned int _sbId) :
		id(_id), ts(_ts), selected(_selected), hilit(false), im(_imId), us(_usId), sb(_sbId) {}

	bool operator!=(const IgLink &ig) const {
		return ts != ig.ts || im.id != ig.im || us.id != ig.us || sb.id != ig.sb;
	}
};



class SoaWg;
class IViewable;

typedef std::vector<WgLink>		WgLinkList;
typedef std::vector<WgNode>		WgNodeVector;
typedef std::vector<WgSubNode>	WgSubNodeVector;

class WdgGraph : public QGLWidget
{
	Q_OBJECT

public:
	WdgGraph(SoaWg *, IViewable *, FilterMode, SolverMode, const HiFilter &);

	void Refresh(unsigned int);
	void Relax();
	void ShowSubNodes(bool);
	void ShowToolTips(bool);

	void ClearHi();
	void ExpandHi(HiDir);
	HiFilter FilterHi();

private:
	static unsigned char alpha;

	SoaWg *wg;
	IViewable *iv;
	FilterMode fm;
	Solver solver;
	HiFilter hiFilter;
	WgNodeVector nodes;
	WgLinkList links;
	WgSubNodeVector subNodes;
	SoaPosition rotation, mousePos;
	SoaBounds bounds;
	SoaPoint view;
	QMenu *menu;
	QTime hover;
	bool showSubNodes, showToolTips;
	int mouseTravel, hilitNodes, viewport[4], ht1, ht2;
	unsigned int hoverNode, domainId;
	double projection[16], model[16];
	TextureFont fnt;

	void initializeGL();
	void setupGL();
	void paintGL();
	void selectGL(int, int, bool, bool);
	void resizeGL(int width, int height);
	void mouseDoubleClickEvent(QMouseEvent *);
	void mousePressEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	void wheelEvent(QWheelEvent *);
	void timerEvent(QTimerEvent *);
	void dragEnterEvent(QDragEnterEvent *);
	void dropEvent(QDropEvent *);

	bool UpdateIndices();
	void UpdateBounds();
	WgNode *GetNode(unsigned int);
	WgSubNode *GetSubNode(unsigned int);

	template<typename T>
	int GetIndex(unsigned int id, const std::vector<T> &vec) {
		int in = 0;
		for (typename std::vector<T>::const_iterator node = vec.begin(); node != vec.end(); ++node)
			if (node->id == id)	return in;
			else				in++;

		return -1;
	}

    static void SetNodeColor(unsigned char, bool, bool, bool);
	static void SetSubNodeColor(unsigned char, bool, bool);
	static void DrawRectangle(unsigned int, float, float, float, float, float);
	static void DrawEllipse(unsigned int, float, float, float);
	static void DrawNode(float, float, float, float, float, int);
	static void DrawMultiuserNode(float, float, float, int);
	static void DrawSubNode(float, float, bool);
	void DrawTooltip(const QString &, double, double);

private slots:
	void CommitSlot();
	void ClearHiSlot();
	void ExpandSlot();
	void ExpandImpSlot();
	void ExpandUsrSlot();
	void CloneHi2Slot();
	void CloneHi25Slot();
	void CloneHi3Slot();
};



class QComboBox;

class WdgGraphContainer : public IView
{
	Q_OBJECT

public:
	WdgGraphContainer(SoaWg *, IViewable *, FilterMode, SolverMode, const HiFilter &, QAction *, QAction *);

	QString Refresh();
	int IsSingleton() { return ivsNone; }

private:
	SoaWg *wg;
	IViewable *iv;
	WdgGraph *graph;
	bool guard;

public slots:
	void SubNodesCheckChangedSlot(int);
	void ToolTipsCheckChangedSlot(int);
};

#endif
