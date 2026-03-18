/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "DimPropertyDelegate.h"
#include "Mx2dDimPropertyDialog.h"
#include "ColorComboBox.h"
#include "Mx2dGuiDocument.h"
#include "MxUtils.h"
#include <QPainter>
#include <QLineEdit>

DimPropertyDelegate::DimPropertyDelegate(const DimPropertyData& initialData, QObject* parent)
	: QStyledItemDelegate(parent), m_data(initialData) 
{
}

void DimPropertyDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	// Retrieve the property type stored in Column 0
	int propType = index.siblingAtColumn(0).data(Mx2dDimPropertyDialog::PropertyTypeRole).toInt();

	if (index.column() == 1 && propType == Prop_Color) {
		// Draw standard background
		QStyleOptionViewItem opt = option;
		initStyleOption(&opt, index);
		opt.text = QString();
		option.widget->style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter, option.widget);

		// Draw color block
		QColor color = index.data(Qt::UserRole).value<QColor>();
		if (color.isValid()) {
			QRect rect = option.rect.adjusted(4, 4, -4, -4);
			painter->save();
			painter->setRenderHint(QPainter::Antialiasing);
			painter->setBrush(color);
			painter->setPen(QPen(Qt::black, 1)); // Black border for color property
			painter->drawRect(rect);
			painter->restore();
		}
	}
	else {
		// Default painting for text (Category, Height, Ratio)
		QStyledItemDelegate::paint(painter, option, index);
	}
}

QWidget* DimPropertyDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	int propType = index.siblingAtColumn(0).data(Mx2dDimPropertyDialog::PropertyTypeRole).toInt();

	if (index.column() == 1) {
		switch (propType) {
		case Prop_Category: {
			QComboBox* editor = new QComboBox(parent);
			editor->setStyleSheet(R"(
		QComboBox {
			border: 1px solid gray;
			padding: 1px 18px 1px 3px;
			min-width: 6em;
			combobox-popup: 0;
		}
		QComboBox::drop-down {
			subcontrol-origin: padding;
			subcontrol-position: top right;
			width: 25px;
			border-left-width: 1px;
		}
		QComboBox::down-arrow {
			image: url(:/resources/images2d/2d_dropdown.svg);
		}
	)");
			editor->addItems(m_data.availableCategories);
			return editor;
		}
		case Prop_TextHeight: {
			QLineEdit* editor = new QLineEdit(parent);
			QDoubleValidator* validator = new QDoubleValidator(0.001, 99999.0, 3, editor);
			validator->setNotation(QDoubleValidator::StandardNotation);
			editor->setValidator(validator);
			return editor;
		}
		case Prop_Ratio: {
			QComboBox* editor = new QComboBox(parent);
			editor->setStyleSheet(R"(
		QComboBox {
			border: 1px solid gray;
			padding: 1px 18px 1px 3px;
			min-width: 6em;
			combobox-popup: 0;
		}
		QComboBox::drop-down {
			subcontrol-origin: padding;
			subcontrol-position: top right;
			width: 25px;
			border-left-width: 1px;
		}
		QComboBox::down-arrow {
			image: url(:/resources/images2d/2d_dropdown.svg);
		}
	)");
			DimPropertyDelegate* delegate = const_cast<DimPropertyDelegate*>(this);
			connect(editor, QOverload<int>::of(&QComboBox::activated), this, [delegate, editor](int index) {
				if (index == 1) {
					Mx2dGuiDocument* pCadGuiDoc = qobject_cast<Mx2dGuiDocument*>(MxUtils::gCurrentTab);
					double globalRatio = pCadGuiDoc->getGlobalRatio();
					editor->setItemText(0, QString::number(globalRatio, 'f', 2));
					editor->setCurrentIndex(0);
				}

				emit delegate->commitData(editor);
				emit delegate->closeEditor(editor);
				});
			return editor;
		}
		}
	}
	return QStyledItemDelegate::createEditor(parent, option, index);
}

void DimPropertyDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	int propType = index.siblingAtColumn(0).data(Mx2dDimPropertyDialog::PropertyTypeRole).toInt();

	if (propType == Prop_Color) {
		ColorComboBox* cb = qobject_cast<ColorComboBox*>(editor);
		if (cb) cb->setCurrentColor(index.data(Qt::UserRole).value<QColor>());
	}
	else if (propType == Prop_Category || propType == Prop_Ratio) {
		QComboBox* cb = qobject_cast<QComboBox*>(editor);
		if (cb) {
			cb->clear();
			double currentValue = index.data(Qt::EditRole).toDouble();
			cb->addItem(QString::number(currentValue, 'f', 2));
			cb->addItem(tr("Current Ratio"));
			cb->setCurrentIndex(0);
		}
	}
	else if (propType == Prop_TextHeight) {
		QLineEdit* le = qobject_cast<QLineEdit*>(editor);
		if (le) le->setText(index.data(Qt::DisplayRole).toString());
	}
	else {
		QStyledItemDelegate::setEditorData(editor, index);
	}
}

void DimPropertyDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	int propType = index.siblingAtColumn(0).data(Mx2dDimPropertyDialog::PropertyTypeRole).toInt();

	if (propType == Prop_Color) {
		ColorComboBox* cb = qobject_cast<ColorComboBox*>(editor);
		if (cb) model->setData(index, cb->getCurrentColor(), Qt::UserRole); // Store as QColor
	}
	else if (propType == Prop_Category) {
		QComboBox* cb = qobject_cast<QComboBox*>(editor);
		if (cb) {
			model->setData(index, cb->currentText(), Qt::DisplayRole);
			model->setData(index, cb->currentIndex(), Qt::UserRole); // Store index for Ratio
		}
	}
	else if (propType == Prop_Ratio) {
		QComboBox* cb = qobject_cast<QComboBox*>(editor);
		if (cb) {
			double value = cb->itemText(0).toDouble();
			model->setData(index, value, Qt::EditRole);
		}
	}
	else if (propType == Prop_TextHeight) {
		QLineEdit* le = qobject_cast<QLineEdit*>(editor);
		if (le) model->setData(index, le->text(), Qt::DisplayRole);
	}
	else {
		QStyledItemDelegate::setModelData(editor, model, index);
	}
}
