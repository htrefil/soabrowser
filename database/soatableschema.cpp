#include "database/soalogger.h"
#include "database/soadatabase.h"
#include "tinyxml/tinyxml.h"



// Checks if the loaded schema is valid: all critical
// columns 'valid' flags must be set to true. Also fails
// if table has no critical columns.
bool SoaTb::IsSchemaValid() const {

	bool hasCritical = false;
	for (SoaClVector::const_iterator cl = columns.begin(); cl != columns.end(); ++cl)
		if (cl->schema.critical) {

			if (!cl->schema.valid)
				return false;

			hasCritical = true;
		}

	return hasCritical;
}

// Loads the schema from an xsd file.
//
void SoaTb::LoadSchema(const std::string &path) {

	sl::log << "Loading schema file from: " << path << sl::end;

	TiXmlDocument *doc = new TiXmlDocument("schema");
	if (!doc->LoadFile(path)) {
		sl::log << "Error loading merge file: " << path << sl::end;
		return;
	}

	// try to apply new schema

	if (!ApplyNewSchemaXml(doc))
		delete doc;
}

// Deserializes schema from the housekeeping singleton.
//
void SoaTb::DeserializeSchema(const std::string &schemaText) {

	TiXmlDocument *doc = new TiXmlDocument("schema");

	if (doc->Parse(schemaText.c_str())) {
		if (!ApplyNewSchemaXml(doc))
			delete doc;
	}
	else {
		sl::log << "DeserializeSchema xml parse error: " << doc->ErrorDesc() << sl::end;
		delete doc;
		return;
	}
}

//
//
bool SoaTb::ApplyNewSchemaXml(TiXmlDocument *doc) {

	// reset schema validity

	for (SoaClVector::iterator cl = columns.begin(); cl != columns.end(); ++cl)
		cl->schema.valid = false;

	auxColumns.clear();

	// traverse nodes

	TiXmlElement *p = doc->FirstChildElement()->FirstChildElement()->FirstChildElement()->FirstChildElement()->FirstChildElement();
	for (TiXmlElement *e = p->FirstChildElement(); e; e = e->NextSiblingElement()) {

		//
		// merge
		//

		if (e->ValueStr() == "xs:annotation") {

			TiXmlElement *m = e->FirstChildElement()->FirstChildElement();
			for (TiXmlElement *f = m->FirstChildElement(); f; f = f->NextSiblingElement()) {	// merge fields

				std::string fieldXSDName = f->ValueStr();
				int colon = fieldXSDName.find_first_of(':') + 1;
				std::string fieldName = fieldXSDName.substr(colon, fieldXSDName.size() - colon);

				SoaCl *cl = SchemaCl(fieldName, scnMerge);
				if (cl) {
					const char *c = f->GetText();
					cl->schema.schema = (c ? c : cl->schema.merge);
				}
				else
					sl::log << "Error loading merge file for table '" << name << "': merge name alias '" << fieldName << "' not found" << sl::end;
			}
		}

		//
		// schema
		//

		else if (e->ValueStr() == "xs:complexType") {

			for (TiXmlElement *c = e->FirstChildElement()->FirstChildElement(); c; c = c->NextSiblingElement()) {

				const char *nameAttr = c->Attribute("name");
				if (nameAttr) {

					std::string mangledName = nameAttr, cleanName = mangledName;

					// extract clean column name (not mangled because of special chars)

					for (TiXmlElement *s = c->FirstChildElement(); s; s = s->NextSiblingElement())
						if (s->ValueStr() == "xs:annotation")
							for (TiXmlElement *t = s->FirstChildElement(); t; t = t->NextSiblingElement())
								if (t->ValueStr() == "xs:appinfo")
									cleanName = t->GetText();

					// add column

					SoaCl *cl = SchemaCl(cleanName, scnSchema);
					if (cl) {
						cl->schema.valid = true; // validates both critical and descriptive
						cl->schema.mangledSchema = mangledName;
					}
					else
						AddAuxCl(SoaCl(mangledName, cleanName, 512));
				}
				else
					sl::log << "Schema node with no 'name' attribute" << sl::end;
			}
		}
	}

	if (!IsSchemaValid())
		auxColumns.clear();		// deallocate aux columns

	// allocate aux row cells (deallocates if schema invalid)
	// set default blank xml into xml column cells

	for (SoaTb::iterator row = begin(); row != end(); ++row)
		(*row)->AllocateAuxCells(auxColumns.size());

	DistributeXml();	// todo: don't call this if schema not valid

	// dump new schema file contents into the housekeeping singleton

	SoaRw *hs = db->GetHousekeeping();
	if (hs) {

		TiXmlPrinter p;
		p.SetIndent("\t");
		doc->Accept(&p);

		hs->SetCe("portfolioSchema", p.CStr());
	}

	return IsSchemaValid();
}

//
// misc.
//

// Gets the column with the matching schema tag name or 0.
SoaCl *SoaTb::SchemaCl(const std::string &_name, SoaClNameType nameType) {

	for (SoaClVector::iterator cl = columns.begin(); cl != columns.end(); ++cl)
		if ((nameType == scnDatabase && cl->name == name) ||
			(nameType == scnSchema && cl->schema.schema == _name) ||
			(nameType == scnMerge && cl->schema.merge == _name))
			return &(*cl);

	return 0;
}

// Prints a TiXmlDocument into a string.
std::string SoaTb::PrintXmlDocToString(TiXmlDocument &doc) {

	TiXmlPrinter p;
	p.SetIndent("\t");
	doc.Accept(&p);
	return p.Str();
}

TiXmlDocument SoaTb::CreateBlankAuxDataXml() {

	TiXmlDocument doc("schema");

	TiXmlElement *r = new TiXmlElement(name);
	doc.LinkEndChild(new TiXmlDeclaration("1.0", "utf-8", ""));
	doc.LinkEndChild(r);

	for (SoaClVector::iterator cl = auxColumns.begin(); cl != auxColumns.end(); ++cl) {
		TiXmlElement *e = new TiXmlElement(cl->Name());
		r->LinkEndChild(e);
	}

	return doc;
}
