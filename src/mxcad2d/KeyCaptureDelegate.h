/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QStyledItemDelegate>
#include <QLineEdit>
#include <QKeyEvent>

class ShortcutLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit ShortcutLineEdit(QWidget* parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent* event) override;
};


class KeyCaptureDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit KeyCaptureDelegate(QObject* parent = nullptr);

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
};
