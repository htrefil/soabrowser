#ifndef spheresolver_h
#define spheresolver_h

#include <vector>
#include <cstdlib>



class Sphere;

struct SphereNode
{
	float *p, s[3], r[3];
	bool fixed;

	SphereNode(float *);

	void AddUse(float *, float *);
	void Init();
};

struct SphereLink
{
	int n1, n2;

	SphereLink(int _n1, int _n2) : n1(_n1), n2(_n2) {}
};

class SphereSolver;

struct Sphere
{
	float p[3], r, l;
	std::vector<SphereNode> nodes;
	std::vector<SphereLink> links;
	SphereSolver *solver;

	Sphere(SphereSolver *, float *, float, float);

	int AddNode(float *);
	void AddUse(int, float *);
	void Solve();
	void ProjectOntoSphere(float *);
};



class SphereSolver
{
public:
	SphereSolver(float, float);

	void AddSphere(float *);
	void AddNode(int, float *);
	void AddUse(int, int, float *);
	void Solve();

private:
	float r, l;
	std::vector<Sphere> spheres;
	std::vector<int> nodeIndices;	// maps global to local indices
};

#endif
