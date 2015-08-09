#ifndef wdgtable_h
#define wdgtable_h

#include "interfaces/iview.h"
#include <QTableWidget>



class SoaWg;
class IViewable;

class WdgTable : public QTableWidget
{
	Q_OBJECT

public:
	WdgTable(QWidget *, SoaWg *, IViewable *, int);

	QString Refresh();

private:
	SoaWg *wg;
	IViewable *iv;
	int tbIndex;
	bool selectionGuard, itemClickedGuard, headerSet;
	std::vector<int> styles;

	QStringList ToStringList(const std::vector<std::string> &);

private slots:
	void ItemSelectionChangedSlot();
	void FormatCellsSlot();
	void FormatColumnsSlot();
	void ContextMenuRequestedSlot(const QPoint &);
};



class WdgTableContainer : public IView
{
	Q_OBJECT

public:
	WdgTableContainer(SoaWg *, IViewable *, int);

	QString Refresh();
	int IsSingleton() { return ivsNone; }

private:
	WdgTable *table;
};

#endif
