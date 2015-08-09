#ifndef soahttp_h
#define soahttp_h

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QBuffer>
#include <QSslError>
#include <vector>



struct PropfindElement
{
	QString href, name;

	PropfindElement(const QString &_href) : href(_href) {}
};

typedef std::vector<PropfindElement> Propfinds;



class SoaHttp : public QNetworkAccessManager
{
	Q_OBJECT

public:
	SoaHttp();

	const QString &Url() const;
	void SetUrl(const QString &);
	QString FixUrl(const QString &);
	void SetAuthentication(const QString &, const QString &, const QString &);

	void Get(const QString &);
	void Propfind(const QString &);

	void Abort();

	Propfinds ParsePropfindReply(const QString &);

private:
	QByteArray body;
	QBuffer buffer;
	QString url, usn, pwd, dom, hostname;
	QUrl uurl;
	QNetworkReply *reply;

private slots:
	void ErrorSlot(QNetworkReply::NetworkError);
	void SslErrorSlot(QNetworkReply *, QList<QSslError>);

signals:
	void ErrorSignal(const QString &);
};

#endif
