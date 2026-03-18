/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "DimCategoryDelegate.h"
#include "Mx2dDimCategoryManagerDialog.h"
#include <QPainter>

DimCategoryDelegate::DimCategoryDelegate(QObject* parent)
	: QStyledItemDelegate(parent) 
{
}

void DimCategoryDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	int col = index.column();

	if (col == static_cast<int>(Mx2dDimCategoryManagerDialog::TableColumn::Color)) {
		// Draw background (selection state, etc.)
		QStyleOptionViewItem opt = option;
		initStyleOption(&opt, index);
		opt.text = QString(); // Clear text
		option.widget->style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter, option.widget);

		// Draw color block
		QColor color = index.data(Qt::UserRole).value<QColor>();
		if (color.isValid()) {
			QRect rect = option.rect;
			rect.adjust(4, 4, -4, -4); // Margins: 4px from each cell border
			painter->save();
			painter->setRenderHint(QPainter::Antialiasing);
			painter->setBrush(color);
			painter->setPen(QPen(Qt::gray, 1)); // 1px gray border around color
			painter->drawRect(rect);
			painter->restore();
		}
	}
	else if (col == static_cast<int>(Mx2dDimCategoryManagerDialog::TableColumn::Show)
		|| col == static_cast<int>(Mx2dDimCategoryManagerDialog::TableColumn::Current)
		|| col == static_cast<int>(Mx2dDimCategoryManagerDialog::TableColumn::Delete)) {
		// Draw background (selection state, etc.)
		QStyleOptionViewItem opt = option;
		initStyleOption(&opt, index);

		// Clear icon and text from the default option so it doesn't draw left-aligned
		opt.icon = QIcon();
		opt.text = QString();
		opt.features &= ~QStyleOptionViewItem::HasDecoration;
		option.widget->style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter, option.widget);

		// Draw the actual icon perfectly centered
		QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
		if (!icon.isNull()) {
			QSize iconSize(24, 24); // Force 24x24 size for standard scaling
			QRect rect = option.rect;
			int x = rect.left() + (rect.width() - iconSize.width()) / 2;
			int y = rect.top() + (rect.height() - iconSize.height()) / 2;
			QRect iconRect(x, y, iconSize.width(), iconSize.height());
			icon.paint(painter, iconRect, Qt::AlignCenter);
		}
	}
	else {
		// Column 0 (Name): Fallback to standard drawing
		QStyledItemDelegate::paint(painter, option, index);
	}
}
