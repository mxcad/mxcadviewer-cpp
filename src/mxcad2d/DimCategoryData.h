/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once
#include <QColor>
#include <QString>

// Struct to hold category data loaded from config
struct DimCategoryData {
	QString name;
	QColor color;
	bool isVisible;
	bool isCurrent;
};