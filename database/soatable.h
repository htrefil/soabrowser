#ifndef soatable_h
#define soatable_h

#include "database/soapattern.h"
#include "database/soacolumn.h"
#include "database/soahints.h"
#include "database/soarow.h"
#include <vector>
#include <map>

#define DEFAULT_INTERMEDIARY_NAME	"no_intermediary"
#define DEFAULT_ENDPOINT_NAME		"direct_endpoint"



class SoaDb;
class SoaTb;
class SoaSc;
class TiXmlDocument;
class TiXmlElement;
struct sqlite3;

typedef std::vector<SoaCl>		SoaClVector;
typedef std::vector<SoaTb *>	SoaTbVector;

class SoaTb : public SoaRwList // ToDo - make SoaRwList protected
{
	friend class SoaDb;
	friend class SoaRw;
	friend class SoaPt;
	friend class SoaCl;

public:
	SoaTb(SoaDb *, const std::string &);
	~SoaTb();

	const std::string &Name() const;
	const SoaClVector &Columns() const;
	const SoaClVector &AuxColumns() const;
	int RowsCount() const;
	bool IsDependency(SoaTb *) const;

	// soatablerows
	SoaRw *AllocRw(int, bool);
	void AddRw(SoaRw *);
	bool IsRwValid(SoaRw *);
	SoaRw *Rw(int);
	SoaRw *Rw(unsigned int, const std::string &);
	SoaRw *RwByViewId(unsigned int);
	SoaRw *FocusRw();
	SoaRw *FrontRw();
    void SetFocusedRwByViewId(unsigned int);
	bool InvertRowSelection(unsigned int, unsigned int);
	bool DeleteRowByViewId(unsigned int);

	// schema
	bool IsSchemaValid() const;
	void LoadSchema(const std::string &);
	void DeserializeSchema(const std::string &);
	bool ApplyNewSchemaXml(TiXmlDocument *);

	// portfolio
	void LoadPortfolio(const std::string &);

	SoaPt &Search();

private:
	SoaDb *db;
	SoaRw *focusRw;
	std::string name;
	std::map<int, int> rwNameCls;
	SoaPt pattern;
	SoaClVector columns, auxColumns;
	SoaTbVector chClasses;
	unsigned int index, nextViewId;
	TableColumnHints tableColumnHints;

	void Clear();
	void SetChanged();
	unsigned int NextViewId();

	int GetAvailableId() const;
	int ChClassIndex(const std::string &) const;
	int ChClassIndex(SoaTb *) const;

	// soatablecolumns
	void AddCl(const SoaCl &column, int nameCl = 0);
	void AddAuxCl(const SoaCl &);
	const SoaCl *Cl(int) const;
	const SoaCl *Cl(const std::string &) const;
	const SoaCl *AuxCl(const std::string &) const;

	// soatablerows
	SoaRw *NewRw(int _id = -1, SoaCeMap *data = 0, bool resolve = false);
	SoaRw *RwByName(const std::string &);
	SoaRwList::iterator RwItByViewId(unsigned int);

	// soatableio
	void FromDB(sqlite3 *);
	void ToSQL(std::string &, bool, SoaPlType);
	bool ResolveKeys();

	void LoadAuxColumns();
	void LoadAuxCells();
	void LoadAuxXml(const std::string &);
	void DistributeXml();

	SoaCl *SchemaCl(const std::string &, SoaClNameType);
	std::string PrintXmlDocToString(TiXmlDocument &);
	TiXmlDocument CreateBlankAuxDataXml();

	// viewing hints

	void AddTableColumnHint(TableColumnHintNames n, TableColumnStyle s = tcsDefault);
	void Rec(SoaTb *, TableColumnHintNames &, std::vector<int> &);

	std::vector<int> ClStyles();
	std::vector<std::string> ClNames();
	std::string ClNamesR(SoaTb *, TableColumnHint &, int);
};

#endif
