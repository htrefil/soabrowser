#ifndef iview_h
#define iview_h

#include <QWidget>
#include <list>

#define IV_NULL_DOMAIN	100000000
#define IV_NULL_VIEWID	100000000



enum IViewSingleton { ivsNone = 0, ivsGateway };

class IView : public QWidget
{
	Q_OBJECT

public:
	IView(QWidget *wg) : QWidget(wg) {}

	virtual QString Refresh() = 0;
	virtual int IsSingleton() = 0;
};

typedef std::list<IView *> IViewList;

#endif
