#include "database/soalogger.h"
#include "database/soatable.h"
#include "tinyxml/tinyxml.h"

typedef std::map<std::string, std::string>	StrStrMap;
typedef std::pair<std::string, std::string>	StrStrPair;



// If schema is valid loads the portfolio xml,
// parses it, composes xml entries for the
// xml column, and calls DistributeXml().
void SoaTb::LoadPortfolio(const std::string &path) {
	sl::log << "Loading portfolio from: " << path << sl::end;

	if (!IsSchemaValid()) {
		sl::log << "Error loading portfolio: schema not valid" << sl::end;
		return;
	}

	TiXmlDocument doc("portfolio");
	if (doc.LoadFile(path)) {

		// create xml schema template to be used for printing portfolio data in xml form

		TiXmlDocument xml = CreateBlankAuxDataXml();

		// compile critical cells

		std::vector<SoaCl *> critCls;
		for (SoaClVector::iterator cl = columns.begin(); cl != columns.end(); ++cl)
			if (cl->schema.critical)
				critCls.push_back(&(*cl));

		// each element 'e' is a node representing an asset, and contains property nodes

		for (TiXmlElement *e = doc.FirstChildElement()->FirstChildElement(); e; e = e->NextSiblingElement()) {

			// load portfolio asset property key/value pairs into a string/string map

			StrStrMap rawPrs;
			for (TiXmlElement *s = e->FirstChildElement(); s; s = s->NextSiblingElement()) {
				const char *text = s->GetText();
				if (text)
					rawPrs.insert(StrStrPair(s->ValueStr(), std::string(text)));
			}

			// check if portfolio entry contains critical data

			std::vector<std::string> critValues;
			for (std::vector<SoaCl *>::iterator cl = critCls.begin(); cl != critCls.end(); ++cl) {

				StrStrMap::iterator p = rawPrs.find((*cl)->schema.mangledSchema);
				if (p == rawPrs.end()) {
					sl::log << "Error loading portfolio entry for table '" << name << "': critical column named '" << (*cl)->schema.mangledSchema << "' not found" << sl::end;
					break;
				}
				else {
					critValues.push_back(p->second);
					rawPrs.erase(p);
				}
			}

			if (critValues.size() != critCls.size())	// if asset portfolio lacks critical data, abort loading current asset
				continue;

			// merge aux properties into schema definition

			StrStrMap prs;
			for (SoaClVector::iterator cl = auxColumns.begin(); cl != auxColumns.end(); ++cl) {

				StrStrMap::iterator p = rawPrs.find(cl->Name());
				if (p == rawPrs.end())
					prs.insert(StrStrPair(cl->Name(), ""));	// aux property missing, add empty
				else
					prs.insert(StrStrPair(cl->Name(), p->second));
			}

			// insert portfolio data into the blank xml

			for (TiXmlElement *e = xml.FirstChildElement()->FirstChildElement(); e; e = e->NextSiblingElement()) {

				e->Clear();
				StrStrMap::iterator p = prs.find(e->ValueStr());

				if (p != prs.end())
					e->LinkEndChild(new TiXmlText(p->second));
			}

			// find row (asset) using critical data

			Search();
			for (unsigned int i = 0; i < critCls.size(); ++i)
				pattern.Wt(critCls[i]->name.c_str(), critValues[i].c_str(), critCls[i]->Fk() != 0);
			SoaRw *row = pattern.Get(this, true);

			if (row) {

				// merge non-critical values into normal cells

				for (SoaClVector::iterator cl = columns.begin(); cl != columns.end(); ++cl)
					if (!cl->schema.critical) {

						StrStrMap::iterator entry = rawPrs.find(cl->schema.mangledSchema);
						if (entry != rawPrs.end()) {

							row->SetCe(cl->Index(), SoaCe(entry->second));
							rawPrs.erase(entry);
						}
					}

				row->SetCe(XML_COL_INDEX, PrintXmlDocToString(xml));
			}
			else {
				sl::log << "Error finding '" << name << "' table row that matches portfolio entry" << sl::end;
			}
		}

		DistributeXml();
	}
	else
		sl::log << "Error loading portfolio for table '" << name << "' from: " << path << sl::end;
}

// If schema is valid, for each row that has any content
// in its xml column cell, parses the xml content and
// distributes node values into its aux cells.
// If xml content is blank or
void SoaTb::DistributeXml() {
	sl::log << "Distributing xml for table '" << name << "'"<< sl::end;

	if (!IsSchemaValid()) {
		sl::log << "Error distributing xml: schema not valid" << sl::end;
		return;
	}

	TiXmlDocument blank = CreateBlankAuxDataXml();
	std::string blankXml = PrintXmlDocToString(blank);

	for (SoaTb::iterator row = begin(); row != end(); ++row) {

		const SoaCe *xmlCell = (*row)->Ce(XML_COL_INDEX);
		const std::string &xml = xmlCell->Txt();
		if (xml != "") {

			TiXmlDocument doc("auxxml");
			if (doc.Parse(xml.c_str())) {

				TiXmlElement *element = doc.FirstChildElement()->FirstChildElement();
				for (; element; element = element->NextSiblingElement()) {

					const SoaCl *cl = AuxCl(element->ValueStr());
					if (cl)
						(*row)->SetAuxCe(element->ValueStr().c_str(), SoaCe(element->GetText()));
					else {
						(*row)->ResetAuxCells();	// reset aux cells
						(*row)->SetCe(XML_COL_INDEX, SoaCe(blankXml));	// sets blank xml
						sl::log << "DistributeXml error: unidentified field name, portfolio data will be reset to default" << sl::end;
						break;
					}
				}
			}
			else {
				(*row)->ResetAuxCells();	// reset aux cells
				(*row)->SetCe(XML_COL_INDEX, SoaCe(blankXml));	// sets blank xml
				sl::log << "DistributeXml parse error: " << doc.ErrorDesc() << ", portfolio data will be reset to default" << sl::end;
			}
		}
		else {
			(*row)->ResetAuxCells();	// reset aux cells
			(*row)->SetCe(XML_COL_INDEX, SoaCe(blankXml));	// sets blank xml
		}
	}
}
