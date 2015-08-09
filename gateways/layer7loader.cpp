#include "gateways/layer7loader.h"
#include <QNetworkReply>
#include <QDomDocument>
#include <iostream>
#include "database/soalogger.h"
#include "logger/logger.h"



Layer7::Layer7() {

	op = lsoStop;

	connect(&http, SIGNAL(finished(QNetworkReply*)), this, SLOT(LoadSlot(QNetworkReply*)));
	connect(&http, SIGNAL(ErrorSignal(QString)), this, SLOT(ErrorSlot(QString)));
}

void Layer7::Load(unsigned int regId, const QString &regName, const QString &url, const QString &usn, const QString &pwd, const QString &dom, const QStringList &_sdoms) {

	registryId = regId;
	registryName = regName;

	Reset();

	op = lsoStart;
	sdoms = _sdoms;

	http.SetUrl(url);
	http.SetAuthentication(usn, pwd, dom);

	sl::log << "load started" << sl::end;

	LoadSlot(0);
}

void Layer7::Cancel() {

	op = lsoStop;
	http.Abort();
	emit DoneSignal("Canceled.");
}

void Layer7::LoadSlot(QNetworkReply *reply) {

	switch (op) {

		case lsoStart:

			emit UpdateSignal("Loading started...");
			Logger::ins.Header() << "Loading registry data from " << http.Url() << "data/ started...\r\n";
			http.Propfind(http.Url() + "data/");
			op = lsoListDomains;

			break;

		case lsoListDomains:

			if (reply->error() == QNetworkReply::NoError) {

				QString replyString(reply->readAll());
				sl::log << "lsoListDomains: " << SQ(replyString) << sl::end;

				Propfinds elements = http.ParsePropfindReply(replyString);
				for (Propfinds::iterator e = elements.begin(); e != elements.end(); ++e)
					if (e->href != http.Url()) {
						domains.push_back(GtDomain(SQ(e->href), SQ(e->name)));
						Logger::ins.Info() << "Service domain: " << e->name << "\r\n";
						emit UpdateSignal("Service Domain: " + e->name);
					}

				if (SelectFirstDomain()) {

					loaded = true;

					// check if all domains match domains from the db

					for (unsigned int d = 0; d < domains.size(); ++d)
						if (!sdoms.contains(QS(domains[d].name))) {
							loaded = false;
							Logger::ins.Error() << "Repository does not contain service domain named " << QS(domains[d].name) << "\r\n";
							emit DoneSignal("Repository does not contain service domain named " + QS(domains[d].name));
						}

					if (loaded) {
						op = lsoListServices;
						http.Propfind(QS(SelectedDomain().href) + "web/");
					}
				}
			}
			else
				emit UpdateSignal(reply->errorString());

			break;

		case lsoListServices:

			if (reply->error() == QNetworkReply::NoError) {

				QString replyString(reply->readAll());
				sl::log << "lsoListServices: " << SQ(replyString) << sl::end;

				Propfinds elements = http.ParsePropfindReply(replyString);
				for (Propfinds::iterator e = elements.begin(); e != elements.end(); ++e)
					if (e->href != http.Url() && e->name != "OLD" && e->name != "REMOVED") {

						endpoints.push_back(GtEndpoint(item++ * 100 + REPO_TB_IDX, SelectedDomain().name, SQ(e->href), SQ(e->name)));
						Logger::ins.Info() << "Service: " << e->name << "\r\n";
						emit UpdateSignal("Service: " + e->name);
					}
			}
			else
				emit UpdateSignal(reply->errorString());

			if (SelectNextDomain())
				http.Propfind(QS(SelectedDomain().href) + "web/");
			else if (SelectFirstEndpoint()) {
				Logger::ins.Info() << "Parsing index files...\r\n";
				emit UpdateSignal("Parsing index files...");
				op = lsoGetIndexXml;
				Logger::ins.Info() << "Parsing " << QS(SelectedEndpoint().urlRegistryFolder) << "index.xml...\r\n";
				http.Get(QS(SelectedEndpoint().urlRegistryFolder) + "index.xml");
			}

			break;

		case lsoGetIndexXml:

			if (reply->error() == QNetworkReply::NoError) {

				QString replyString(reply->readAll());
				sl::log << "lsoGetIndexXml: " << SQ(replyString) << sl::end;

				QDomDocument doc;
				doc.setContent(replyString);

				for (QDomElement r = doc.firstChildElement(); !r.isNull(); r = r.nextSiblingElement()) {

					if (r.nodeName() == "service") {

						for (QDomElement e = r.firstChildElement(); !e.isNull(); e = e.nextSiblingElement()) {

							if (e.nodeName() == "files") {

								for (QDomElement f = e.firstChildElement(); !f.isNull(); f = f.nextSiblingElement()) {

									QDomAttr r = f.attributeNode("root"), t = f.attributeNode("type");
									if (!r.isNull() && r.nodeValue() == "true" && !t.isNull() && t.value() == "WSDL")
										SelectedEndpoint().urlWSDL = SQ(f.text());
								}
							}
							else if (e.nodeName() == "name")
								SelectedEndpoint().intGivenName = SQ(e.text());
							else if (e.nodeName() == "version")
								SelectedEndpoint().intGivenVersion = SQ(e.text());
							else if (e.nodeName() == "policyVersion")
								SelectedEndpoint().policyVersion = SQ(e.text());
							else if (e.nodeName() == "enabled")
								SelectedEndpoint().enabled = SQ(e.text());
							else if (e.nodeName() == "soap")
								SelectedEndpoint().soap = SQ(e.text());
							else if (e.nodeName() == "resolutionPath")
								SelectedEndpoint().resolutionPath = SQ(e.text());
							else if (e.nodeName() == "id")
								SelectedEndpoint().intGivenId = SQ(e.text());
							else if (e.nodeName() == "protectedEndpoint")
								SelectedEndpoint().protectedEndpoint = SQ(e.text());
						}

						break;
					}
				}

				if (SelectedEndpoint().urlWSDL == "") {
					Logger::ins.Warning() << "WSDL file url not found for endpoint '" << QS(SelectedEndpoint().intGivenName) << "'\r\n";
					emit UpdateSignal("WSDL file url not found for endpoint '" + QS(SelectedEndpoint().intGivenName) + "'");
					SelectedEndpoint().serviceName = SelectedEndpoint().intGivenName;
				}

				if (SelectedEndpoint().protectedEndpoint == "") {
					Logger::ins.Warning() << "Ignoring non-protected endpoint '" << QS(SelectedEndpoint().intGivenName) << "'\r\n";
					emit UpdateSignal("Ignoring non-protected endpoint '" + QS(SelectedEndpoint().intGivenName) + "'");
					RemoveSelectedEndpoint();
				}
			}
			else {
				Logger::ins.Error() << reply->errorString() << "\r\n";
				emit UpdateSignal(reply->errorString());
			}

			if (SelectNextEndpoint()) {
				Logger::ins.Info() << "Parsing " << QS(SelectedEndpoint().urlRegistryFolder) << "index.xml...\r\n";
				http.Get(QS(SelectedEndpoint().urlRegistryFolder) + "index.xml");
			}
			else if (SelectFirstEndpoint()) {
				Logger::ins.Info() << "Parsing WSDL files...\r\n";
				emit UpdateSignal("Parsing WSDL files...");
				op = lsoGetWsdl;
				Logger::ins.Info() << "Parsing " << QS(SelectedEndpoint().urlRegistryFolder + SelectedEndpoint().urlWSDL) << "\r\n";
				http.Get(QS(SelectedEndpoint().urlRegistryFolder + SelectedEndpoint().urlWSDL));
			}

			break;

		case lsoGetWsdl:

			if (reply->error() == QNetworkReply::NoError) {

				QString replyString(reply->readAll());
				sl::log << "lsoGetWsdl: " << SQ(replyString) << sl::end;

				QDomDocument doc;
				doc.setContent(replyString);

				for (QDomElement r = doc.firstChildElement(); !r.isNull(); r = r.nextSiblingElement()) {
					if (r.nodeName() == "definitions" || r.nodeName() == "wsdl:definitions") {

						QDomAttr n = r.attributeNode("targetNamespace");
						if (!n.isNull())
							SelectedEndpoint().targetNamespace = SQ(n.nodeValue());

						n = r.attributeNode("name");
						if (!n.isNull())
							SelectedEndpoint().definitionName = SQ(n.nodeValue());

						// find all service names

						std::vector<std::string> serviceNames;
						for (QDomElement e = r.firstChildElement(); !e.isNull(); e = e.nextSiblingElement()) {

							if (e.nodeName() == "service" || e.nodeName() == "wsdl:service") {
								QDomAttr a = e.attributeNode("name");
								if (!a.isNull())
									serviceNames.push_back(SQ(a.nodeValue()));
							}
						}

						// assign service name

						if (serviceNames.size() == 1)
							SelectedEndpoint().serviceName = serviceNames[0];
						else if (serviceNames.size() > 1) {

							// search service node name with best fit to protected endpoint string

							std::map<int, std::string> m;
							for (std::vector<std::string>::iterator s = serviceNames.begin(); s != serviceNames.end(); ++s)
								m.insert(std::pair<int, std::string>(LevenshteinDistance(SelectedEndpoint().protectedEndpoint, *s, true), *s));

							SelectedEndpoint().serviceName = m.begin()->second;
						}
						else {
							Logger::ins.Warning() << "Service name not found for endpoint '" << QS(SelectedEndpoint().intGivenName) + "'\r\n";
							emit UpdateSignal("Service name not found for endpoint '" + QS(SelectedEndpoint().intGivenName) + "'");
							SelectedEndpoint().serviceName = SelectedEndpoint().intGivenName;
						}

						break;
					}
				}
			}
			else {
				Logger::ins.Error() << reply->errorString() << "\r\n";
				emit UpdateSignal(reply->errorString());
			}

			if (SelectNextEndpoint()) {
				Logger::ins.Info() << "Parsing " << QS(SelectedEndpoint().urlRegistryFolder + SelectedEndpoint().urlWSDL) << "\r\n";
				http.Get(QS(SelectedEndpoint().urlRegistryFolder + SelectedEndpoint().urlWSDL));
			}
			else {
				Logger::ins.Info() << "Finished loading registry.\r\n";
				emit DoneSignal("Finished loading.");
			}

			break;

		default:;
	}
}

void Layer7::ErrorSlot(const QString &text) {
	emit UpdateSignal(text);
	emit DoneSignal("Stopped.");
}
