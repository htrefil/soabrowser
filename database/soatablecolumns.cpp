#include "database/soatable.h"
#include "soalogger.h"



// Adds a column.
void SoaTb::AddCl(const SoaCl &column, int nameCl) {

	columns.push_back(SoaCl(columns.size(), column));

	// if column is foreign key

	if (columns.back().Fk() != 0)
		columns.back().Fk()->chClasses.push_back(this);

	// column is part of public name

	if (nameCl)
		rwNameCls.insert(std::pair<int, int>(nameCl, columns.back().Index()));
}

// Adds a column.
void SoaTb::AddAuxCl(const SoaCl &column) {
	auxColumns.push_back(SoaCl(auxColumns.size(), column));
}

// Get column by index.
const SoaCl *SoaTb::Cl(int columnIndex) const {

	if ((columnIndex >= 0) && (columnIndex < (int)columns.size()))
		return &columns[columnIndex];
	else {
		sl::log << "invalid column index: " << columnIndex << sl::end;

		return 0;
	}
}

// Get column by name.
const SoaCl *SoaTb::Cl(const std::string &columnName) const {

	for (SoaClVector::const_iterator column = columns.begin(); column != columns.end(); ++column)
		if (column->Name() == columnName)
			return &(*column);

	sl::log << "column not found: " << columnName << sl::end;

	return 0;
}

// Get aux column by (mangled) name.
const SoaCl *SoaTb::AuxCl(const std::string &columnName) const {

	for (SoaClVector::const_iterator column = auxColumns.begin(); column != auxColumns.end(); ++column)
		if (column->Name() == columnName)
			return &(*column);

	sl::log << "aux column not found: " << columnName << sl::end;

	return 0;
}

