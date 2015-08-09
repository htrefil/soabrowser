#ifndef gateway_h
#define gateway_h

#include "gateways/entities.h"
#include "gateways/soahttp.h"
#include "qs.h"
#include <QObject>



class SoaGtwy : public QObject
{
	Q_OBJECT

public:
	SoaGtwy();

	bool Loaded() const;
	GtDomains ConstructTree();

protected:
	unsigned int registryId;
	QString registryName;
	int op;
	unsigned int domain, service, item;
	bool loaded;
	SoaHttp http;
	GtDomains domains;
	GtwyEndpoints endpoints;

	void Reset();

	bool SelectFirstDomain();
	bool SelectNextDomain();
	GtDomain &SelectedDomain();
	bool SelectFirstEndpoint();
	bool SelectNextEndpoint();
	GtEndpoint &SelectedEndpoint();
	void RemoveSelectedEndpoint();

signals:
	void UpdateSignal(const QString &);
	void DoneSignal(const QString &);
};

#endif
