#include "widgets/wdginspector.h"
#include "widgets/assetscombo.h"
#include "soawidget.h"
#include <QSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include <QMessageBox>
#include <QDesktopServices>
#include <iostream>



WdgInspectorDelegate::WdgInspectorDelegate(WdgInspector *_ins, SoaWg *_wg) :
	QItemDelegate(_ins), ins(_ins), wg(_wg), row(0) {}

void WdgInspectorDelegate::SetRow(SoaRw *_row) {
	row = _row;
}

void WdgInspectorDelegate::AddNewCBItemSlot() {}

QWidget *WdgInspectorDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const {

	int i = index.row() + SKIP_COLUMNS;
	const SoaCl *cl = &row->Tb()->Columns()[i];

	if (index.column() == 0) {

		if (cl->Type() == sctTxt) {

			QString t = QString::fromStdString(row->Ce(cl->Index())->Txt());

			if (t.startsWith("http", Qt::CaseInsensitive))
				QDesktopServices::openUrl(QUrl(t));
			else if (t.endsWith("wsdl", Qt::CaseInsensitive) || t.endsWith("docx", Qt::CaseInsensitive) ||
					t.endsWith("doc", Qt::CaseInsensitive) || t.endsWith("xls", Qt::CaseInsensitive) ||
					t.endsWith("txt", Qt::CaseInsensitive) || t.endsWith("xlsx", Qt::CaseInsensitive) ||
					t.endsWith("pdf", Qt::CaseInsensitive) || t.endsWith("xsd", Qt::CaseInsensitive)) {

				t.replace('\\', '/');
				QDesktopServices::openUrl(QUrl::fromLocalFile(t));
			}
		}

		return 0;
	}
	else {

		if (!cl->Schema().editable)
			return 0;

		switch (cl->Type()) {
		case sctInt:	return new QSpinBox(parent);
		case sctTxt:	return new QLineEdit(parent);
		case sctRow:	return new AssetsCombo(parent, wg, cl->Fk());
		default:		return 0;
		}
	}
}

void WdgInspectorDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {

	int i = index.row() + SKIP_COLUMNS;
	const SoaCl *cl = &row->Tb()->Columns()[i];

	if (cl->Type() == sctInt)
		((QSpinBox *)editor)->setValue(row->Ce(i)->Num());
	else if (cl->Type() == sctTxt)
		((QLineEdit *)editor)->setText(QString::fromUtf8(row->Ce(i)->Txt().c_str()));
	else if (cl->Type() == sctRow)
		((AssetsCombo *)editor)->Refresh(row->Ce(i)->Row());
}

void WdgInspectorDelegate::setModelData(QWidget *editor, QAbstractItemModel *, const QModelIndex &index) const {

	int i = index.row() + SKIP_COLUMNS;
	const SoaCl *cl = &row->Tb()->Columns()[i];

	if (cl->Type() == sctInt)
		row->SetCe(i, ((QSpinBox *)editor)->value());
	else if (cl->Type() == sctTxt)
		row->SetCe(i, ((QLineEdit *)editor)->text().toStdString());
	else if (cl->Type() == sctRow) {

		// get fk id

		AssetsCombo *cb = (AssetsCombo *)editor;
		unsigned int fkId = cb->itemData(cb->currentIndex()).toUInt();

		if (fkId != IV_NULL_VIEWID) {

			// disconnect fk

			SoaRw *fk = row->Ce(i)->Row();
			if (fk) {
				row->SetCe(i, (SoaRw *)CE_DEF_ROW);
				fk->RemoveCh(row);
			}

			// connect new fk

			fk = cl->Fk()->RwByViewId(fkId);
			if (fk) {
				row->SetCe(i, fk);
				fk->AddCh(row);
			}
		}
	}

	if (wg)
		wg->RefreshViews();
	else
		ins->SetRow(row);

	emit modifiedSignal();

	std::cerr << "setting model data" << std::endl;
}

void WdgInspectorDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const {
	editor->setGeometry(option.rect);
}
