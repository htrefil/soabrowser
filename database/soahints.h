#ifndef soahints_h
#define soahints_h

#include <vector>
#include <list>



//
// table view hints (per column)
//

class TableColumnHintNames : public std::vector<const char *>
{
public:
	TableColumnHintNames &operator<<(const char *n) {
		push_back(n);
		return *this;
	}
};

enum TableColumnStyle { tcsDefault, tcsFixedWide };

struct TableColumnHint
{
	std::vector<int> indices;
	TableColumnStyle style;

	TableColumnHint(std::vector<int> _indices, TableColumnStyle _style = tcsDefault) :
		indices(_indices), style(_style) {}
};

typedef std::list<TableColumnHint> TableColumnHints;

#endif
