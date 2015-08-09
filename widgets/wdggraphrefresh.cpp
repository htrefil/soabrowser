#include "widgets/wdggraph.h"
#include "soawidget.h"



void WdgGraph::Refresh(unsigned int _domainId) {

	if (!solver.Running()) {

		domainId = _domainId;

		bool needsRelax = false, needsBoundsUpdate = false, filterActive = hiFilter.Active();

		//
		// refresh nodes
		//

		IgNodeMap nodeMap = iv->NodeMap(fm, domainId);

		WgNodeVector::iterator node = nodes.begin();
		while (node != nodes.end()) {
			IgNodeMap::iterator nodeIt = nodeMap.find(node->id);

			if (nodeIt != nodeMap.end()) {

				node->selected = nodeIt->second.selected;
				node->external = nodeIt->second.external;

				// row found, refresh node

				if (*node != nodeIt->second) {
					node->ts = nodeIt->second.ts;
					node->title = QString::fromUtf8(nodeIt->second.text.c_str());
				}

				nodeMap.erase(nodeIt);
				node++;
			}
			else {

				// node id not found, delete node

				if (node->hi.dir != hdNone)
					--hilitNodes;
				node = nodes.erase(node);
				needsRelax = needsBoundsUpdate = true;
			}
		}

		for (IgNodeMap::iterator nodeIt = nodeMap.begin(); nodeIt != nodeMap.end(); ++nodeIt) {

			if (!filterActive || hiFilter.ContainsNode(nodeIt->first)) {

				nodes.push_back(WgNode(
					nodeIt->first,
					nodeIt->second.ts,
					nodeIt->second.selected,
					nodeIt->second.external,
					QString::fromUtf8(nodeIt->second.text.c_str())));

				needsRelax = needsBoundsUpdate = true;
			}
		}

		//
		// refresh sub nodes
		//

		IgSubNodeMap subNodeMap = iv->SubNodeMap(fm, domainId);

		WgSubNodeVector::iterator subNode = subNodes.begin();
		while (subNode!= subNodes.end()) {
			IgSubNodeMap::iterator subNodeIt = subNodeMap.find(subNode->id);

			if (subNodeIt != subNodeMap.end()) {

				subNode->selected = subNodeIt->second.selected;
				subNode->external = subNodeIt->second.external;

				// row found, refresh node

				if (*subNode != subNodeIt->second) {
					subNode->ts = subNodeIt->second.ts;
					subNode->title = QString::fromUtf8(subNodeIt->second.text.c_str());
					subNode->im.id = subNodeIt->second.im;
				}

				subNodeMap.erase(subNodeIt);
				subNode++;
			}
			else {

				// node id not found, delete node

				subNode = subNodes.erase(subNode);
				needsRelax = needsBoundsUpdate = true;
			}
		}

		for (IgSubNodeMap::iterator subNodeIt = subNodeMap.begin(); subNodeIt != subNodeMap.end(); ++subNodeIt) {

			if (!filterActive || hiFilter.ContainsSubNode(subNodeIt->first)) {

				subNodes.push_back(WgSubNode(
					subNodeIt->first,
					subNodeIt->second.ts,
					subNodeIt->second.selected,
					subNodeIt->second.external,
					QString::fromUtf8(subNodeIt->second.text.c_str()),
					subNodeIt->second.im));

				needsRelax = needsBoundsUpdate = true;
			}
		}

		if (needsBoundsUpdate)
			UpdateBounds();

		//
		// refresh links
		//

		IgLinkMap linkMap = iv->LinkMap(fm, domainId);

		WgLinkList::iterator link = links.begin();
		while (link != links.end()) {
			IgLinkMap::iterator linkIt = linkMap.find(link->id);

			if (linkIt != linkMap.end()) {

				link->selected = linkIt->second.selected;

				// row found, refresh link

				if (*link != linkIt->second) {
					link->ts = linkIt->second.ts;
					link->us.id = linkIt->second.us;
					link->im.id = linkIt->second.im;
					link->sb.id = linkIt->second.sb;

					needsRelax = true;
				}

				linkMap.erase(linkIt);
				link++;
			}
			else {
				// link id not found, delete link
				link = links.erase(link);
				needsRelax = true;
			}
		}

		for (IgLinkMap::iterator linkIt = linkMap.begin(); linkIt != linkMap.end(); ++linkIt) {

			if (!filterActive || hiFilter.ContainsLink(linkIt->first)) {

				links.push_back(WgLink(
					linkIt->first,
					linkIt->second.ts,
					linkIt->second.selected,
					linkIt->second.us,
					linkIt->second.im,
					linkIt->second.sb));

				needsRelax = true;
			}
		}

		//
		// if modified, relax
		//

		if (needsRelax)
			Relax();

		updateGL();
	}
}

