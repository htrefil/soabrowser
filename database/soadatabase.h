#ifndef soadatabase_h
#define soadatabase_h

#include "database/soatable.h"
#include "interfaces/iviewable.h"
#include <limits>

#define SDB_NULL_INSPECTEE	std::numeric_limits<unsigned int>::max()



typedef std::vector<SoaTb *>	SoaTbVector;

class SoaDb : private SoaTbVector, public IViewable
{
	friend class SoaTb;

public:
	SoaDb();
	~SoaDb();

	unsigned int SelId() const;
	unsigned int ChangeId() const;
	SoaTb *Tb(const std::string &);
	SoaRw *GetHousekeeping();

	bool FromDB(const std::string &);
	void FromSQL(const std::string &);
	void FromXML(const std::string &);
	void ToDB(const std::string &);
	std::string &ToSQL(std::string &, SoaPlType);
	void ResetHousekeeping();

	// iviewable
	IgNodeMap NodeMap(int, unsigned int);
	IgSubNodeMap SubNodeMap(int, unsigned int);
	IgLinkMap LinkMap(int, unsigned int);
	IgDomainMap DomainMap();
	ItItemMap ItemMap(int, unsigned int);
	ItTable ItemTable(int);
	InItem Inspectee();
	GtDomain Services(unsigned int);
    void DeselectAll();
	void InvertRowSelection(unsigned int);
	unsigned int SelectedDomain();
	std::string SelectedDomainName();
	void SelectDomain(unsigned int);
	std::string TbNameFromViewId(unsigned int);
	SoaTb *TbFromViewId(unsigned int);
	MgIntermediaryMap IntermediaryMap(int);
	MgRegistryMap RegistryMap();

	void DeserializeSchemas();

	// soadatabasespecific
	SoaRw *DeployApplicationVersion(unsigned int, unsigned int, bool);

protected:
	unsigned int selId, changeId;
	SoaTb *selTb;

	void Clear();
	void SetChanged();
	bool ResolveKeys();
};

#endif
