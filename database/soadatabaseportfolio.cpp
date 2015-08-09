#include "soadatabase.h"
#include "soalogger.h"
#include "tinyxml/tinyxml.h"
#include <iostream>
#include <sstream>



void SoaDb::DeserializeSchemas() {

	SoaRw *hs = GetHousekeeping();
	SoaTb *apps = Tb("Application");

	if (hs == 0 || apps == 0) {
		sl::log << "DeserializeSchemas error: could not find housekeeping singleton or applications table" << sl::end;
		return;
	}

	apps->DeserializeSchema(hs->Ce("portfolioSchema")->Txt());
}
