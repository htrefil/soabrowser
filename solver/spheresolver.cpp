#include "spheresolver.h"
#include <cmath>



SphereSolver::SphereSolver(float _r, float _lFactor) : r(_r), l(r * _lFactor) {}

void SphereSolver::AddSphere(float *p) {
	spheres.push_back(Sphere(this, p, r, l));
}

void SphereSolver::AddNode(int sphereIndex, float *nodePos) {
	nodeIndices.push_back(spheres[sphereIndex].AddNode(nodePos));
}

void SphereSolver::AddUse(int sphereIndex, int nodeIndex, float *userPos) {
	spheres[sphereIndex].AddUse(nodeIndices[nodeIndex], userPos);
}

void SphereSolver::Solve() {
	for (std::vector<Sphere>::iterator sphere = spheres.begin(); sphere != spheres.end(); ++sphere)
		sphere->Solve();
}

//
// sphere
//

Sphere::Sphere(SphereSolver *_solver, float *_p, float _r, float _l) : r(_r), l(_l), solver(_solver) {
	p[0] = _p[0]; p[1] = _p[1]; p[2] = _p[2];
}

int Sphere::AddNode(float *nodePos) {
	nodes.push_back(SphereNode(nodePos));
	return nodes.size() - 1;
}

void Sphere::AddUse(int nodeIndex, float *userPos) {
	nodes[nodeIndex].AddUse(userPos, p);
}

void Sphere::Solve() {

	for (std::vector<SphereNode>::iterator node = nodes.begin(); node != nodes.end(); ++node) {
		node->s[0] += p[0]; node->s[1] += p[1]; node->s[2] += p[2];
		ProjectOntoSphere(node->s);
		node->Init();
	}

	// create implicit links

	for (int a = 0; a < (int)nodes.size() - 1; ++a)
		for (int b = a + 1; b < (int)nodes.size(); ++b)
			links.push_back(SphereLink(a, b));

	// relax

	for (int i = 0; i < 200; ++i) {

		float maxR = 0.0f, sumR = 0.0f;

		// original positions perception

		for (std::vector<SphereNode>::iterator node = nodes.begin(); node != nodes.end(); ++node) {
			if (node->fixed) {
				node->r[0] = (node->s[0] - node->p[0]);
				node->r[1] = (node->s[1] - node->p[1]);
				node->r[2] = (node->s[2] - node->p[2]);
			}
		}

		// implicit links perception

		for (std::vector<SphereLink>::iterator link = links.begin(); link != links.end(); ++link) {
			SphereNode &n1 = nodes[link->n1];
			SphereNode &n2 = nodes[link->n2];

			float dx = n1.p[0] - n2.p[0], dy = n1.p[1] - n2.p[1], dz = n1.p[2] - n2.p[2];
			float d = sqrt(dx * dx + dy * dy + dz * dz);

			if (d < l) {
				float f = (l - d) / d;

				dx *= f; dy *= f; dz *= f;
				n1.r[0] += dx; n1.r[1] += dy; n1.r[2] += dz;
				n2.r[0] -= dx; n2.r[1] -= dy; n2.r[2] -= dz;
			}
		}

		// get largest force

		for (std::vector<SphereNode>::iterator node = nodes.begin(); node != nodes.end(); ++node) {

			float r = node->r[0] * node->r[0] + node->r[1] * node->r[1] + node->r[2] * node->r[2];
			sumR += r;

			if (r > maxR)
				maxR = r;
		}

		sumR /= (float)links.size();

		// move nodes using normalized force

		if (maxR > 0.0f) {
			maxR = sqrt(maxR) * (100.0f / r);

			for (std::vector<SphereNode>::iterator node = nodes.begin(); node != nodes.end(); ++node) {

				node->p[0] += node->r[0] / maxR;
				node->p[1] += node->r[1] / maxR;
				node->p[2] += node->r[2] / maxR;

				node->r[0] = node->r[1] = node->r[2] = 0.0f;
			}

			for (std::vector<SphereNode>::iterator node = nodes.begin(); node != nodes.end(); ++node)
				ProjectOntoSphere(node->p);
		}
	}
}

// Projects all nodes onto the sphere.
void Sphere::ProjectOntoSphere(float *nodeP) {
	float dx = nodeP[0] - p[0], dy = nodeP[1] - p[1], dz = nodeP[2] - p[2];
	float ratio = r / sqrt(dx * dx + dy * dy + dz * dz);

	for (int dim = 0; dim < 3; ++dim)
		nodeP[dim] = p[dim] + (nodeP[dim] - p[dim]) * ratio;
}

//
// sphere node
//

SphereNode::SphereNode(float *_p) : p(_p), fixed(false) {
	s[0] = (float)rand() / RAND_MAX - 0.5f;
	s[1] = (float)rand() / RAND_MAX - 0.5f;
	s[2] = (float)rand() / RAND_MAX - 0.5f;
	r[0] = r[1] = r[2] = 0.0f;
}

void SphereNode::AddUse(float *userPos, float *spherePos) {
	s[0] += userPos[0] - spherePos[0]; s[1] += userPos[1] - spherePos[1]; s[2] += userPos[2] - spherePos[2];
	fixed = true;
}

void SphereNode::Init() {
	p[0] = s[0]; p[1] = s[1]; p[2] = s[2];
}
