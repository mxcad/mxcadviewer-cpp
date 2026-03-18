/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "KeyCaptureDelegate.h"
#include <QKeySequence>

ShortcutLineEdit::ShortcutLineEdit(QWidget* parent) : QLineEdit(parent)
{
    setReadOnly(true);
    setAlignment(Qt::AlignCenter);
}

void ShortcutLineEdit::keyPressEvent(QKeyEvent* event)
{
    int key = event->key();
    if (key == Qt::Key_unknown || key == Qt::Key_Control || key == Qt::Key_Shift ||
        key == Qt::Key_Alt || key == Qt::Key_Meta) {
        return;
    }

    if (key == Qt::Key_Backspace || key == Qt::Key_Delete) {
        clear();
        return;
    }

    Qt::KeyboardModifiers mods = event->modifiers();

    QKeySequence seq(mods | key);
    setText(seq.toString(QKeySequence::NativeText));
}

KeyCaptureDelegate::KeyCaptureDelegate(QObject* parent) : QStyledItemDelegate(parent) {}

QWidget* KeyCaptureDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&/*option*/, const QModelIndex&/*index*/) const
{
    return new ShortcutLineEdit(parent);
}

void KeyCaptureDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    ShortcutLineEdit* lineEdit = qobject_cast<ShortcutLineEdit*>(editor);
    if (lineEdit) {
        lineEdit->setText(index.data(Qt::EditRole).toString());
    }
}

void KeyCaptureDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    ShortcutLineEdit* lineEdit = qobject_cast<ShortcutLineEdit*>(editor);
    if (lineEdit) {
        model->setData(index, lineEdit->text(), Qt::EditRole);
    }
}