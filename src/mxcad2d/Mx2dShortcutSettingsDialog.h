/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QDialog>
#include <QStyledItemDelegate>
#include <QLineEdit>
#include <QKeyEvent>
class QLabel;
class QTableWidget;
class QPushButton;
class QEvent;

// Line edit that can listen to keyboard events
class ShortcutLineEdit : public QLineEdit
{
	Q_OBJECT
public:
	explicit ShortcutLineEdit(QWidget* parent = nullptr) : QLineEdit(parent) {}

protected:
	// Override key press event
	void keyPressEvent(QKeyEvent* event) override
	{
		event->accept(); // Accept event to prevent further processing

		int key = event->key();

		// Ignore single modifier key presses (Ctrl, Shift, Alt), allow user to hold them first
		if (key == Qt::Key_Control || key == Qt::Key_Shift || key == Qt::Key_Alt || key == Qt::Key_Meta) {
			return; // Do nothing, wait for next non-modifier key
		}

		// Use QKeySequence to convert key event to standard string (e.g., "Ctrl+S")
		QKeySequence keySequence(event->modifiers() | key);
		QString shortcutString = keySequence.toString(QKeySequence::NativeText);

		// If Delete or Backspace is pressed, clear the shortcut
		if (key == Qt::Key_Delete || key == Qt::Key_Backspace) {
			shortcutString = "";
		}

		// Update LineEdit's own text
		setText(shortcutString);

		// Emit editingFinished signal to notify delegate that data is ready and editor can be closed
		// emit editingFinished(); // This doesn't work, probably related to event loop. Need to think about it later.
		clearFocus(); // Lose focus to close editor
	}
};

// Delegate for writing data from editor to model and rendering model data as ShortcutLineEdit
class ShortcutDelegate : public QStyledItemDelegate
{
public:
	explicit ShortcutDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}

	// Create our custom editor when editing starts
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override
	{
		Q_UNUSED(option);
		Q_UNUSED(index);
		return new ShortcutLineEdit(parent);
	}

	// Set model data to editor (display current value when editing starts)
	void setEditorData(QWidget* editor, const QModelIndex& index) const override
	{
		QString value = index.model()->data(index, Qt::EditRole).toString();
		ShortcutLineEdit* lineEdit = static_cast<ShortcutLineEdit*>(editor);
		lineEdit->setText(value);
	}

	// Write editor data back to model (update table cell value after editing)
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override
	{
		ShortcutLineEdit* lineEdit = static_cast<ShortcutLineEdit*>(editor);
		QString value = lineEdit->text();
		model->setData(index, value, Qt::EditRole);
	}
};

class Mx2dShortcutSettingsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Mx2dShortcutSettingsDialog(QWidget* parent = nullptr);
	~Mx2dShortcutSettingsDialog();

private:
	void setupUi();
	void populateTables();
	void setupConnections();

	QLabel* m_pInfoLabel1;
	QLabel* m_pInfoLabel2;
	QTableWidget* m_pEditableTable; // Editable shortcuts table
	QLabel* m_pFixedLabel;
	QTableWidget* m_pFixedTable;    // Non-editable shortcuts table
	QPushButton* m_pSaveButton;
};