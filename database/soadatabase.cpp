#include "soadatabase.h"
#include "soalogger.h"



SoaDb::SoaDb() : selId(1), changeId(0), selTb(0) {

	push_back(new SoaTb(this, "Housekeeping"));
	back()->AddCl(SoaCl("name", sctTxt, SoaClSc("Name")), 1);
	back()->AddCl(SoaCl("portfolioSchema"));

	push_back(new SoaTb(this, "Organization"));
	back()->AddCl(SoaCl("name", sctTxt, SoaClSc("Name")), 1);
	back()->AddCl(SoaCl("sourceURL", sctTxt));

	push_back(new SoaTb(this, "BusinessUnit"));
	back()->AddCl(SoaCl("name", sctTxt, SoaClSc("Name")), 1);
	back()->AddCl(SoaCl(Tb("Organization"), SoaClSc("Organization")));	// !
	back()->AddCl(SoaCl("sourceURL", sctTxt));

	push_back(new SoaTb(this, "InfrastructureDomain"));
	back()->AddCl(SoaCl("name", sctTxt), 1);
	back()->AddCl(SoaCl(Tb("Organization")));
	back()->AddCl(SoaCl("sourceURL", sctTxt));

	push_back(new SoaTb(this, "RuntimeRegistry"));
	back()->AddCl(SoaCl("name", sctTxt, SoaClSc("name")), 1);	// !
	back()->AddCl(SoaCl("type", sctTxt, SoaClSc("type")));	// !
	back()->AddCl(SoaCl("url", sctTxt, SoaClSc("URL")));	// !
	back()->AddCl(SoaCl("securityDomain", sctTxt));
	back()->AddCl(SoaCl("username", sctTxt, SoaClSc("username")));	// !
	back()->AddCl(SoaCl("sourceURL", sctTxt));

	push_back(new SoaTb(this, "ServiceDomain"));
	back()->AddCl(SoaCl("name", sctTxt, SoaClSc("Name")), 1);
	back()->AddCl(SoaCl(Tb("Organization"), SoaClSc("Organization")));	// !
	back()->AddCl(SoaCl("sourceURL", sctTxt));

	push_back(new SoaTb(this, "Address"));
	back()->AddCl(SoaCl("address", sctTxt), 1);
	back()->AddCl(SoaCl("ip", sctInt), 2);
	back()->AddCl(SoaCl(Tb("ServiceDomain")));

	push_back(new SoaTb(this, "SystemManager"));
	back()->AddCl(SoaCl("userid", sctTxt), 1);
	back()->AddCl(SoaCl(Tb("Organization")));
	back()->AddCl(SoaCl("sourceURL", sctTxt));

	push_back(new SoaTb(this, "Interface"));
	back()->AddCl(SoaCl("name", sctTxt), 1);
	back()->AddCl(SoaCl(Tb("BusinessUnit")));
	back()->AddCl(SoaCl("sourceURL", sctTxt));

	push_back(new SoaTb(this, "DataDefinition"));
	back()->AddCl(SoaCl("name", sctTxt), 1);
	back()->AddCl(SoaCl(Tb("BusinessUnit")));
	back()->AddCl(SoaCl("sourceURL", sctTxt));

	push_back(new SoaTb(this, "Server"));
	back()->AddCl(SoaCl("name", sctTxt), 1);
	back()->AddCl(SoaCl(Tb("InfrastructureDomain")));	// ! izlistat / racunski centar
	back()->AddCl(SoaCl("sourceURL", sctTxt));
	// service domain

	push_back(new SoaTb(this, "ServerAddress"));
	back()->AddCl(SoaCl(Tb("Address")), 1);
	back()->AddCl(SoaCl(Tb("Server")), 2);

	push_back(new SoaTb(this, "Administrator"));
	back()->AddCl(SoaCl("userid", sctTxt), 1);
	back()->AddCl(SoaCl(Tb("BusinessUnit")));
	back()->AddCl(SoaCl("sourceURL", sctTxt));

	push_back(new SoaTb(this, "Application"));
	back()->AddCl(SoaCl("name", sctTxt, SoaClSc("Name")), 1);
	back()->AddCl(SoaCl("sourceURL", sctTxt));
	back()->AddCl(SoaCl("external", sctInt));
	back()->AddCl(SoaCl(Tb("BusinessUnit"), SoaClSc("BusinessUnit")));
	back()->AddCl(SoaCl("singleUser", sctInt));

	push_back(new SoaTb(this, "Service"));
	back()->AddCl(SoaCl("name", sctTxt, SoaClSc("Name")), 1);
	back()->AddCl(SoaCl("sourceURL", sctTxt));
	back()->AddCl(SoaCl("external", sctInt));
	back()->AddCl(SoaCl("type", sctTxt));
	back()->AddCl(SoaCl(Tb("Application"), SoaClSc("Application")));

	push_back(new SoaTb(this, "ApplicationVersion"));
	back()->AddCl(SoaCl("version", sctTxt, SoaClSc("Version")), 2);	// !
	back()->AddCl(SoaCl("sourceURL", sctTxt));
	back()->AddCl(SoaCl(Tb("Application"), SoaClSc("Application")), 1);	// !

	push_back(new SoaTb(this, "DeployedApplication"));
	back()->AddCl(SoaCl(Tb("Server")));
	back()->AddCl(SoaCl(Tb("ApplicationVersion"), SoaClSc("ApplicationVersion")), 1);	// !
	back()->AddCl(SoaCl(Tb("ServiceDomain")));	// !
	back()->AddCl(SoaCl("name", sctTxt));
	back()->AddCl(SoaCl("sourceURL", sctTxt));
	back()->AddCl(SoaCl("multipleAddresses", sctInt));

	push_back(new SoaTb(this, "DeployedApplicationServer"));
	back()->AddCl(SoaCl(Tb("DeployedApplication")), 1);
	back()->AddCl(SoaCl(Tb("Server")), 2);

	push_back(new SoaTb(this, "DataDefinitionVersion"));
	back()->AddCl(SoaCl("version", sctTxt));
	back()->AddCl(SoaCl("XSD", sctTxt));
	back()->AddCl(SoaCl(Tb("DataDefinition")));
	back()->AddCl(SoaCl("sourceURL", sctTxt));

	push_back(new SoaTb(this, "InterfaceVersion"));
	back()->AddCl(SoaCl("version", sctTxt));
	back()->AddCl(SoaCl("WSDL", sctTxt));
	back()->AddCl(SoaCl(Tb("Interface")));
	back()->AddCl(SoaCl(Tb("DataDefinitionVersion")));
	back()->AddCl(SoaCl("sourceURL", sctTxt));

	push_back(new SoaTb(this, "ServiceVersion"));
	back()->AddCl(SoaCl("version", sctTxt, SoaClSc("Version")), 2);	// !
	back()->AddCl(SoaCl("WSDL", sctTxt));
	back()->AddCl(SoaCl(Tb("Service"), SoaClSc("Service", false)), 1);	// !
	back()->AddCl(SoaCl(Tb("ApplicationVersion"), SoaClSc("ApplicationVersion")));	// !
	back()->AddCl(SoaCl(Tb("InterfaceVersion")));
	back()->AddCl(SoaCl(Tb("DataDefinitionVersion")));
	back()->AddCl(SoaCl("sourceURL", sctTxt));

	push_back(new SoaTb(this, "DeployedService"));
	back()->AddCl(SoaCl("targetNameSpace", sctTxt));
	back()->AddCl(SoaCl("definitionName", sctTxt));
	back()->AddCl(SoaCl("protectedEndpoint", sctTxt));
	back()->AddCl(SoaCl(Tb("ServiceVersion"), SoaClSc("ServiceVersion")), 1);	// !
	back()->AddCl(SoaCl(Tb("DeployedApplication"), SoaClSc("DeployedApplication")));	// !
	back()->AddCl(SoaCl("sourceURL", sctTxt));

	push_back(new SoaTb(this, "Intermediary"));
	back()->AddCl(SoaCl("name", sctTxt, SoaClSc("Name")), 1);
	back()->AddCl(SoaCl(Tb("ServiceDomain"), SoaClSc("ServiceDomain")));
	back()->AddCl(SoaCl(Tb("RuntimeRegistry")));
	back()->AddCl(SoaCl(Tb("InfrastructureDomain")));
	back()->AddCl(SoaCl("sourceURL", sctTxt));

	push_back(new SoaTb(this, "IntermediaryServer"));
	back()->AddCl(SoaCl(Tb("Intermediary")), 1);
	back()->AddCl(SoaCl(Tb("Server")), 2);

	push_back(new SoaTb(this, "DesignTimeServiceUse"));
	back()->AddCl(SoaCl(Tb("ServiceVersion"), SoaClSc("ServiceVersion")));
	back()->AddCl(SoaCl(Tb("ApplicationVersion"), SoaClSc("ApplicationVersion")));
	back()->AddCl(SoaCl("sourceURL", sctTxt));

	push_back(new SoaTb(this, "Policy"));	// moguce da ce se izbaciti
	back()->AddCl(SoaCl("name", sctTxt));
	back()->AddCl(SoaCl(Tb("Intermediary")));
	back()->AddCl(SoaCl("sourceURL", sctTxt));

	push_back(new SoaTb(this, "Endpoint"));
	back()->AddCl(SoaCl(Tb("ServiceDomain")));
	back()->AddCl(SoaCl(Tb("DeployedService"), SoaClSc("DeployedService")));	// !
	back()->AddCl(SoaCl(Tb("Intermediary")));
	back()->AddCl(SoaCl("intermediaryGivenName", sctTxt), 1);
	back()->AddCl(SoaCl("policyVersion", sctTxt));
	back()->AddCl(SoaCl("intermediaryGivenVersion", sctTxt));
	back()->AddCl(SoaCl("intermediaryGivenId", sctTxt));
	back()->AddCl(SoaCl("resolutionPath", sctTxt));
	back()->AddCl(SoaCl("enabled", sctTxt));
	back()->AddCl(SoaCl("soap", sctTxt));
	back()->AddCl(SoaCl("urlWSDL", sctTxt));
	back()->AddCl(SoaCl("urlRegistryFolder", sctTxt));
	back()->AddCl(SoaCl("sourceURL", sctTxt));
	back()->AddCl(SoaCl("lastUpdate", sctTxt));
	back()->AddCl(SoaCl("serviceNameByUser", sctTxt));

	push_back(new SoaTb(this, "RuntimeServiceUse"));
	back()->AddCl(SoaCl(Tb("Endpoint"), SoaClSc("Endpoint")));	// !
	back()->AddCl(SoaCl(Tb("DeployedApplication"), SoaClSc("DeployedApplication")));	// !
	back()->AddCl(SoaCl("sourceURL", sctTxt));
	back()->AddCl(SoaCl("createdManually", sctInt));

	//
	// viewing hints
	//

	SoaTb *tb = Tb("Organization");
	tb->AddTableColumnHint(TableColumnHintNames() << "name");
	tb->AddTableColumnHint(TableColumnHintNames() << "description", tcsFixedWide);

	tb = Tb("BusinessUnit");
	tb->AddTableColumnHint(TableColumnHintNames() << "name");
	tb->AddTableColumnHint(TableColumnHintNames() << "description", tcsFixedWide);
	tb->AddTableColumnHint(TableColumnHintNames() << "Organization");

	tb = Tb("Application");
	tb->AddTableColumnHint(TableColumnHintNames() << "name");
	tb->AddTableColumnHint(TableColumnHintNames() << "BusinessUnit");
	tb->AddTableColumnHint(TableColumnHintNames() << "description", tcsFixedWide);

	tb = Tb("ApplicationVersion");
	tb->AddTableColumnHint(TableColumnHintNames() << "Application");
	tb->AddTableColumnHint(TableColumnHintNames() << "Application" << "BusinessUnit");
	tb->AddTableColumnHint(TableColumnHintNames() << "description", tcsFixedWide);
	tb->AddTableColumnHint(TableColumnHintNames() << "version");

	tb = Tb("DeployedApplication");
	tb->AddTableColumnHint(TableColumnHintNames() << "ApplicationVersion" << "Application");
	tb->AddTableColumnHint(TableColumnHintNames() << "ApplicationVersion" << "Application" << "BusinessUnit");
	tb->AddTableColumnHint(TableColumnHintNames() << "description", tcsFixedWide);
	tb->AddTableColumnHint(TableColumnHintNames() << "ApplicationVersion" << "version");
	tb->AddTableColumnHint(TableColumnHintNames() << "ServiceDomain");

	tb = Tb("Service");
	tb->AddTableColumnHint(TableColumnHintNames() << "name");
	tb->AddTableColumnHint(TableColumnHintNames() << "description", tcsFixedWide);
	tb->AddTableColumnHint(TableColumnHintNames() << "Application");

	tb = Tb("ServiceVersion");
	tb->AddTableColumnHint(TableColumnHintNames() << "Service");
	tb->AddTableColumnHint(TableColumnHintNames() << "Service" << "Application" << "BusinessUnit");
	tb->AddTableColumnHint(TableColumnHintNames() << "description", tcsFixedWide);
	tb->AddTableColumnHint(TableColumnHintNames() << "version");
	tb->AddTableColumnHint(TableColumnHintNames() << "ApplicationVersion" << "Application");
	tb->AddTableColumnHint(TableColumnHintNames() << "ApplicationVersion" << "version");

	tb = Tb("DeployedService");
	tb->AddTableColumnHint(TableColumnHintNames() << "ServiceVersion" << "Service");
	tb->AddTableColumnHint(TableColumnHintNames() << "ServiceVersion" << "Service" << "BusinessUnit");
	tb->AddTableColumnHint(TableColumnHintNames() << "description", tcsFixedWide);
	tb->AddTableColumnHint(TableColumnHintNames() << "ServiceVersion" << "version");
	tb->AddTableColumnHint(TableColumnHintNames() << "DeployedApplication" << "ApplicationVersion" << "Application");
	tb->AddTableColumnHint(TableColumnHintNames() << "DeployedApplication" << "ApplicationVersion" << "version");
	tb->AddTableColumnHint(TableColumnHintNames() << "DeployedApplication" << "ServiceDomain");

	ResetHousekeeping();
}

SoaDb::~SoaDb() {

	for (SoaTbVector::reverse_iterator t = rbegin(); t != rend(); ++t)
		(*t)->Clear();

	for (SoaTbVector::iterator t = begin(); t != end(); ++t)
		delete *t;
}

// Clears the model data.
void SoaDb::Clear() {
	for (SoaTbVector::iterator table = begin(); table != end(); ++table)
		(*table)->Clear();

	ResetHousekeeping();
	SetChanged();
}

unsigned int SoaDb::SelId() const {
	return selId;
}

unsigned int SoaDb::ChangeId() const {
	return changeId;
}

void SoaDb::SetChanged() {
	++changeId;
}

//
// search
//

SoaTb *SoaDb::Tb(const std::string &tableName) {
	for (SoaTbVector::iterator table = begin(); table != end(); ++table)
		if ((*table)->Name() == tableName)
			return *table;

	sl::log << "table not found: " << tableName << sl::end;

	return 0;
}

SoaRw *SoaDb::GetHousekeeping() {
	SoaTb *hs = Tb("Housekeeping");
	return (hs && hs->size()) ? hs->back() : 0;
}

//
// selection
//

void SoaDb::DeselectAll() {
	selId++;
}

void SoaDb::InvertRowSelection(unsigned int rowId) {

	if (rowId % 100 < size() && at(rowId % 100)->InvertRowSelection(rowId, selId)) {
		selTb = at(rowId % 100);
		return;
	}

	selTb = 0;
}
