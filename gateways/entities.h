#ifndef entities_h
#define entities_h

#include <string>
#include <vector>

#define REPO_TB_IDX	99



enum GtEntityStatus { gesOld = 0, gesNew, gesDeleted, gesUnknown };

class GtEntity
{
public:
	GtEntityStatus status;
	unsigned int viewId;
	bool selected;
	int sId, eId;

	GtEntity(GtEntityStatus _s, unsigned int _i, bool _selected) :
		status(_s), viewId(_i), selected(_selected) {}

	void SetIndices(int _sId, int _eId = -1);
	std::string StatusToString() const;
};



class GtEndpoint : public GtEntity
{
public:
	std::string domain, urlRegistryFolder, name, urlWSDL;
	std::string targetNamespace, definitionName, serviceName;
	std::string intGivenName, intGivenVersion, intGivenId, policyVersion, protectedEndpoint;
	std::string enabled, soap, resolutionPath;

	GtEndpoint(unsigned int _id, bool _selected, const std::string &_intGivenId) : GtEntity(gesOld, _id, _selected),
		intGivenId(_intGivenId) {}

	GtEndpoint(unsigned _id, const std::string &_domain, const std::string &_href, const std::string &_name) : GtEntity(gesUnknown, _id, false),
		domain(_domain), urlRegistryFolder(_href), name(_name) {}
};

typedef std::vector<GtEndpoint> GtwyEndpoints;



class GtService : public GtwyEndpoints, public GtEntity
{
public:
	std::string name;
	std::string targetNamespace, definitionName, protectedEndpoint;	// from endpoints

	GtService(unsigned _id, const std::string &_name, const std::string &_tn, const std::string &_dn, const std::string &_pe) :
		GtEntity(gesUnknown, _id, false),
		name(_name),
		targetNamespace(_tn),
		definitionName(_dn),
		protectedEndpoint(_pe) {}

	GtService(unsigned int _id, bool _selected, const std::string &_name) : GtEntity(gesOld, _id, _selected),
		name(_name) {}

	iterator Find(const std::string &);
};



class GtDomain : public std::vector<GtService>
{
public:
	std::string href, name;

	GtDomain() :
		href(""), name("") {}

	GtDomain(const std::string &_name) :
		href(""), name(_name) {}

	GtDomain(const std::string &_href, const std::string &_name) :
		href(_href), name(_name) {}

	void AddEndpoint(const GtEndpoint &, unsigned &);
	iterator Find(const std::string &);
	void ClearSelection();
	void Select(unsigned);
	void SelectEndpoint(unsigned);
	void SelectEndpoint(unsigned, unsigned);
};



class GtDomains : public std::vector<GtDomain>
{
public:
	unsigned int registryId;
	std::string registryName;

	GtDomains(unsigned int, const std::string &);

	void AddEndpoint(const GtEndpoint &, unsigned &);
	GtDomain *Find(const std::string &);
};


#endif
