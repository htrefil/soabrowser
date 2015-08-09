#include "widgets/wdgtab.h"
#include "soawidget.h"
#include <QVBoxLayout>
#include <QStackedWidget>



WdgTab::WdgTab(QWidget *_parent, FilterMode _fm) : QTabWidget(_parent),
	fm(_fm) {
	setTabsClosable(true);

	connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(CloseTabSlot(int)));
}

bool WdgTab::ContainsSpecialTab(int s) {

	for (int i = 0; i < count(); ++i)
		if (((IView *)widget(i))->IsSingleton() == s) {
			setCurrentIndex(i);
			return true;
		}

	return false;
}

FilterMode WdgTab::Mode() const {
	return fm;
}

void WdgTab::Refresh() {
	for (int i = 0; i < count(); ++i)
		setTabText(i, ((IView *)widget(i))->Refresh());
}

//
// tabs
//

void WdgTab::AddTab(IView *view) {
	addTab(view, view->windowTitle());
	setCurrentWidget(view);
}

void WdgTab::RemoveTab(IView *view) {
	removeTab(indexOf(view));
}

//
// slots
//

void WdgTab::CloseTabSlot(int index) {
	removeTab(index);
}

//
// container
//

WdgTabContainer::WdgTabContainer(SoaWg *_wg) : IView(_wg),
	wg(_wg),
	fm(fmUndefined),
	stack(new QStackedWidget(this)) {

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(stack);

	stack->addWidget(new WdgTab(this, fmOutline));
	stack->addWidget(new WdgTab(this, fmRealization));
	stack->addWidget(new WdgTab(this, fmRuntime));
}

QString WdgTabContainer::Refresh() {

	// focus tab widget that matches mode

	if (fm != wg->Mode()) {

		fm = wg->Mode();

		for (int t = 0; t < stack->count(); ++t) {
			if (((WdgTab *)stack->widget(t))->Mode() == fm) {
				stack->setCurrentIndex(t);
				break;
			}
		}
	}

	// refresh all tab widget views

	for (int t = 0; t < stack->count(); ++t)
		((WdgTab *)stack->widget(t))->Refresh();

	return "";
}

void WdgTabContainer::AddTab(IView *view, FilterMode _fm) {

	// focus tab widget that matches mode

	if (fm != _fm) {

		fm = _fm;

		for (int t = 0; t < stack->count(); ++t) {
			if (((WdgTab *)stack->widget(t))->Mode() == fm) {
				stack->setCurrentIndex(t);
				break;
			}
		}
	}

	// add view to current tab widget

	((WdgTab *)stack->currentWidget())->AddTab(view);
}

void WdgTabContainer::RemoveTab(IView *view) {
	for (int t = 0; t < stack->count(); ++t)
		((WdgTab *)stack->widget(t))->RemoveTab(view);
}

bool WdgTabContainer::ContainsSpecialTab(int s) {
	return (fm < 3) ? ((WdgTab *)stack->widget(fm))->ContainsSpecialTab(s) : false;
}
