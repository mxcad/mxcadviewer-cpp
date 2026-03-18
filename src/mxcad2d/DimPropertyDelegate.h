/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/
#pragma once
#include <QStyledItemDelegate>

#include "DimPropertyData.h"
// -------------------------------------------------------------------------
// Custom Delegate for the Property Table
// Handles painting color blocks and providing specific editors
// -------------------------------------------------------------------------
class DimPropertyDelegate : public QStyledItemDelegate {
public:
    explicit DimPropertyDelegate(const DimPropertyData& initialData, QObject* parent = nullptr);

    // 1. Handle Display (Painting)
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    // 2. Provide the correct Editor when a cell is double-clicked / clicked
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    // 3. Pass data from the Model to the Editor
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;

    // 4. Pass data from the Editor back to the Model
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

private:
    DimPropertyData m_data;
};

