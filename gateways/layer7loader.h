#ifndef layer7_h
#define layer7_h

#include "gateways/gateway.h"
#include <QStringList>
#include <QObject>
#include <vector>



class QNetworkReply;

class Layer7 : public SoaGtwy
{
	Q_OBJECT

	enum LayerSevenOp { lsoStop = 0, lsoStart, lsoListDomains, lsoListServices, lsoGetIndexXml, lsoGetWsdl };

public:
	Layer7();

	void Load(unsigned int, const QString &, const QString &, const QString &, const QString &, const QString &, const QStringList &);
	void Cancel();

private:
	QStringList sdoms;

private slots:
	void LoadSlot(QNetworkReply *reply);
	void ErrorSlot(const QString &);
};

#endif
