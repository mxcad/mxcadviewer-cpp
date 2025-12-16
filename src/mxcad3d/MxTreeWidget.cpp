/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "MxTreeWidget.h"
#include <QMouseEvent>

MxTreeWidget::MxTreeWidget(QWidget* parent)
	: QTreeWidget(parent)
{
	setSelectionMode(QAbstractItemView::ExtendedSelection);
}

MxTreeWidget::~MxTreeWidget()
{
}

void MxTreeWidget::mousePressEvent(QMouseEvent* event)
{
	QTreeWidgetItem* item = itemAt(event->pos());
	if (!item)
	{
		clearSelection();
		setCurrentItem(nullptr);
	}
	QTreeWidget::mousePressEvent(event);
}
