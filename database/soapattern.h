#ifndef soapattern_h
#define soapattern_h

#include "soacell.h"
#include "soacolumn.h"
#include <vector>
#include <list>
#include <map>



class SoaRw;
class SoaTb;

typedef std::map<int, SoaCe>	SoaCeMap;
typedef std::pair<int, SoaCe>	SoaCePair;

class SoaPt : private SoaCeMap
{
public:
	SoaPt(const std::vector<SoaCl> *, const std::list<SoaRw *> *);

	SoaPt &Clear();
	SoaPt &Wt(const char *_columnName, int _value);
	SoaPt &Wt(const char *_columnName, SoaRw *_value);
	SoaPt &Wt(const char *_columnName, const char *_value, bool alias = false);
	SoaRw *Get(SoaTb *tb = 0, bool resolve = false);

private:
	const std::vector<SoaCl> *columns;
	const std::list<SoaRw *> *rows;

	bool Fits(SoaRw *);
//	void CopyTo(SoaRw *);
};

#endif
