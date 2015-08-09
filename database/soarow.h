#ifndef soarow_h
#define soarow_h

#include "database/soacell.h"
#include "database/soahints.h"
#include <string>
#include <vector>
#include <list>
#include <map>



class SoaRw;
class SoaTb;

typedef std::vector<SoaCe>		SoaCeVector;
typedef std::list<SoaRw *>		SoaRwList;
typedef std::vector<SoaRwList>	SoaRwListVector;

class SoaRw
{
	friend class SoaDb;
	friend class SoaTb;

public:
	SoaRw(SoaTb *, int, int, int, int);
	~SoaRw();

	void DeleteDependencies();
	SoaRwList GetDependencies();

	void AllocateAuxCells(int);

	int Id();
	void SetId(int);
	unsigned int ViewId() const;
	SoaTb *Tb() { return tb; }
	std::string Name() const;
	bool Contains(const std::string &) const;
	std::vector<std::string> ToStrings(TableColumnHints &);

	void SetCe(unsigned int, const SoaCe &);
	const SoaCe *Ce(unsigned int);
	void SetCe(const char *, const SoaCe &);
	const SoaCe *Ce(const std::string &);
	void SetPr(const char *, SoaRw *);
	void SetAuxCe(const char *, const SoaCe &);
	const SoaCe *AuxCe(unsigned int);
	void ResetAuxCells();

	SoaRwList *Children(const char *);
	void AddCh(SoaRw *);	// todo: rename to register & unregister
	void RemoveCh(SoaRw *);
	void RemovePr(SoaRw *);	// todo: see if neccessary
	void RegisterWithFks();

	bool InvertSelection(unsigned int);

protected:
	SoaTb *tb;
	SoaCeVector cells, auxCells;
	SoaRwListVector chClasses;
	unsigned int viewId, viewTs, selId;
};

#endif
