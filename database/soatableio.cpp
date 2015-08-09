#include "database/soatable.h"
#include "soalogger.h"
#include "sqlite3.h"
#include <sstream>



void SoaTb::FromDB(sqlite3 *db) {
	sl::log << "reloading table: " << name << sl::end;

	Clear();

	std::string sql = "SELECT * FROM " + name + ";";

	sqlite3_stmt *statement;
	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &statement, 0) == SQLITE_OK) {

		bool running = true;
		while (running) {

			int stepResult = sqlite3_step(statement);
			if (stepResult == SQLITE_ROW) {
				sl::log << "sqlite3_step copying row data" << sl::end;

				SoaRw *row = NewRw();

				for (int columnIndex = 0; columnIndex < (int)columns.size(); ++columnIndex) {
					const SoaCl &column = columns[columnIndex];

					if ((column.Type() == sctInt) || (column.Type() == sctUid) || (column.Type() == sctRow))
						row->SetCe(columnIndex, SoaCe(sqlite3_column_int(statement, columnIndex)));
					else if ((column.Type() == sctTxt) || (column.Type() == sctXml))
						row->SetCe(columnIndex, SoaCe((const char *)sqlite3_column_text(statement, columnIndex)));
				}
			}
			else if (stepResult == SQLITE_DONE) {
				running = false;
				sl::log << "sqlite3_step done" << sl::end;
			}
			else if (stepResult == SQLITE_ERROR) {
				running = false;
				sl::log << "sqlite3_step error: " << sl::end;
			}
			else
				sl::log << "unhandled sqlite3_step result: " << stepResult << sl::end;
		}
	}
	else
		sl::log << "failed to prepare the get table statement" << sl::end;
}

// Sets foreign & home key pointers from both
// ids and foreign key pointers.
bool SoaTb::ResolveKeys() {
	sl::log << "resolving keys for table: " << name << sl::end;

	bool ok = true;
	for (int columnIndex = 0; columnIndex < (int)columns.size(); ++columnIndex) {
		SoaCl &column = columns[columnIndex];

		if (column.Fk() != 0) {
			for (SoaRwList::iterator row = begin(); row != end(); ++row) {

				SoaRw *fk = 0;
				int fkId = (*row)->Ce(columnIndex)->Num();
				const std::string &fkNm = (*row)->Ce(columnIndex)->Txt();

				// id foreign key must be resolved first

				if (fkId != CE_DEF_NUM) {
					fk = column.Fk()->Rw(fkId);

					if (fk != 0)
						(*row)->SetCe(columnIndex, SoaCe(fk));
					else {
						sl::log << "could not resolve fk " << fkId << " from table '" << name << "', column '" << column.Name() << "'" << sl::end;
						ok = false;
					}
				}
				else if (fkNm != CE_DEF_TXT) {	// todo: add secondary key (string)
					const SoaCl *cl = column.Fk()->Cl("name");	// assuming 'name' column is secondary key

					if (cl) {
						fk = column.Fk()->Rw(cl->Index(), fkNm);

						if (fk)
							(*row)->SetCe(columnIndex, SoaCe(fk));
						else {
							sl::log << "could not resolve fk " << fkId << " from table '" << name << "', column '" << column.Name() << "'" << sl::end;
							ok = false;
						}
					}
				}
				else
					fk = (*row)->Ce(columnIndex)->Row();

				// resolve home keys

				if (fk != 0)
					fk->AddCh(*row);
			}
		}
	}

	return ok;
}

//
// serialize
//

void SoaTb::ToSQL(std::string &sql, bool data, SoaPlType platform) {
	if (!data) {
		std::string lqs;

		sql += "CREATE TABLE " + name + " (";

		// columns

		for (unsigned int columnIndex = 0; columnIndex < columns.size(); ++columnIndex) {
			columns[columnIndex].ToSQL(sql, lqs, platform);

			if (columnIndex < columns.size() - 1)
				sql += ", ";
		}

		sql += lqs + ");\n";
	}
	else {
		for (SoaRwList::iterator row = begin(); row != end(); ++row) {
			std::stringstream stream;

			sql += "INSERT INTO " + name + " VALUES (";

			// other cells

			for (unsigned int columnIndex = 0; columnIndex < columns.size(); ++columnIndex) {
				switch (columns[columnIndex].Type()) {
				case sctUid:
				case sctInt:
					stream << (*row)->Ce(columnIndex)->Num();
					break;
				case sctXml:
				case sctTxt:
					stream << "'" + (*row)->Ce(columnIndex)->SqlTxt() + "'";
					break;
				case sctRow:
					stream << (((*row)->Ce(columnIndex)->Row()) ? (*row)->Ce(columnIndex)->Row()->Id() : CE_DEF_NUM);
					break;
				default: break;
				}

				if (columnIndex < columns.size() - 1)
					stream << ", ";
			}

			sql += stream.str() + ");\n";
		}
	}
}

