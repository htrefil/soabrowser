#include "database/soatable.h"
#include "database/soacell.h"
#include "database/soalogger.h"
#include <sstream>



SoaRw::SoaRw(SoaTb *_tb, int nCells, int nAuxCells, int nChClasses, int _id) :
	tb(_tb),
	cells(nCells), auxCells(nAuxCells), chClasses(nChClasses),
	viewId(tb->NextViewId()), viewTs(0), selId(0) {

	cells[0] = SoaCe(_id);
}

// Removes this from its table and disconnects from parents
// and children. Does not delete children.
SoaRw::~SoaRw() {

	// remove from table

	tb->remove(this);
	tb->SetChanged();

	// disconnect from parents

	for (SoaCeVector::iterator cell = cells.begin(); cell != cells.end(); ++cell)
		if (cell->Row())
			cell->Row()->RemoveCh(this);

	// disconnect from children

	for (SoaRwListVector::iterator chClass = chClasses.begin(); chClass != chClasses.end(); ++chClass)
		for (SoaRwList::iterator child = chClass->begin(); child != chClass->end(); ++child)
			(*child)->RemovePr(this);
}

void SoaRw::DeleteDependencies() {

	for (unsigned int cc = 0; cc < tb->chClasses.size(); ++cc) {

		if (chClasses[cc].size() == 0)
			continue;

		if (tb->chClasses[cc]->IsDependency(tb)) {

			while (chClasses[cc].size()) {
				chClasses[cc].back()->DeleteDependencies();
				delete chClasses[cc].back();
			}
		}
	}
}

SoaRwList SoaRw::GetDependencies() {

	SoaRwList deps;

	for (unsigned int cc = 0; cc < tb->chClasses.size(); ++cc) {

		if (chClasses[cc].size() == 0)
			continue;

		if (tb->chClasses[cc]->IsDependency(tb))
			for (SoaRwList::iterator c = chClasses[cc].begin(); c != chClasses[cc].end(); ++c)
				deps.push_back(*c);
	}

	return deps;
}

void SoaRw::AllocateAuxCells(int nAuxCells) {
	if (nAuxCells)
		auxCells = SoaCeVector(nAuxCells);
	else
		auxCells.clear();
}

//
// derived properties
//

// Gets the row id. Default id column is the first
// column named 'id'
int SoaRw::Id() {
	return cells[0].Num();
}

void SoaRw::SetId(int _id) {
	cells[0] = SoaCe(_id);
}

unsigned int SoaRw::ViewId() const {
	return viewId;
}

std::string SoaRw::Name() const {
	std::stringstream ss;

	for (std::map<int, int>::const_iterator nameCl = tb->rwNameCls.begin(); nameCl != tb->rwNameCls.end(); ++nameCl) {
		ss << cells[nameCl->second].ToString();
		if (nameCl != --tb->rwNameCls.end())
			ss << " ";
	}

	return ss.str();
}

bool SoaRw::Contains(const std::string &text) const {
	for (SoaCeVector::const_iterator cell = cells.begin(); cell != cells.end(); ++cell)
		if (cell->Txt() == text)
			return true;

	return false;
}

std::vector<std::string> SoaRw::ToStrings(TableColumnHints &hints) {

	std::vector<std::string> strings;

	if (hints.size()) {

		for (TableColumnHints::iterator hint = hints.begin(); hint != hints.end(); ++hint) {
			SoaRw *row = this;
			for (unsigned int i = 0; i < hint->indices.size(); ++i) {
				if (i == hint->indices.size() - 1)
					strings.push_back(row ? row->Ce(hint->indices[i])->ToString() : std::string());
				else {
					row = row->Ce(hint->indices[i])->Row();
					if (row == 0) {
						strings.push_back("");
						break;
					}
				}
			}
		}
	}
	else
		for (SoaCeVector::const_iterator cell = ++++cells.begin(); cell != cells.end(); ++cell)
			strings.push_back(cell->ToString());

	for (SoaCeVector::const_iterator cell = auxCells.begin(); cell != auxCells.end(); ++cell)
		strings.push_back(cell->ToString());

	return strings;
}

//
// cells
//

// Sets a cell value by index.
void SoaRw::SetCe(unsigned int columnIndex, const SoaCe &cell) {
	if (columnIndex < cells.size()) {
		cells[columnIndex] = cell;
		++viewTs;
		tb->SetChanged();
	}
	else
		sl::log << "SoaRw::SetCe(): column index out of range" << sl::end;
}

// Gets a cell value by index.
const SoaCe *SoaRw::Ce(unsigned int columnIndex) {
	if (columnIndex < cells.size())
		return &cells[columnIndex];
	else {
		sl::log << "SoaRw::Ce(): column index out of range" << sl::end;
		return 0;
	}
}

// Sets a cell value by column name.
void SoaRw::SetCe(const char *columnName, const SoaCe &cell) {
	const SoaCl *column = tb->Cl(columnName);

	if (column) {
		cells[column->Index()] = cell;
		++viewTs;
		tb->SetChanged();
	}
	else
		sl::log << "SoaRw::SetCe(): invalid column name '" << columnName << "' from table '" << tb->Name() << "'" << sl::end;
}

// Gets a cell value by column name.
const SoaCe *SoaRw::Ce(const std::string &columnName) {
	const SoaCl *column = tb->Cl(columnName);

	if (column)
		return &cells[column->Index()];
	else {
		sl::log << "SoaRw::Ce(): invalid column name '" << columnName << "' from table '" << tb->Name() << "'" << sl::end;
		return 0;
	}
}

void SoaRw::SetPr(const char *clName, SoaRw *pr) {
	const SoaCl *cl = tb->Cl(clName);

	if (cl) {

		if (cells[cl->Index()].Row())
			cells[cl->Index()].Row()->RemoveCh(this);

		if (pr && cl->Fk() == pr->Tb())
			cells[cl->Index()] = SoaCe(pr);
		else
			cells[cl->Index()].Reset();

		++viewTs;
		tb->SetChanged();
	}
	else
		sl::log << "SoaRw::SetPr(): invalid column name '" << clName << "' from table '" << tb->Name() << "'" << sl::end;
}

// Sets an aux cell value by column name.
void SoaRw::SetAuxCe(const char *columnName, const SoaCe &cell) {
	const SoaCl *column = tb->AuxCl(columnName);

	if (column) {
		auxCells[column->Index()] = cell;
		++viewTs;
		tb->SetChanged();
	}
	else
		sl::log << "SoaRw::SetAuxCe(): invalid aux column name '" << columnName << "' from table '" << tb->Name() << "'" << sl::end;
}

// Gets an aux cell value by index.
const SoaCe *SoaRw::AuxCe(unsigned int columnIndex) {
	if (columnIndex < auxCells.size())
		return &auxCells[columnIndex];
	else {
		sl::log << "SoaRw::AuxCe(): column index out of range" << sl::end;
		return 0;
	}
}

// Resets all aux cells to default values.
void SoaRw::ResetAuxCells() {
	for (unsigned int i = 0; i < auxCells.size(); ++i)
		auxCells[i].Reset();
}

// Gets a list of children belonging to the
// specified table.
SoaRwList *SoaRw::Children(const char *tbName) {
	int chClassIndex = tb->ChClassIndex(tbName);

	if (chClassIndex != -1)
		return &chClasses[chClassIndex];
	else {
		sl::log << "SoaRw::Children(): invalid children table name: " << tbName << sl::end;
		return 0;
	}
}

//
// modify
//

// Adds a child row pointer to the appropriate child class.
void SoaRw::AddCh(SoaRw *child) {
	int chClassIndex = tb->ChClassIndex(child->tb);

	if (chClassIndex != -1) {

		// check if key already present

		for (SoaRwList::iterator r = chClasses[chClassIndex].begin(); r != chClasses[chClassIndex].end(); ++r)
			if (*r == child) {
				sl::log << "failed to add child to row in table '" << tb->Name() << "', child already present: " << child->tb->Name() << sl::end;
				return;
			}

		chClasses[chClassIndex].push_back(child);
	}
	else
		sl::log << "failed to add child to row in table '" << tb->Name() << "', invalid child table: " << child->tb->Name() << sl::end;
}

// Removes a child row pointer from the appropriate child class.
void SoaRw::RemoveCh(SoaRw *child) {
	int chClassIndex = tb->ChClassIndex(child->tb);

	if (chClassIndex != -1)
		chClasses[chClassIndex].remove(child);
	else
		sl::log << "failed to remove child from row in table '" << tb->Name() << "', invalid child table: " << child->tb->Name() << sl::end;
}

// Removes a parent pointer from the cell.
void SoaRw::RemovePr(SoaRw *parent) {

	for (SoaCeVector::iterator cell = cells.begin(); cell != cells.end(); ++cell)
		if (cell->Row() == parent) {
			cell->Reset();
			return;
		}

	sl::log << "failed to remove parent from row in table '" << tb->Name() << "', invalid parent table: " << parent->tb->Name() << sl::end;
}

// Try to register this row with all foreign keys. The
// registration procedure checks if already registered.
void SoaRw::RegisterWithFks() {

	for (SoaCeVector::iterator cell = cells.begin(); cell != cells.end(); ++cell)
		if (cell->Row() != CE_DEF_ROW)
			cell->Row()->AddCh(this);
}

//
// selection
//

bool SoaRw::InvertSelection(unsigned int globalSelId) {

	if (selId == globalSelId) {
		selId--;
		return false;
	}
	else {
		selId = globalSelId;
		return true;
	}
}
