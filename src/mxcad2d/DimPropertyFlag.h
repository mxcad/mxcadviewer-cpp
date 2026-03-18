/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once
#include <QFlags>

// Enum to define which properties to show
enum DimPropertyFlag {
    Prop_None         = 0,
    Prop_Color        = 1 << 0,
    Prop_Category     = 1 << 1,
    Prop_TextHeight   = 1 << 2,
    Prop_Ratio        = 1 << 3,
    Prop_TextPosition = 1 << 4,
    Prop_Content      = 1 << 5,
    Prop_All          = Prop_Color | Prop_Category | Prop_TextHeight | Prop_Ratio | Prop_TextPosition | Prop_Content
};
Q_DECLARE_FLAGS(DimPropertyFlags, DimPropertyFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(DimPropertyFlags)