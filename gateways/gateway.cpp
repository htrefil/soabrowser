#include "gateways/gateway.h"



SoaGtwy::SoaGtwy() :
	registryId(0),
	op(0),
	domain(0),
	service(0),
	loaded(false),
	domains(13021980, "") {}

bool SoaGtwy::Loaded() const {
	return loaded;
}

GtDomains SoaGtwy::ConstructTree() {

	GtDomains o(registryId, SQ(registryName));

	for (GtwyEndpoints::const_iterator e = endpoints.begin(); e != endpoints.end(); ++e)
		o.AddEndpoint(*e, item);

	return o;
}

void SoaGtwy::Reset() {
	op = 0;
	domain = service = item = 0;
	loaded = false;
	domains.clear();
	endpoints.clear();
}

//
// iterators
//

bool SoaGtwy::SelectFirstDomain() {
	domain = 0;
	return domains.size();
}

bool SoaGtwy::SelectNextDomain() {
	return ++domain < domains.size();
}

GtDomain &SoaGtwy::SelectedDomain() {
	return domains[domain];
}

bool SoaGtwy::SelectFirstEndpoint() {
	service = 0;
	return endpoints.size();
}

bool SoaGtwy::SelectNextEndpoint() {
	return ++service < endpoints.size();
}

GtEndpoint &SoaGtwy::SelectedEndpoint() {
	return endpoints[service];
}

void SoaGtwy::RemoveSelectedEndpoint() {
	endpoints.erase(endpoints.begin() + service);
	--service;
}
