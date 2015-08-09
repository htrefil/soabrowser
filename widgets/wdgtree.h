#ifndef wdgtree_h
#define wdgtree_h

#include "interfaces/iview.h"
#include "interfaces/iviewable.h"
#include "database/soarow.h"
#include <QTreeWidget>



class WdgTreeItem : public QTreeWidgetItem
{
public:
    unsigned ts, id;
    bool en;
    int sortBy;

    WdgTreeItem(QTreeWidgetItem *_parent, const QStringList &strings, int _sortBy) :
        QTreeWidgetItem(_parent, strings),
        sortBy(_sortBy) {}

    bool operator<(const QTreeWidgetItem &other) const {
        if (((WdgTreeItem &)other).sortBy == 0)
            return false;
        else
            return QTreeWidgetItem::operator<(other);
    }
};



class SoaWg;

class WdgTree : public QTreeWidget
{
	Q_OBJECT

public:
	WdgTree(SoaWg *, IViewable *);

	void Refresh();
	QWidget *GetWidget();

private:
	SoaWg *wg;
	IViewable *iv;
	QString menuItemArg;
	bool selectionGuard;
	unsigned int menuItemId, inspecteeId, travel, assetId;

    bool RefreshItems(QTreeWidgetItem *, std::map<unsigned int, ItItem> &);
    void SetIcon(WdgTreeItem *, int);

	void mousePressEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent *);

private slots:
    void ItemSelectionChangedSlot();
	void CustomContextMenuRequestedSlot(const QPoint &);
	void ShowTableSlot();
    void ExpandSlot();
};



class WdgTreeContainer : public IView
{
	Q_OBJECT

public:
	WdgTreeContainer(SoaWg *, IViewable *);

	QString Refresh();
	int IsSingleton() { return ivsNone; }

private:
	WdgTree *tree;
};

#endif
