#include "solver/solver.h"
#include "solver/spheresolver.h"
#include "database/soalogger.h"



// Creates implicit links.
void Solver::Preprocess() {
	for (int a = 0; a < (int)nodes.size() - 1; ++a) {
		for (int b = a + 1; b < (int)nodes.size(); ++b) {

			std::map<unsigned int, int>::iterator match = linkHashes.find(SymmetricHash(a, b));

			if (match == linkHashes.end())
				impLinks.push_back(Link(a, b, -1));
		}

		linkHashes.clear();
	}
}

bool Solver::Step() {

	float maxR = 0.0f, sumR = 0.0f;

	//
	// 3D mode
	//

	if (mode == svm3D) {

		// links perception

		for (std::vector<Link>::iterator link = links.begin(); link != links.end(); ++link) {
			Node &us = nodes[link->us];
			Node &im = nodes[link->im];

			float dx = us.p[0] - im.p[0], dy = us.p[1] - im.p[1], dz = us.p[2] - im.p[2];
			float l = sqrt(dx * dx + dy * dy + dz * dz);

			float f = (50.0f - l) / l;

			dx *= f; dy *= f; dz *= f;
			us.r[0] += dx; us.r[1] += dy; us.r[2] += dz;
			im.r[0] -= dx; im.r[1] -= dy; im.r[2] -= dz;
		}

		// implicit links perception

		for (std::vector<Link>::iterator link = impLinks.begin(); link != impLinks.end(); ++link) {
			Node &us = nodes[link->us];
			Node &im = nodes[link->im];

			float dx = us.p[0] - im.p[0], dy = us.p[1] - im.p[1], dz = us.p[2] - im.p[2];
			float l = sqrt(dx * dx + dy * dy + dz * dz);

			float f = (50.0f - l);

			if (f > 0.0f) {
				f /= l;

				dx *= f; dy *= f; dz *= f;
				us.r[0] += dx; us.r[1] += dy; us.r[2] += dz;
				im.r[0] -= dx; im.r[1] -= dy; im.r[2] -= dz;
			}
		}

		// get largest force

		for (std::vector<Node>::iterator node = nodes.begin(); node != nodes.end(); ++node) {

			float r = node->r[0] * node->r[0] + node->r[1] * node->r[1] + node->r[2] * node->r[2];
			sumR += r;

			if (r > maxR)
				maxR = r;
		}

		// move nodes using normalized force

		if (maxR > 0.0f) {
			maxR = sqrt(maxR);

			for (std::vector<Node>::iterator node = nodes.begin(); node != nodes.end(); ++node) {

				node->p[0] += node->r[0] / maxR;
				node->p[1] += node->r[1] / maxR;
				node->p[2] += node->r[2] / maxR;

				node->r[0] = node->r[1] = node->r[2] = 0.0f;
			}
		}
	}

	//
	// 2D mode
	//

	else if ((mode == svm2D) || (mode == svm25D)) {

		// links perception

		for (std::vector<Link>::iterator link = links.begin(); link != links.end(); ++link) {
			Node &us = nodes[link->us];
			Node &im = nodes[link->im];

			float dx = us.p[0] - im.p[0], dy = us.p[1] - im.p[1];
			float l = sqrt(dx * dx + dy * dy);

			float f = (50.0f - l) / l;

			dx *= f; dy *= f;
			us.r[0] += dx; us.r[1] += dy;
			im.r[0] -= dx; im.r[1] -= dy;
		}

		// implicit links perception

		for (std::vector<Link>::iterator link = impLinks.begin(); link != impLinks.end(); ++link) {
			Node &us = nodes[link->us];
			Node &im = nodes[link->im];

			float dx = us.p[0] - im.p[0], dy = us.p[1] - im.p[1];
			float l = sqrt(dx * dx + dy * dy);

			float f = (50.0f - l);

			if (f > 0.0f) {
				f /= l;

				dx *= f; dy *= f;
				us.r[0] += dx; us.r[1] += dy;
				im.r[0] -= dx; im.r[1] -= dy;
			}
		}

		// get largest force

		for (std::vector<Node>::iterator node = nodes.begin(); node != nodes.end(); ++node) {

			float r = node->r[0] * node->r[0] + node->r[1] * node->r[1];
			sumR += r;

			if (r > maxR)
				maxR = r;
		}

		// move nodes using normalized force

		if (maxR > 0.0f) {
			maxR = sqrt(maxR);

			for (std::vector<Node>::iterator node = nodes.begin(); node != nodes.end(); ++node) {

				node->p[0] += node->r[0] / maxR;
				node->p[1] += node->r[1] / maxR;

				node->r[0] = node->r[1] = 0.0f;
			}
		}
	}

	int linksCount = links.size() + impLinks.size();
	return (linksCount) ? (sumR / (float)linksCount) < 0.1f : true; // stop condition
}

void Solver::Postprocess() {
	if (mode == svm25D) {

		// get parents & children counts for each node

		for (std::vector<Link>::iterator link = links.begin(); link != links.end(); ++link) {
			nodes[link->us].ch++;
			nodes[link->im].pr++;
		}

		// get z range

		SoaMinmax x, y;
		for (std::vector<Node>::iterator node = nodes.begin(); node != nodes.end(); ++node) {
			x.Absorb(node->p[0]);
			y.Absorb(node->p[1]);
		}

		float xRange = x.Range(), yRange = y.Range();
		float zRange = (xRange + yRange) * 0.5f * 0.75f;

		// set each node's z coordinates

		for (std::vector<Node>::iterator node = nodes.begin(); node != nodes.end(); ++node) {
			if (node->pr == node->ch)
				node->p[2] = zRange * 0.5f;
			else if (node->pr == 0)
				node->p[2] = zRange;
			else if (node->ch == 0)
				node->p[2] = 0.0f;
			else
				node->p[2] = zRange * (float)node->pr / (float)(node->pr + node->ch);
		}

		if (zRange == 0.0) {
			xRange = yRange = zRange = 1.0f;
		}

		walls = SoaRect(x.Mid(), y.Mid(), zRange, xRange * 0.5f, yRange * 0.5f);
	}

	//
	// sub nodes
	//

	SphereSolver solver(10.0f, 1.0f);

	// create a sphere for each node

	for (std::vector<Node>::iterator node = nodes.begin(); node != nodes.end(); ++node)
		solver.AddSphere(node->p);

	// add sub nodes to respective spheres

	for (std::vector<SubNode>::iterator subNode = subNodes.begin(); subNode != subNodes.end(); ++subNode)
		solver.AddNode(subNode->im, subNode->p);

	// add uses

	for (std::vector<Link>::iterator link = links.begin(); link != links.end(); ++link)
		solver.AddUse(link->im, link->sb, nodes[link->us].p);

	// solve spheres

	solver.Solve();
}
