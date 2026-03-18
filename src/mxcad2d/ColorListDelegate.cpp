/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "ColorListDelegate.h"
#include <QPainter>

ColorListDelegate::ColorListDelegate(QObject* parent)
	: QStyledItemDelegate(parent)
{
}

void ColorListDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	// Draw normal background (selection highlight, etc.)
	QStyleOptionViewItem opt = option;
	initStyleOption(&opt, index);
	opt.text = QString(); // Clear text so default painter doesn't draw it
	option.widget->style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter, option.widget);

	QString text = index.data(Qt::UserRole).toString();

	if (text == tr("MORE")) {
		// Draw "More Colors..." text
		painter->drawText(option.rect, Qt::AlignCenter, tr("More Colors..."));
	}
	else {
		// Draw stretched color block for colors
		QColor color = index.data(Qt::UserRole).value<QColor>();
		if (color.isValid()) {
			QRect rect = option.rect.adjusted(4, 2, -4, -2); // Margins
			painter->save();
			painter->setRenderHint(QPainter::Antialiasing);
			painter->setBrush(color);
			painter->setPen(QPen(Qt::black, 1));
			painter->drawRect(rect);
			painter->restore();
		}
	}
}

QSize ColorListDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	return { option.rect.width(), 36 }; // Set a fixed height for drop-down items
}
