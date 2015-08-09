#include "database/soatable.h"
#include "soalogger.h"



// Allocates a new row, but doesn't add it to the table.
// Optionally fills with available data (selected rows for fks).
SoaRw *SoaTb::AllocRw(int _id, bool fill) {

	SoaRw *row = new SoaRw(this, columns.size(), auxColumns.size(), chClasses.size(), _id);

	if (fill)
		for (SoaClVector::iterator cl = columns.begin(); cl != columns.end(); ++cl)
			if (cl->IsMandatory() && cl->Fk() && cl->Fk()->FocusRw())
				row->SetCe(cl->Index(), SoaCe(cl->Fk()->FocusRw()));

	return row;
}

// Adds only valid rows that are not already in the table.
// Registers fks.
void SoaTb::AddRw(SoaRw *row) {

	if (!IsRwValid(row))
		return;

	for (iterator rw = begin(); rw != end(); ++rw)
		if (*rw == row)
			return;

	row->SetId(GetAvailableId());
	row->RegisterWithFks();

	push_back(row);
}

// Checks that mandatory column cells are not null, and
// the row cells are allocated for this table.
bool SoaTb::IsRwValid(SoaRw *row) {

	if (row->Tb() != this)
		return false;

	for (SoaClVector::iterator cl = columns.begin(); cl != columns.end(); ++cl)
		if (cl->IsMandatory() && row->Ce(cl->Index())->IsNull())
			return false;

	return true;
}

// Creates a new row and adds it to the table. Copies any provided
// data, and optionally resolves mandatory dependencies (adding fk
// rows to fk tables if neccessary).
SoaRw *SoaTb::NewRw(int _id, SoaCeMap *data, bool resolve) {

	SoaRw *row = AllocRw(_id, false);

	if (data) {

		// copy data

		for (SoaCeMap::iterator d = data->begin(); d != data->end(); ++d)
			row->SetCe(d->first, d->second);

		if (resolve) {

			// resolve mandatory dependencies

			for (SoaClVector::iterator cl = columns.begin(); cl != columns.end(); ++cl) {

				if (cl->IsMandatory()) {

					SoaTb *tb = cl->Fk();
					const SoaCe *ce = row->Ce(cl->Index());

					if (ce->IsNull()) {	// no data provided

						if (tb) {	// mandatory fk value not provided

							SoaRw *fk = tb->FocusRw();	// get last selected
							if (fk) {
								row->SetCe(cl->Index(), SoaCe(fk));
								fk->AddCh(row);
							}
							else {
								delete row;
								return 0;	// mandatory fk value not provided & no last selected found
							}
						}
						else {
							delete row;
							return 0;	// mandatory non-fk value not provided
						}
					}
					else {

						if (tb) {	// if fk column

							if (ce->Row() == CE_DEF_ROW) {	// mandatory fk alias value provided

								SoaRw *fk = 0;
								if (ce->Num() != CE_DEF_NUM)	// int fk alias
									fk = tb->RwByViewId(ce->Num());
								else if (ce->Txt() != CE_DEF_TXT)	// text fk alias
									fk = tb->Search().Wt("name", ce->Txt().c_str(), true).Get(tb, true);

								if (fk) {
									row->SetCe(cl->Index(), SoaCe(fk));
									fk->AddCh(row);
								}
								else {
									delete row;
									return 0;
								}
							}
							else {	// mandatory fk provided

								if (tb == ce->Row()->Tb()) {
									row->SetCe(cl->Index(), SoaCe(ce->Row()));
									ce->Row()->AddCh(row);
								}
								else {
									delete row;
									return 0;
								}
							}
						}
					}
				}
			}
		}
	}

	push_back(row);
	return row;
}

bool SoaTb::DeleteRowByViewId(unsigned int rowId) {

	SoaRw *row = RwByViewId(rowId);
	if (row == 0) {
		sl::log << "error: could not delete row from table '" << name << "', row with view id: " << rowId << " not found" << sl::end;
		return false;
	}

	row->DeleteDependencies();
	delete row;

	focusRw = 0;

	return true;
}

// Get focus row. Focus row is either the last selected row
// or the last row in table if no row is selected or the last
// selected row was deselected.
SoaRw *SoaTb::FocusRw() {
	return focusRw;// ? focusRw : (size() ? back() : 0);
}

SoaRw *SoaTb::FrontRw() {
	return size() ? front() : 0;
}

void SoaTb::SetFocusedRwByViewId(unsigned int rowId) {
	focusRw = RwByViewId(rowId);
}

// Inverts the specified row selection and focuses the row if selected.
bool SoaTb::InvertRowSelection(unsigned int rowId, unsigned int globalSelId) {

	iterator row = RwItByViewId(rowId);
	if (row != end()) {
		focusRw = ((*row)->InvertSelection(globalSelId)) ? *row : 0;
		return true;
	}

	return false;
}



// Get row by name.
SoaRw *SoaTb::RwByName(const std::string &rwName) {
	const SoaCl *cl = Cl("name");
	return cl ? Rw(cl->Index(), rwName) : 0;
}

// Get row by view id.
SoaRw *SoaTb::RwByViewId(unsigned int viewId) {
	for (iterator r = begin(); r != end(); ++r)
		if ((*r)->viewId == viewId)
			return *r;

	return 0;
}

// Get row iterator by view id.
SoaRwList::iterator SoaTb::RwItByViewId(unsigned int viewId) {
	for (iterator r = begin(); r != end(); ++r)
		if ((*r)->viewId == viewId)
			return r;

	return end();
}

// Get row by database id. Used for resolving keys.
SoaRw *SoaTb::Rw(int id) {
	for (iterator r = begin(); r != end(); ++r)
		if ((*r)->Id() == id)
			return *r;

	return 0;
}

// Get row by column index and string value. Used for resolving keys.
SoaRw *SoaTb::Rw(unsigned int clIndex, const std::string &txt) {
	for (iterator r = begin(); r != end(); ++r)
		if ((*r)->Ce(clIndex)->Txt() == txt)
			return *r;

	return 0;
}
