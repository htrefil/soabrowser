#include "gateways/soahttp.h"
#include <QNetworkReply>
#include <QDomDocument>
#include <QSslConfiguration>
#include <QStringList>
#include <iostream>



SoaHttp::SoaHttp() : reply(0) {}

const QString &SoaHttp::Url() const {
	return url;
}

void SoaHttp::SetUrl(const QString &_url) {
	url = _url;
//	hostname = (url.startsWith("https") ? "https://" : "http://") + QUrl(url).host() + ":" + QUrl(url).port();
	uurl = QUrl(url);
}

QString SoaHttp::FixUrl(const QString &t) {

	QUrl newUrl(t);
	newUrl.setScheme(uurl.scheme());
	newUrl.setHost(uurl.host());
	newUrl.setPort(uurl.port());

	return newUrl.toString();
}

void SoaHttp::SetAuthentication(const QString &_usn, const QString &_pwd, const QString &_dom) {
	usn = _usn;
	pwd = _pwd;
	dom = _dom;
}

void SoaHttp::Get(const QString &path) {

//	url = path.startsWith("http") ? path : (hostname + path);
	url = FixUrl(path);

	QString authorization(((dom != "") ? (dom + "\\" + usn) : usn) + ":" + pwd);

	QNetworkRequest req;
	req.setUrl(QUrl(path));
	req.setRawHeader("Authorization", ("Basic " + QByteArray(authorization.toUtf8()).toBase64()));
	if (path.startsWith("https"))
		req.setSslConfiguration(QSslConfiguration::defaultConfiguration());

	if (reply)
		delete reply;

	reply = get(req);
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(ErrorSlot(QNetworkReply::NetworkError)));
}

void SoaHttp::Propfind(const QString &path) {

//	url = path.startsWith("http") ? path : (hostname + path);
	url = FixUrl(path);

	QString authorization(((dom != "") ? (dom + "\\" + usn) : usn) + ":" + pwd);

	QNetworkRequest req;
	req.setUrl(QUrl(path));
	req.setRawHeader(QByteArray("Depth"), QString("1").toUtf8());
	req.setRawHeader("Authorization", ("Basic " + QByteArray(authorization.toUtf8()).toBase64()));
	if (path.startsWith("https"))
		req.setSslConfiguration(QSslConfiguration::defaultConfiguration());

	body = "<?xml version=\"1.0\"?><D:propfind xmlns:D=\"DAV:\"><D:prop><D:displayname/></D:prop></D:propfind>";
	buffer.setBuffer(&body);
	int size = buffer.size();

	QString host = QUrl(path).host();	// 95.97.10.160
	req.setRawHeader(QByteArray("Host"), host.toUtf8());
	req.setRawHeader(QByteArray("Connection"), QByteArray("Keep-Alive"));
	req.setHeader(QNetworkRequest::ContentLengthHeader, QVariant(size));
	req.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/xml; charset=utf-8"));

	if (reply)
		delete reply;

	reply = sendCustomRequest(req, QString("PROPFIND").toUtf8(), &buffer);
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(ErrorSlot(QNetworkReply::NetworkError)));
	connect(this, SIGNAL(sslErrors(QNetworkReply *, QList<QSslError>)), this, SLOT(SslErrorSlot(QNetworkReply *, QList<QSslError>)));
}

void SoaHttp::ErrorSlot(QNetworkReply::NetworkError) {
	emit ErrorSignal(reply->errorString());
}

void SoaHttp::SslErrorSlot(QNetworkReply *r, QList<QSslError>) {
	r->ignoreSslErrors();
}

void SoaHttp::Abort() {
	if (reply->isRunning())
		reply->abort();
}

//
// reply parsing
//

Propfinds SoaHttp::ParsePropfindReply(const QString &reply) {

	Propfinds elements;

	QDomDocument doc;
	doc.setContent(reply);

	for (QDomElement e = doc.firstChildElement(); !e.isNull(); e = e.nextSiblingElement()) {

		if (e.nodeName() == "D:multistatus") {

			for (QDomElement r = e.firstChildElement(); !r.isNull(); r = r.nextSiblingElement()) {

				for (QDomElement h = r.firstChildElement(); !h.isNull(); h = h.nextSiblingElement()) {

					if (h.nodeName() == "D:href") {

						QString href = FixUrl(h.text());
						QStringList bits = href.split('/', QString::SkipEmptyParts);
						elements.push_back(PropfindElement(href));
						elements.back().name = bits.back();
					}
				}
			}

			break;
		}
	}

	return elements;
}


