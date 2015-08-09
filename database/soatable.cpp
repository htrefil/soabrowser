#include "database/soadatabase.h"
#include "database/soapattern.h"
#include "database/soalogger.h"



SoaTb::SoaTb(SoaDb *_db, const std::string &_name) :
	db(_db),
	focusRw(0),
	name(_name),
	pattern(&columns, this),
	index(db->size()),
	nextViewId(0) {

	AddCl(SoaCl("id", sctUid));
	AddCl(SoaCl("xml"));
	AddCl(SoaCl("description", 1024, SoaClSc("Description", true, false)));
}

SoaTb::~SoaTb() {}

void SoaTb::Clear() {

	while (size())
		delete back();

	nextViewId = 0;
}

void SoaTb::SetChanged() {
	db->SetChanged();
}

//
// properties
//

const std::string &SoaTb::Name() const {
	return name;
}

// Gets the next available row view id. Encodes
// the table index within the id.
unsigned int SoaTb::NextViewId() {
	db->SetChanged();
	return nextViewId++ * 100 + index;	// table index = id % 100
}

int SoaTb::RowsCount() const {
	return size();
}

// 1. doesn't have to be deleted if parent is deleted (not a dependency)
// 2. has to be deleted if parent is deleted and can be automatically deleted by parent
// 3. has to be deleted if parent is deleted but cannot be automatically deleted by parent
bool SoaTb::IsDependency(SoaTb *fk) const {

	for (SoaClVector::const_iterator cl = columns.begin(); cl != columns.end(); ++cl)
		if (cl->Fk() == fk && cl->IsMandatory())
			return true;

	return false;
}

// Searches for an available row uid.
int SoaTb::GetAvailableId() const {

	int id = 0;
	while (true) {

		const_iterator row = begin();
		while (row != end()) {
			if ((*row)->Id() == id) {
				id++;
				break;
			}
			else
				row++;
		}

		if (row == end())
			break;
	}

	return id;
}

SoaPt &SoaTb::Search() {
	return pattern.Clear();
}

const SoaClVector &SoaTb::Columns() const {
	return columns;
}

const SoaClVector &SoaTb::AuxColumns() const {
	return auxColumns;
}

// Get index of a home key list.
int SoaTb::ChClassIndex(const std::string &chClassName) const {

	SoaTb *chClass = db->Tb(chClassName);
	if (chClass != 0) {

		for (int chClassIndex = 0; chClassIndex < (int)chClasses.size(); ++chClassIndex)
			if (chClass == chClasses[chClassIndex])
				return chClassIndex;

		sl::log << "hk table not found: " << chClass->Name() << sl::end;
	}

	return -1;
}

// Get index of a home key list.
int SoaTb::ChClassIndex(SoaTb *chClass) const {
	if (chClass != 0) {

		for (int chClassIndex = 0; chClassIndex < (int)chClasses.size(); ++chClassIndex)
			if (chClass == chClasses[chClassIndex])
				return chClassIndex;

		sl::log << "hk table not found: " << chClass->Name() << sl::end;
	}

	return -1;
}

//
// viewing hints
//

void SoaTb::AddTableColumnHint(TableColumnHintNames names, TableColumnStyle style) {

	std::vector<int> indices;
	Rec(this, names, indices);
	tableColumnHints.push_back(TableColumnHint(indices, style));
}

// todo: rename
void SoaTb::Rec(SoaTb *tb, TableColumnHintNames &n, std::vector<int> &indices) {

	if (tb) {

		const SoaCl *cl = tb->Cl(n.front());
		n.erase(n.begin());
		if (cl) {
			indices.push_back(cl->Index());

			if (n.size())
				Rec(cl->Fk(), n, indices);
		}
		else
			sl::log << "AddTableColumnHint() error" << sl::end;
	}
	else
		sl::log << "AddTableColumnHint() error" << sl::end;
}

// column names wrt table column hints

std::vector<int> SoaTb::ClStyles() {
	std::vector<int> styles;

	for (TableColumnHints::iterator hint = tableColumnHints.begin(); hint != tableColumnHints.end(); ++hint)
		styles.push_back(hint->style);

	for (SoaClVector::const_iterator column = auxColumns.begin(); column != auxColumns.end(); ++column)
		styles.push_back(tcsDefault);

	return styles;
}

std::vector<std::string> SoaTb::ClNames() {
	std::vector<std::string> names;

	for (TableColumnHints::iterator hint = tableColumnHints.begin(); hint != tableColumnHints.end(); ++hint)
		names.push_back(ClNamesR(this, *hint, 0));

	for (SoaClVector::const_iterator column = auxColumns.begin(); column != auxColumns.end(); ++column)
		names.push_back(column->DisplayName());

	return names;
}

std::string SoaTb::ClNamesR(SoaTb *tb, TableColumnHint &hint, int l) {

	if (tb && l < (int)hint.indices.size()) {
		const SoaCl *cl = tb->Cl(hint.indices[l]);
		if (cl)
			return (l == (int)hint.indices.size() - 1) ? cl->DisplayName() : ClNamesR(cl->Fk(), hint, ++l);
	}

	return "invalidClName";
}
