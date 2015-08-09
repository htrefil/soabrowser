#include "soatable.h"
#include <sstream>



SoaCl::SoaCl() :
	fk(0), name("undefined"), displayName(name), type(sctInv), capacity(0), index(-1) {}

SoaCl::SoaCl(SoaTb *_fk, const SoaClSc &_schema) :
	fk(_fk), name(_fk->Name()), displayName(name), type(sctRow), schema(_schema), capacity(0), index(-1) {}

SoaCl::SoaCl(const std::string &_name, SoaClType _type, const SoaClSc &_schema) :
	fk(0), name(_name), displayName(name), type(_type), schema(_schema), capacity(512), index(-1) {}

SoaCl::SoaCl(const std::string &_name, int _capacity, const SoaClSc &_schema) :
	fk(0), name(_name), displayName(name), type(sctTxt), schema(_schema), capacity(_capacity), index(-1) {}

SoaCl::SoaCl(const std::string &_name, const SoaClSc &_schema) : // ??? max. capacity
	fk(0), name(_name), displayName(name), type(sctXml), schema(_schema), capacity(10240), index(-1) {}

SoaCl::SoaCl(const std::string &_name, const std::string &_displayName, int _capacity) :
	fk(0), name(_name), displayName(_displayName), type(sctTxt), schema(), capacity(_capacity), index(-1) {}

SoaCl::SoaCl(int _index, const SoaCl &other) :
	fk(other.fk), name(other.name), displayName(other.displayName), type(other.type), schema(other.schema), capacity(other.capacity), index(_index) {}

//
// properties
//

SoaTb *SoaCl::Fk() const {
	return fk;
}

const std::string &SoaCl::Name() const {
	return name;
}

const std::string &SoaCl::DisplayName() const {
	return displayName;
}

SoaClType SoaCl::Type() const {
	return type;
}

int SoaCl::Index() const {
	return index;
}

const SoaClSc &SoaCl::Schema() const {
	return schema;
}

bool SoaCl::IsMandatory() const {
	return schema.critical;
}

//
// serializing
//

void SoaCl::ToSQL(std::string &sql, std::string &lqs, SoaPlType platform) {
	std::stringstream stream, maerts;
	stream << name;

	switch (type) {
	case sctUid:
		stream << " INT";

		if ((platform == sptSQLServer) || (platform == sptSQLite))
			stream << " PRIMARY KEY";
		else if (platform == sptMySQL)
			maerts << ", PRIMARY KEY (id)";
		break;
	case sctInt:
		stream << " INT";
		break;
	case sctTxt:
	case sctXml: // ??? detect xml from sql
		stream << " VARCHAR (" << capacity << ")";
		break;
	case sctRow:
		stream << " INT";

		if (platform == sptSQLServer)
			stream << " FOREIGN KEY REFERENCES " << fk->Name() << " (id)";
		else if ((platform == sptMySQL) || (platform == sptSQLite))
			maerts << ", FOREIGN KEY (" << name << ") REFERENCES " << fk->Name() << " (id)";
		break;
	default:
		break;
	}

	sql += stream.str();
	lqs += maerts.str();
}

//
// static
//

std::string SoaCl::TypeToString(SoaClType type) {
	switch (type) {
	case sctInv:	return "invalid";
	case sctUid:	return "id";
	case sctInt:	return "int";
	case sctTxt:	return "string";
	case sctXml:	return "xml";
	case sctRow:	return "foreign";
	default:		return "undefined";
	}
}

SoaClType SoaCl::StringToType(const std::string &str) {
	if (str == "invalid")		return sctInv;
	else if (str == "id")		return sctUid;
	else if (str == "int")		return sctInt;
	else if (str == "string")	return sctTxt;
	else if (str == "xml")		return sctXml;
	else if (str == "foreign")	return sctRow;
	else						return sctInv;
}
