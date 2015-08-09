#include "soacell.h"
#include "soarow.h"
#include <sstream>



SoaCe::SoaCe() : row(CE_DEF_ROW), txt(CE_DEF_TXT), num(CE_DEF_NUM), type(cetInv)  {}

SoaCe::SoaCe(SoaRw *_row) : row(_row), txt(CE_DEF_TXT), num(CE_DEF_NUM), type(cetRow) {}

SoaCe::SoaCe(const char *_txt) : row(CE_DEF_ROW), txt((_txt) ? _txt : ""), num(CE_DEF_NUM), type(cetTxt) {
	CleanText();
}

SoaCe::SoaCe(const std::string &_txt) : row(CE_DEF_ROW), txt(_txt), num(CE_DEF_NUM), type(cetTxt) {
	CleanText();
}

SoaCe::SoaCe(const std::string &_txt, CeType _type) : row(CE_DEF_ROW), txt(_txt), num(CE_DEF_NUM), type(_type) {
	CleanText();
}

SoaCe::SoaCe(int _num) : row(CE_DEF_ROW), txt(CE_DEF_TXT), num(_num), type(cetNum) {}

bool SoaCe::operator!=(const SoaCe &other) const {
	switch (type) {
	case cetRowAlias:	return (other.Row()) ? !other.Row()->Contains(txt) : true;
	case cetRow:		return row != other.row;
	case cetTxt:		return txt != other.txt;
	case cetNum:		return num != other.num;
	default:			return true;
	}
}

void SoaCe::Reset() {
	row = CE_DEF_ROW;
	txt = CE_DEF_TXT;
	num = CE_DEF_NUM;
}

SoaRw *SoaCe::Row() const {
	return row;
}

const std::string &SoaCe::Txt() const {
	return txt;
}

std::string SoaCe::SqlTxt() const {
	std::string t = txt;

	for (std::string::iterator c = t.begin(); c != t.end(); ++c) {
		std::string::iterator p = c;
		if (*c == '\'' && (++p == t.end() || *p != '\''))
			c = t.insert(p, '\'');
	}

	return t;
}

int SoaCe::Num() const {
	return num;
}

bool SoaCe::IsNull() const {
	return txt == CE_DEF_TXT && num == CE_DEF_NUM && row == CE_DEF_ROW;
}

// Returns a string representing the value:
// empty string if default value,
std::string SoaCe::ToString() const {
	std::stringstream ss;

	ss << "";
	if (type == cetTxt || type == cetRowAlias)
		ss << txt;
	else if (type == cetNum && num != CE_DEF_NUM)
		ss << num;
	else if (type == cetRow && row != CE_DEF_ROW)
		ss << row->Name();

	return ss.str();
}

// Replaces all occurrences of '' with '.
void SoaCe::CleanText() {
	for (std::string::iterator c = txt.begin(); c != txt.end(); ++c) {
		std::string::iterator p = c;
		if (*c == '\'' && ++p != txt.end() && *p == '\'')
			txt.erase(p);
	}
}
