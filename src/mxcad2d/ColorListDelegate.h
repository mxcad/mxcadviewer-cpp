/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once
#include <QStyledItemDelegate>
// -------------------------------------------------------------------------
// Custom Delegate for the ComboBox Drop-down List
// Handles drawing the long color strips inside the drop-down menu
// -------------------------------------------------------------------------
class ColorListDelegate : public QStyledItemDelegate {
public:
	explicit ColorListDelegate(QObject* parent = nullptr);

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};