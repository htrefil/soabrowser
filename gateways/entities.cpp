#include "gateways/entities.h"



void GtEntity::SetIndices(int _sId, int _eId) {
	sId = _sId;
	eId = _eId;
}

std::string GtEntity::StatusToString() const {
	switch (status) {
		case gesOld: return "Existing";
		case gesNew: return "New";
		case gesDeleted: return "Deleted";
		case gesUnknown: return "Unknown";
		default: return "Unhandled";
	}
}

//
// service
//

GtService::iterator GtService::Find(const std::string &_i) {

	for (iterator e = begin(); e != end(); ++e)
		if (e->intGivenId == _i)
			return e;

	return end();
}

//
// domain
//

void GtDomain::AddEndpoint(const GtEndpoint &e, unsigned &item) {

	for (iterator s = begin(); s != end(); ++s) {
		if (s->name == e.serviceName && s->protectedEndpoint == e.protectedEndpoint) {
			s->push_back(e);
			return;
		}
	}

	push_back(GtService(item++ * 100 + REPO_TB_IDX, e.serviceName, e.targetNamespace, e.definitionName, e.protectedEndpoint));
	back().push_back(e);
}

GtDomain::iterator GtDomain::Find(const std::string &_name) {

	for (iterator s = begin(); s != end(); ++s)
		if (s->name == _name)
			return s;

	return end();
}

void GtDomain::ClearSelection() {

	for (iterator s = begin(); s != end(); ++s) {
		s->selected = false;
		for (GtService::iterator e = s->begin(); e != s->end(); ++e)
			e->selected = false;
	}
}

void GtDomain::Select(unsigned id) {

	for (iterator s = begin(); s != end(); ++s)
		if (s->viewId == id) {
			s->selected = true;
			return;
		}
}

void GtDomain::SelectEndpoint(unsigned endpointId) {

	for (iterator s = begin(); s != end(); ++s)
		for (GtService::iterator e = s->begin(); e != s->end(); ++e)
			if (e->viewId == endpointId) {
				e->selected = true;
				return;
			}
}

void GtDomain::SelectEndpoint(unsigned serviceId, unsigned endpointId) {

	for (iterator s = begin(); s != end(); ++s)
		if (s->viewId == serviceId) {
			for (GtService::iterator e = s->begin(); e != s->end(); ++e)
				if (e->viewId == endpointId) {
					e->selected = true;
					return;
				}
			return;
		}
}

//
// domains
//

GtDomains::GtDomains(unsigned int regId, const std::string &regName) :
	registryId(regId), registryName(regName) {}

void GtDomains::AddEndpoint(const GtEndpoint &e, unsigned &item) {

	for (iterator d = begin(); d != end(); ++d) {
		if (d->name == e.domain) {
			d->AddEndpoint(e, item);
			return;
		}
	}

	push_back(GtDomain(e.domain));
	back().AddEndpoint(e, item);
}

GtDomain *GtDomains::Find(const std::string &_d) {

	for (iterator d = begin(); d != end(); ++d)
		if (d->name == _d)
			return &(*d);

	return 0;
}
