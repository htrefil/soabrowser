#include "soatable.h"
#include "soalogger.h"



SoaPt::SoaPt(const SoaClVector *_columns, const SoaRwList *_rows) : columns(_columns), rows(_rows) {}

SoaPt &SoaPt::Clear() {
	clear();
	return *this;
}

//
// row compare & create
//

bool SoaPt::Fits(SoaRw *row) {
	for (SoaCeMap::iterator trait = begin(); trait != end(); ++trait)
		if (trait->second != *row->Ce(trait->first))
			return false;

	return true;
}

//
// pattern elements management
//

SoaPt &SoaPt::Wt(const char *columnName, int value) {

	for (SoaClVector::const_iterator cl = columns->begin(); cl != columns->end(); ++cl)

		if (cl->Name() == columnName) {

			if (cl->Type() == sctInt)
				insert(SoaCePair(cl->Index(), SoaCe(value)));
			else
				sl::log << "invalid pattern column type sctRow for column: " << columnName << sl::end;

			return *this;
		}

	sl::log << "invalid pattern column name: " << columnName << sl::end;
	return *this;
}

SoaPt &SoaPt::Wt(const char *columnName, SoaRw *value) {

	for (SoaClVector::const_iterator cl = columns->begin(); cl != columns->end(); ++cl)

		if (cl->Name() == columnName) {

			if (cl->Type() == sctRow)
				insert(SoaCePair(cl->Index(), SoaCe(value)));
			else
				sl::log << "invalid pattern column type sctRow for column: " << columnName << sl::end;

			return *this;
		}

	sl::log << "invalid pattern column name: " << columnName << sl::end;
	return *this;
}

SoaPt &SoaPt::Wt(const char *columnName, const char *value, bool alias) {

	for (SoaClVector::const_iterator cl = columns->begin(); cl != columns->end(); ++cl)

		if (cl->Name() == columnName) {

			if (cl->Type() == sctTxt)
				insert(SoaCePair(cl->Index(), SoaCe(value)));
			else if (alias && cl->Type() == sctRow)
				insert(SoaCePair(cl->Index(), SoaCe(value, SoaCe::cetRowAlias)));
			else
				sl::log << "invalid pattern column type sctTxt for column: " << columnName << sl::end;

			return *this;
		}

	sl::log << "invalid pattern column name: " << columnName << sl::end;
	return *this;
}

// Finds the row with matching cell data specified in
// the pattern. If the table ('tb') arg is not 0, and a
// row is not found, create a new row and initialize it
// with pattern data.
SoaRw *SoaPt::Get(SoaTb *tb, bool resolve) {

	// find matching row

	for (SoaRwList::const_iterator row = rows->begin(); row != rows->end(); ++row)
		if (Fits(*row))
			return *row;

	// if row not found, create new row

	return tb ? tb->NewRw(tb->GetAvailableId(), this, resolve) : 0;
}
