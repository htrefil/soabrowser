#include "soadatabase.h"
#include "soalogger.h"
#include "sqlite3.h"
#include "tinyxml/tinyxml.h"
#include <iostream>
#include <fstream>



bool SoaDb::FromDB(const std::string &path) {
	sqlite3 *db;

	Clear();

	bool opened = false;
	if (sqlite3_open(path.c_str(), &db) == SQLITE_OK) {
		sl::log << path.c_str() << " database connected, loading data" << sl::end;

		for (SoaTbVector::iterator table = begin(); table != end(); ++table)
			(*table)->FromDB(db);

		if (ResolveKeys())
			DeserializeSchemas();

		opened = true;
	}
	else
		sl::log << "sqlite3_open() error: " << sqlite3_errmsg(db) << sl::end;

	sqlite3_close(db);
	SetChanged();

	return opened;
}

void SoaDb::FromSQL(const std::string &source) {
	sqlite3 *db;
	char *errMsg;

	Clear();

	if (sqlite3_open(":memory:", &db) == SQLITE_OK) {
		sl::log << ":memory: database connected, loading SQL" << sl::end;

		if (sqlite3_exec(db, source.c_str(), 0, 0, &errMsg) == SQLITE_OK) {
			sl::log << "SQL executed successfully, copying data" << sl::end;

			for (SoaTbVector::iterator table = begin(); table != end(); ++table)
				(*table)->FromDB(db);

			ResolveKeys();
		}
		else {
			sl::log << "sqlite3_exec() error: " << errMsg << sl::end;
			sqlite3_free(errMsg);
		}
	}
	else
		sl::log << "sqlite3_open() error: " << sqlite3_errmsg(db) << sl::end;

	sqlite3_close(db);
	SetChanged();
}

void SoaDb::ResetHousekeeping() {

	SoaTb *hs = Tb("Housekeeping");
	hs->Clear();
	hs->Search().Wt("name", "Housekeeping").Get(hs);

	SetChanged();
}

// Imports the db from a CentraSite XML report.
// Clears the database and creates a blank
// housekeeping singleton.
void SoaDb::FromXML(const std::string &path) {

	TiXmlDocument report(path.c_str());
	if (report.LoadFile(TIXML_ENCODING_UTF8)) {

		Clear();
		ResetHousekeeping();

		TiXmlNode *rootNode = report.FirstChild();
		while (rootNode) {
			if (rootNode->ValueStr() == "rep") {

				// create organization

				SoaTb *orgs = Tb("Organization");
				SoaTb *depts = Tb("BusinessUnit");
				SoaTb *apps = Tb("Application");
				SoaTb *appVersions = Tb("ApplicationVersion");
				SoaTb *services = Tb("Service");
				SoaTb *serviceVersions = Tb("ServiceVersion");
				SoaTb *designTimeServiceUses = Tb("DesignTimeServiceUse");

				SoaRw *org = orgs->Search().Wt("name", "Organization").Get(orgs);

				TiXmlNode *soaNode = rootNode->FirstChild();
				while (soaNode) {
					if (soaNode->ValueStr() == "applications") {

						TiXmlElement *appNode = soaNode->FirstChildElement();
						while (appNode) {
							if (appNode->ValueStr() == "application") {

								// add application

								const char *appName = appNode->Attribute("name");
								const char *appDep = appNode->Attribute("organization");
								const char *appVersion = appNode->Attribute("version");

								SoaRw *dept = depts->Search().Wt("name", appDep).Wt("Organization", org).Get(depts);
								SoaRw *app = apps->Search().Wt("name", appName).Wt("BusinessUnit", dept).Wt("external", false).Get(apps);
								SoaRw *appVer = appVersions->Search().Wt("version", appVersion).Wt("Application", app).Get(appVersions);

								TiXmlNode *appSubNode = appNode->FirstChild();
								while (appSubNode) {
									if (appSubNode->ValueStr() == "implements") {

										TiXmlElement *serviceNode = appSubNode->FirstChildElement();
										while (serviceNode) {
											if (serviceNode->ValueStr() == "service") {

												// add service

												const char *serviceName = serviceNode->Attribute("name");
												const char *serviceVersion = serviceNode->Attribute("version");

												SoaRw *service = services->Search()
														.Wt("name", serviceName)
														.Wt("BusinessUnit", dept)
														.Wt("Application", app)
														.Wt("external", false).Get(services);
												serviceVersions->Search()
														.Wt("version", serviceVersion)
														.Wt("Service", service)
														.Wt("ApplicationVersion", appVer).Get(serviceVersions);
											}
											else
												sl::log << "ignored 'implements' node sub node: " << serviceNode->ValueStr() << sl::end;

											serviceNode = serviceNode->NextSiblingElement();
										}
									}
									else if (appSubNode->ValueStr() == "description") {
										const char *appDescription = appSubNode->ToElement()->GetText();

										app->SetCe("description", SoaCe(appDescription));
									}
									else
										sl::log << "ignored 'application' node sub node: " << appSubNode->ValueStr() << sl::end;

									appSubNode = appSubNode->NextSibling();
								}
							}
							else
								sl::log << "ignored 'applications' node sub node: " << appNode->ValueStr() << sl::end;

							appNode = appNode->NextSiblingElement();
						}
					}
					else if (soaNode->ValueStr() == "applicationServers") {
						// skip
					}
					else if (soaNode->ValueStr() == "organizations") {

						TiXmlElement *deptNode = soaNode->FirstChildElement();
						while (deptNode) {
							if (deptNode->ValueStr() == "organization") {
								const char *deptName = deptNode->Attribute("name");

								SoaRw *dept = depts->Search().Wt("name", deptName).Wt("Organization", org).Get(depts);

								TiXmlNode *deptSubNode = deptNode->FirstChild();
								while (deptSubNode) {
									if (deptSubNode->ValueStr() == "services") {

										TiXmlElement *serviceNode = deptSubNode->FirstChildElement();
										while (serviceNode) {
											if (serviceNode->ValueStr() == "service") {
												const char *serviceName = serviceNode->Attribute("name");
												const char *serviceVersion = serviceNode->Attribute("version");

												SoaRw *service = services->Search().Wt("name", serviceName).Get();

												SoaRw *serviceVer = serviceVersions->Search()
														.Wt("version", serviceVersion)
														.Wt("Service", service)
														.Get();

												if (serviceVer) {

													TiXmlElement *serviceSubNode = serviceNode->FirstChildElement();
													while (serviceSubNode) {
														if (serviceSubNode->ValueStr() == "consumedby") {

															TiXmlElement *appNode = serviceSubNode->FirstChildElement();
															while (appNode) {
																if (appNode->ValueStr() == "application") {
																	const char *appName = appNode->Attribute("name");
																	const char *appDept = appNode->Attribute("organization");
																	const char *appVer = appNode->Attribute("version");

																	SoaRw *dept = depts->Search()
																			.Wt("name", appDept)
																			.Get();

																	SoaRw *app = apps->Search()
																			.Wt("name", appName)
																			.Wt("BusinessUnit", dept)
																			.Get();

																	SoaRw *appVersion = appVersions->Search()
																			.Wt("version", appVer)
																			.Wt("Application", app)
																			.Get();

																	if (appVersion) {
																		designTimeServiceUses->Search()
																				.Wt("ServiceVersion", serviceVer)
																				.Wt("ApplicationVersion", appVersion)
																				.Get(designTimeServiceUses);
																	}
																}

																appNode = appNode->NextSiblingElement();
															}
														}

														serviceSubNode = serviceSubNode->NextSiblingElement();
													}
												}
											}
											else
												sl::log << "ignored 'services' node sub node: " << serviceNode->ValueStr() << sl::end;

											serviceNode = serviceNode->NextSiblingElement();
										}
									}
									else if (deptSubNode->ValueStr() == "description") {
										const char *deptDescription = deptSubNode->ToElement()->GetText();

										dept->SetCe("description", SoaCe(deptDescription));
									}
									else
										sl::log << "ignored 'organization' node sub node: " << deptSubNode->ValueStr() << sl::end;

									deptSubNode = deptSubNode->NextSibling();
								}
							}
							else
								sl::log << "ignored 'organizations' node sub node: " << deptNode->ValueStr() << sl::end;

							deptNode = deptNode->NextSiblingElement();
						}
					}
					else
						sl::log << "unhandled CentraSite report node name: " << soaNode->ValueStr() << sl::end;

					soaNode = soaNode->NextSibling();
				}

				break;
			}
			else
				rootNode = rootNode->NextSibling();
		}

		ResolveKeys();
	}
	else
		sl::log << "could not open " << path << sl::end;

	SetChanged();
}

bool SoaDb::ResolveKeys() {

	bool ok = true;
	for (SoaTbVector::iterator table = begin(); table != end(); ++table)
		if (!(*table)->ResolveKeys())
			ok = false;

	if (!ok)
		Clear();

	return ok;
}

//
// serialize
//

void SoaDb::ToDB(const std::string &path) {

	sqlite3 *db;
	char *errMsg;

	std::ofstream file;
	file.open(path.c_str(), std::ios::out | std::ios::trunc);
	file.close();

	if (sqlite3_open(path.c_str(), &db) == SQLITE_OK) {
		sl::log << path << " database connected, generating SQL" << sl::end;

		std::string sql;
		ToSQL(sql, sptSQLite);

		if (sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg) == SQLITE_OK)
			sl::log << "database saved" << sl::end;
		else {
			sl::log << "sqlite3_exec() error: " << errMsg << sl::end;
			sqlite3_free(errMsg);
		}
	}
	else
		sl::log << "sqlite3_open() error: " << sqlite3_errmsg(db) << sl::end;

	sqlite3_close(db);
}

std::string &SoaDb::ToSQL(std::string &sql, SoaPlType platform) {

	sql += "BEGIN TRANSACTION;\n";

	for (SoaTbVector::iterator table = begin(); table != end(); ++table)
		(*table)->ToSQL(sql, false, platform);

	sql += "COMMIT TRANSACTION;\nBEGIN TRANSACTION;\n";

	for (SoaTbVector::iterator table = begin(); table != end(); ++table)
		(*table)->ToSQL(sql, true, platform);

	sql += "COMMIT TRANSACTION;\n";

	return sql;
}
