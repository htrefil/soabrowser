#include "solver/solver.h"
#include "database/soalogger.h"



Solver::Solver(SolverMode _mode) : QThread(), mode(_mode), state(svsInactive), stop(false), resume(false), noCommit(false), iteration(0) {
	QObject::connect(this, SIGNAL(finished()), this, SLOT(FinishedSlot()));
}

Solver::~Solver() {
	Stop();
}

bool Solver::Running() const {
	return state != svsInactive;
}

const std::vector<Node> &Solver::Nodes() const {
	return nodes;
}

const std::vector<SubNode> &Solver::SubNodes() const {
	return subNodes;
}

SolverMode Solver::Mode() const {
	return mode;
}

const SoaRect &Solver::Walls() const {
	return walls;
}

void Solver::Clear() {
	nodes.clear();
	subNodes.clear();
	links.clear();
	impLinks.clear();
	linkHashes.clear();
	state = svsInactive;
	stop = resume = noCommit = false;
	iteration = 0;
}

// Adds a node to the solver. Should only be
// called between solver Clear() and Prepare()
// calls.
void Solver::AddNode(const Node &node) {
	nodes.push_back(node);
	if (mode != svm3D)
		nodes.back().p[2] = 0.0f;
}

// Adds a link to the solver. Should only be
// called between solver Clear() and Prepare()
// calls.
void Solver::AddLink(const Link &link) {
	linkHashes.insert(std::pair<unsigned int, int>(SymmetricHash(link.us, link.im), links.size()));
	links.push_back(link);
}

// Adds a node to the solver. Should only be
// called between solver Clear() and Prepare()
// calls.
void Solver::AddSubNode(const SubNode &node) {
	subNodes.push_back(node);
}

// Used to notify the solve thread that the
// commit is finished, and solve can be resumed.
void Solver::Resume() {
	if (state == svsCommitting)
		resume = true;
}

// Sets the stop flag to true, and waits for
// the running thread to terminate.
void Solver::Stop() {
	if (state != svsInactive) {
		stop = noCommit = true;
		wait();
	}
}

// Thread function. Called by the
// QThread object.
void Solver::run() {

	state = svsRunning;

	Preprocess();

	while (!stop && (iteration < 500)) {

		int i1 = iteration;
		while (!stop && (iteration - i1 < 300)) {
			stop = Step();
			++iteration;
		}

		if (stop)
			break;

		state = svsCommitting;
		resume = false;

		sl::log << "started commit" << sl::end;

		emit commit();

		int sleeps = 0;
		while (!resume) {
			msleep(10);
			sleeps++;
		}

		sl::log << "ended commit in: " << sleeps << sl::end;

		state = svsRunning;
	}

	Postprocess();
}

// This method is called by the QThread
// object when the run() method finishes.
void Solver::FinishedSlot() {

	state = svsCommitting;

	if (!noCommit) {
		sl::log << "thread finish commit" << sl::end;

		emit commit();
	}

	Clear();
}

//
// utilities
//

// For unsigned 4 byte int limit of about 4*10^9
// node count must be less than 65000.
unsigned int SymmetricHash(int a, int b) {
	return (a < b) ? a + b * 65000 : b + a * 65000;
}
