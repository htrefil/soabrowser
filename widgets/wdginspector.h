#ifndef wdginspector_h
#define wdginspector_h

#include "interfaces/iview.h"
#include <QItemDelegate>
#include <QTextBrowser>
#include <QTreeWidget>
#include <QSplitter>

#define SKIP_COLUMNS	2



class SoaWg;
class SoaRw;
class WdgInspector;

class WdgInspectorDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	WdgInspectorDelegate(WdgInspector *, SoaWg *);

	void SetRow(SoaRw *);

	QWidget *createEditor(QWidget *, const QStyleOptionViewItem &, const QModelIndex &) const;
	void setEditorData(QWidget *, const QModelIndex &) const;
	void updateEditorGeometry(QWidget *, const QStyleOptionViewItem &, const QModelIndex &) const;
	void setModelData(QWidget *, QAbstractItemModel *, const QModelIndex &) const;

private:
	WdgInspector *ins;
	SoaWg *wg;
	SoaRw *row;

private slots:
	void AddNewCBItemSlot();

signals:
	void modifiedSignal() const;
};



class WdgInspector : public QTreeWidget
{
	Q_OBJECT

public:
	WdgInspector(QWidget *, SoaWg *);

	const WdgInspectorDelegate *Delegate() const;
	void SetRow(SoaRw *);

private:
	SoaRw *row;
	WdgInspectorDelegate *delegate;
};



class IViewable;

class WdgInspectorContainer : public IView
{
	Q_OBJECT

public:
	WdgInspectorContainer(SoaWg *, IViewable *);

	QString Refresh();
	int IsSingleton() { return ivsNone; }

private:
	IViewable *iv;
	WdgInspector *inspector;
	QTextBrowser *browser;
	QSplitter *splitter;
};

#endif
