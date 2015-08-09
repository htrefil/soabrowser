#include "widgets/wdggraph.h"



void WdgGraph::ClearHi() {
	for (WgNodeVector::iterator node = nodes.begin(); node != nodes.end(); ++node) {
		node->hi.dir = hdNone;
		node->hi.iEnd = node->hi.uEnd = false;
	}

	for (WgSubNodeVector::iterator subNode = subNodes.begin(); subNode != subNodes.end(); ++subNode)
		subNode->hilit = false;

	for (WgLinkList::iterator link = links.begin(); link != links.end(); ++link)
		link->hilit = false;

	hilitNodes = 0;
}

void WdgGraph::ExpandHi(HiDir dir) {

	std::vector<WgNode *> iFront, uFront;

	for (WgNodeVector::iterator node = nodes.begin(); node != nodes.end(); ++node) {

		if (node->selected && node->hi.dir != hdAll) {
			if (node->hi.dir == hdNone)
				++hilitNodes;
			node->hi.dir = hdAll;
			node->hi.iEnd = node->hi.uEnd = true;
		}

		WgNode *other;
		WgSubNode *subOther;

		if ((dir & hdImp) && (node->hi.dir & dir) && node->hi.iEnd) {
			for (WgLinkList::iterator link = links.begin(); link != links.end(); ++link)
				if (link->us.id == node->id) {
					if ((other = GetNode(link->im.id))) {
						iFront.push_back(other);
						link->hilit = true;
						if ((subOther = GetSubNode(link->sb.id)))
							subOther->hilit = true;
					}
				}
			node->hi.iEnd = false;
		}

		if ((dir & hdUsr) && (node->hi.dir & dir) && node->hi.uEnd)	{
			for (WgLinkList::iterator link = links.begin(); link != links.end(); ++link)
				if (link->im.id == node->id) {
					if ((other = GetNode(link->us.id))) {
						uFront.push_back(other);
						link->hilit = true;
						if ((subOther = GetSubNode(link->sb.id)))
							subOther->hilit = true;
					}
				}
			node->hi.uEnd = false;
		}

	}

	for (std::vector<WgNode *>::iterator node = iFront.begin(); node != iFront.end(); ++node) {
		if ((*node)->hi.dir == hdNone)
			++hilitNodes;
		(*node)->hi.dir = hdImp;
		(*node)->hi.iEnd = true;
	}

	for (std::vector<WgNode *>::iterator node = uFront.begin(); node != uFront.end(); ++node) {
		if ((*node)->hi.dir == hdNone)
			++hilitNodes;
		(*node)->hi.dir = hdUsr;
		(*node)->hi.uEnd = true;
	}
}

HiFilter WdgGraph::FilterHi() {
	HiFilter hifi;

	for (WgNodeVector::iterator n = nodes.begin(); n != nodes.end(); ++n)
		if (n->hi.dir != hdNone)
			hifi.nodes.insert(n->id);

	for (WgSubNodeVector::iterator sn = subNodes.begin(); sn != subNodes.end(); ++sn)
		if (hifi.nodes.find(sn->im.id) != hifi.nodes.end())
			hifi.subNodes.insert(sn->id);
	//	if (sn->hilit)

	for (WgLinkList::iterator l = links.begin(); l != links.end(); ++l)
		if (l->hilit)
			hifi.links.insert(l->id);

	return hifi;
}
