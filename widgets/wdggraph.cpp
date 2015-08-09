#include "soawidget.h"
#include "widgets/wdggraph.h"
#include "solver/solver.h"
#include "database/soalogger.h"
#include <QVBoxLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QToolBar>



WdgGraph::WdgGraph(SoaWg *_wg, IViewable *_iv, FilterMode _fm, SolverMode sm, const HiFilter &_hiFilter) : QGLWidget(_wg),
	wg(_wg), iv(_iv), fm(_fm), solver(sm), hiFilter(_hiFilter),
	rotation(-90.0f, 0.0f), view(0.0f, 0.0f, 1.0f),
	menu(new QMenu(this)),
	showSubNodes(true), showToolTips(hiFilter.Active()),
	mouseTravel(0), hilitNodes(0), ht1(0), ht2(0), hoverNode(13021980), domainId(IV_NULL_DOMAIN) {

	setContextMenuPolicy(Qt::CustomContextMenu);
	setMouseTracking(true);
	setAcceptDrops(true);

	addAction(menu->addAction("Clear highlight", this, SLOT(ClearHiSlot()), QKeySequence("Ctrl+D")));
	menu->addSeparator();
	addAction(menu->addAction("Expand", this, SLOT(ExpandSlot()), QKeySequence("Ctrl+Shift+X")));
	addAction(menu->addAction("Expand to used", this, SLOT(ExpandImpSlot()), QKeySequence("Ctrl+X")));
	addAction(menu->addAction("Expand to users", this, SLOT(ExpandUsrSlot()), QKeySequence("Shift+X")));
	menu->addSeparator();
	addAction(menu->addAction("Clone highlight (2D)", this, SLOT(CloneHi2Slot())));
	addAction(menu->addAction("Clone highlight (2.5D)", this, SLOT(CloneHi25Slot())));
	addAction(menu->addAction("Clone highlight (3D)", this, SLOT(CloneHi3Slot())));

	QObject::connect(&solver, SIGNAL(commit()), this, SLOT(CommitSlot()));

	hover.start();
	startTimer(200);
}

void WdgGraph::ShowSubNodes(bool _showSubNodes) {
	showSubNodes = _showSubNodes;
	updateGL();
}

void WdgGraph::ShowToolTips(bool _showToolTips) {
	showToolTips = _showToolTips;
	updateGL();
}

// Fills the solver with data, and starts it in
// another thread.
void WdgGraph::Relax() {

	if (UpdateIndices()) {

		// add nodes

		for (WgNodeVector::iterator node = nodes.begin(); node != nodes.end(); ++node)
			solver.AddNode(Node(node->p));

		// add links

		for (WgLinkList::iterator link = links.begin(); link != links.end(); ++link)
			solver.AddLink(Link(link->us.in, link->im.in, link->sb.in));

		// add sub nodes

		for (WgSubNodeVector::iterator node = subNodes.begin(); node != subNodes.end(); ++node)
			solver.AddSubNode(SubNode(node->im.in));

		// start solver

		solver.start();
	}
	else
		sl::log << "WdgGraph::Relax() error: one or more node/link connection ids are invalid, aborting relax" << sl::end;
}

// Updates link indices by matching node or
// sub node ids. All indices *must* be valid!
bool WdgGraph::UpdateIndices() {

	for (WgLinkList::iterator link = links.begin(); link != links.end(); ++link)
		if ((link->im.in = GetIndex<WgNode>(link->im.id, nodes)) == -1 ||
			(link->us.in = GetIndex<WgNode>(link->us.id, nodes)) == -1 ||
			(link->sb.in = GetIndex<WgSubNode>(link->sb.id, subNodes)) == -1)
			return false;

	for (WgSubNodeVector::iterator subNode = subNodes.begin(); subNode != subNodes.end(); ++subNode)
		if ((subNode->im.in = GetIndex<WgNode>(subNode->im.id, nodes)) == -1)
			return false;

	return true;
}

// Updates 3D bounds of currently visible
// nodes. Ignores sub nodes.
void WdgGraph::UpdateBounds() {

	bounds.Reset();
	for (WgNodeVector::iterator node = nodes.begin(); node != nodes.end(); ++node)
		bounds.Absorb(node->p);
}

WgNode *WdgGraph::GetNode(unsigned int nodeId) {
	for (WgNodeVector::iterator node = nodes.begin(); node != nodes.end(); ++node)
		if (node->id == nodeId)
			return &(*node);

	return 0;
}

WgSubNode *WdgGraph::GetSubNode(unsigned int subNodeId) {
	for (WgSubNodeVector::iterator subNode = subNodes.begin(); subNode != subNodes.end(); ++subNode)
		if (subNode->id == subNodeId)
			return &(*subNode);

	return 0;
}

//
// slots
//

void WdgGraph::CommitSlot() {

	// copy data from solver

	WgNodeVector::iterator node = nodes.begin();
	for (std::vector<Node>::const_iterator solverNode = solver.Nodes().begin(); solverNode != solver.Nodes().end(); ++solverNode, ++node)
		node->Set(solverNode->p);

	WgSubNodeVector::iterator subNode = subNodes.begin();
	for (std::vector<SubNode>::const_iterator solverSubNode = solver.SubNodes().begin(); solverSubNode != solver.SubNodes().end(); ++solverSubNode, ++subNode)
		subNode->Set(solverSubNode->p);

	// resume solver

	solver.Resume();

	// draw changes

	UpdateBounds();
	updateGL();
}

void WdgGraph::ClearHiSlot() {
	ClearHi();
	updateGL();
}

void WdgGraph::ExpandSlot() {
	ExpandHi(hdAll);
	updateGL();
}

void WdgGraph::ExpandImpSlot() {
	ExpandHi(hdImp);
	updateGL();
}

void WdgGraph::ExpandUsrSlot() {
	ExpandHi(hdUsr);
	updateGL();
}

void WdgGraph::CloneHi2Slot() {
	if (hilitNodes)
		wg->AddGraph(svm2D, FilterHi());
}

void WdgGraph::CloneHi25Slot() {
	if (hilitNodes)
		wg->AddGraph(svm25D, FilterHi());
}

void WdgGraph::CloneHi3Slot() {
	if (hilitNodes)
		wg->AddGraph(svm3D, FilterHi());
}

//
// container
//

WdgGraphContainer::WdgGraphContainer(SoaWg *_wg, IViewable *_iv, FilterMode fm, SolverMode sm, const HiFilter &hifi, QAction *addUseAction, QAction *deleteUseAction) :
	IView(_wg), wg(_wg), iv(_iv),
	graph(new WdgGraph(wg, iv, fm, sm, hifi)),
	guard(false) {

	QCheckBox *subNodeCheck = new QCheckBox(this);
	subNodeCheck->setText("Services");
	subNodeCheck->setChecked(true);

	QCheckBox *toolTipCheck = new QCheckBox(this);
	toolTipCheck->setText("Tooltips");
	toolTipCheck->setChecked(hifi.Active());

	QToolBar *toolbar = new QToolBar(this);
	toolbar->addAction(addUseAction);
	toolbar->addAction(deleteUseAction);
	toolbar->addWidget(subNodeCheck);
	toolbar->addWidget(toolTipCheck);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	layout->addWidget(toolbar);
	layout->addWidget(graph);

	switch (sm) {
		case svm2D:		setWindowTitle("2D graph"); break;
		case svm25D:	setWindowTitle("2.5D graph"); break;
		case svm3D:		setWindowTitle("3D graph"); break;
		default:		setWindowTitle("Graph"); break;
	}

	QObject::connect(subNodeCheck, SIGNAL(stateChanged(int)), this, SLOT(SubNodesCheckChangedSlot(int)));
	QObject::connect(toolTipCheck, SIGNAL(stateChanged(int)), this, SLOT(ToolTipsCheckChangedSlot(int)));
}

QString WdgGraphContainer::Refresh() {

	guard = true;

	// refresh graph

	graph->Refresh(iv->SelectedDomain());

	guard = false;

	return windowTitle();
}

void WdgGraphContainer::SubNodesCheckChangedSlot(int state) {
	graph->ShowSubNodes(state == Qt::Checked);
}

void WdgGraphContainer::ToolTipsCheckChangedSlot(int state) {
	graph->ShowToolTips(state == Qt::Checked);
}
