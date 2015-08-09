#ifndef wdgtab_h
#define wdgtab_h

#include "interfaces/iview.h"
#include "interfaces/iviewable.h"
#include <QTabWidget>



class SoaWg;

class WdgTab : public QTabWidget
{
	Q_OBJECT

public:
	WdgTab(QWidget *, FilterMode);

	bool ContainsSpecialTab(int);
	FilterMode Mode() const;

	void Refresh();
	void AddTab(IView *);
	void RemoveTab(IView *);

private:
	FilterMode fm;

private slots:
	void CloseTabSlot(int);
};



class QStackedWidget;

class WdgTabContainer : public IView
{
public:
	WdgTabContainer(SoaWg *);

	QString Refresh();
	int IsSingleton() { return ivsNone; }

	void AddTab(IView *, FilterMode);
	void RemoveTab(IView *);
	bool ContainsSpecialTab(int);

private:
	SoaWg *wg;
	FilterMode fm;
	QStackedWidget *stack;
};

#endif
