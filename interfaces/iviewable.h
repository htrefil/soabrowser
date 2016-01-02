#ifndef iviewable_h
#define iviewable_h

#include "gateways/entities.h"
#include <string>
#include <vector>
#include <map>



enum FilterMode
{
	fmOutline = 0,
	fmRealization,
	fmRuntime,
	fmUndefined
};



class SoaRw;

//
// graph-specific structs
//

struct IgNode
{
	unsigned int ts;
	bool selected, external, multiuser;
	std::string text;
	IgNode(unsigned int _ts, bool _selected, bool _external, bool _multiuser, const std::string &_text) :
		ts(_ts), selected(_selected), external(_external), multiuser(_multiuser), text(_text) {}
};

struct IgSubNode
{
	unsigned int ts;
	bool selected, external;
	std::string text;
	unsigned int im;
	IgSubNode(unsigned int _ts, bool _selected, bool _external, const std::string &_text, unsigned int _im) :
		ts(_ts), selected(_selected), external(_external), text(_text), im(_im) {}
};

struct IgLink
{
	unsigned int ts;
	bool selected;
	unsigned int us, im, sb;
	IgLink(unsigned int _ts, bool _selected, unsigned int _us, unsigned int _im, unsigned int _sb) :
		ts(_ts), selected(_selected), us(_us), im(_im), sb(_sb) {}
};

struct IgDomain
{
	std::string text;
	bool selected;
	IgDomain(const std::string &_text, bool _selected) : text(_text), selected(_selected) {}
};

typedef std::map<unsigned int, IgNode>		IgNodeMap;
typedef std::pair<unsigned int, IgNode>		IgNodePair;
typedef std::map<unsigned int, IgSubNode>	IgSubNodeMap;
typedef std::pair<unsigned int, IgSubNode>	IgSubNodePair;
typedef std::map<unsigned int, IgLink>		IgLinkMap;
typedef std::pair<unsigned int, IgLink>		IgLinkPair;
typedef std::map<unsigned int, IgDomain>	IgDomainMap;
typedef std::pair<unsigned int, IgDomain>	IgDomainPair;

//
// tree-specific structs
//

enum ItItemIcon { iiiOrg, iiiOrg2, iiiUnit, iiiUnit2, iiiApp, iiiApp1, iiiApp2, iiiServ, iiiServ1, iiiServ2, iiiEnd, iiiEnd2 };

struct ItItem
{
	unsigned int ts;
	bool selected;
	std::string text;
	std::map<unsigned int, ItItem> children;
	bool enabled;
	int icon, sortby;
	ItItem(unsigned int _ts, bool _selected, const std::string &_text, bool _enabled = true, int _icon = -1, int _sortby = 1) :
		ts(_ts), selected(_selected), text(_text), enabled(_enabled), icon(_icon), sortby(_sortby) {}
};

class ItItemMap : public std::map<unsigned int, ItItem>
{
public:
	unsigned int in;
	ItItemMap(unsigned int _in) : in(_in) {}
};
typedef std::pair<unsigned int, ItItem>	ItItemPair;

//
// table-specific structs
//

struct ItRow
{
	unsigned int ts;
	bool selected;
	std::vector<std::string> texts;
	ItRow(unsigned int _ts, bool _selected, const std::vector<std::string> &_texts) :
		ts(_ts), selected(_selected), texts(_texts) {}
};

typedef std::map<unsigned int, ItRow>	ItRowMap;
typedef std::pair<unsigned int, ItRow>	ItRowPair;

struct ItTable
{
	std::string name;
	std::vector<std::string> columns;
	std::vector<int> styles;
	ItRowMap rows;
};

//
// inspector-specific structs
//

struct InItem
{
	SoaRw *row;
	std::string text;
	InItem(SoaRw *_row, const std::string &_text) : row(_row), text(_text) {}
};

//
// intermediary management
//

struct MgIntermediary
{
	std::string name, sdName;
	int registry;
	MgIntermediary(const std::string &_name) :
		name(_name), registry(-1) {}
	MgIntermediary(const std::string &_name, const std::string &_sdName, int _r) :
		name(_name), sdName(_sdName), registry(_r) {}
};

typedef std::map<unsigned int, MgIntermediary> MgIntermediaryMap;
typedef std::pair<unsigned int, MgIntermediary> MgIntermediaryPair;

//
// registry management
//

struct MgRegistry
{
	std::string name;
	MgIntermediaryMap intermediaries;
	MgRegistry(const std::string &_name) : name(_name) {}
};

typedef std::map<unsigned int, MgRegistry> MgRegistryMap;
typedef std::pair<unsigned int, MgRegistry> MgRegistryPair;



class IViewable
{
public:
	// graph

	virtual IgNodeMap NodeMap(int, unsigned int) = 0;
	virtual IgSubNodeMap SubNodeMap(int, unsigned int) = 0;
	virtual IgLinkMap LinkMap(int, unsigned int) = 0;
	virtual IgDomainMap DomainMap() = 0;

	// tree

	virtual ItItemMap ItemMap(int, unsigned int) = 0;

	// table

	virtual ItTable ItemTable(int) = 0;

	// inspector

	virtual InItem Inspectee() = 0;

	// intermediary

	virtual GtDomain Services(unsigned int) = 0;

	// selection

	virtual void DeselectAll() = 0;
	virtual void InvertRowSelection(unsigned int) = 0;
	virtual unsigned int SelectedDomain() = 0;
	virtual std::string SelectedDomainName() = 0;
	virtual void SelectDomain(unsigned int) = 0;

	// other

	virtual std::string TbNameFromViewId(unsigned int) = 0;
	virtual MgIntermediaryMap IntermediaryMap(int) = 0;
	virtual MgRegistryMap RegistryMap() = 0;
};

#endif
