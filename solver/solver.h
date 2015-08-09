#ifndef solver_h
#define solver_h

#include "soamath.h"
#include <QThread>
#include <vector>
#include <map>



enum SolverMode { svm2D = 0, svm25D = 1, svm3D = 2 };

struct Node
{
	float p[3], r[3];
	unsigned char pr, ch; // counts

	Node(float *_p) : pr(0), ch(0) {
		p[0] = _p[0]; p[1] = _p[1]; p[2] = _p[2];
		r[0] = r[1] = r[2] = 0.0f;
	}
};

struct Link
{
	int us, im, sb; // indices

	Link(int _us, int _im, int _sb) : us(_us), im(_im), sb(_sb) {}
};

struct SubNode
{
	float p[3];
	int im;	// index

	SubNode(int _im) : im(_im) {}
};



class Solver : public QThread
{
	Q_OBJECT

public:
	enum SolverState { svsInactive, svsRunning, svsCommitting };

	Solver(SolverMode);
	~Solver();

	bool Running() const;
	const std::vector<Node> &Nodes() const;
	const std::vector<SubNode> &SubNodes() const;
	SolverMode Mode() const;
	const SoaRect &Walls() const;

	void AddNode(const Node &);
	void AddLink(const Link &);
	void AddSubNode(const SubNode &);
	void Resume();
	void Stop();

private:
	volatile SolverMode mode;
	volatile SolverState state;
	volatile bool stop, resume, noCommit;
	volatile int iteration;
	std::vector<Node> nodes;
	std::vector<SubNode> subNodes;
	std::vector<Link> links, impLinks;
	std::map<unsigned int, int> linkHashes; // ToDo: to std::set
	SoaRect walls;

	void Clear();
	void Preprocess();
	bool Step();
	void Postprocess();
	void run();

signals:
	void commit();

private slots:
	void FinishedSlot();
};

unsigned int SymmetricHash(int, int);

#endif
