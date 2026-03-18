/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dShortcutSettingsDialog.h"
#include "KeyCaptureDelegate.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QHeaderView>
#include <QMouseEvent>
#include <QMessageBox>
#include <QFont>
#include <QDebug>

Mx2dShortcutSettingsDialog::Mx2dShortcutSettingsDialog(const QList<ShortcutData>& shortcutList, QWidget* parent)
	: QDialog(parent), m_shortcutDataList(shortcutList)
{
	Qt::WindowFlags flags = Qt::Dialog;
	flags |= Qt::CustomizeWindowHint;
	flags |= Qt::WindowTitleHint;
	flags |= Qt::WindowCloseButtonHint;
	setWindowFlags(flags);

	setupUI();
	setWindowTitle(tr("Shortcut Settings"));
	resize(600, 940);
}

void Mx2dShortcutSettingsDialog::setupUI()
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->setSpacing(10);

	QLabel* lblTip1 = new QLabel(tr("You can click a row in the table to set a shortcut for the function"), this);
	mainLayout->addWidget(lblTip1);

	QLabel* lblTip2 = new QLabel(tr("(Shortcut setting method: Press keys directly on the keyboard)"), this);
	QFont boldFont;
	boldFont.setBold(true);
	boldFont.setPointSize(11);
	lblTip2->setFont(boldFont);
	lblTip2->setAlignment(Qt::AlignCenter);
	mainLayout->addWidget(lblTip2);


	m_customTable = new QTableView(this);
	m_customModel = new QStandardItemModel(m_shortcutDataList.size(), 2, this);
	m_customModel->setHorizontalHeaderLabels({ tr("Function Name"), tr("Shortcut") });

	int row = 0;
	for (const ShortcutData& data : m_shortcutDataList) {
		QStandardItem* nameItem = new QStandardItem(data.displayName);
		nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
		nameItem->setTextAlignment(Qt::AlignCenter);

		nameItem->setData(data.id, Qt::UserRole);

		QStandardItem* keyItem = new QStandardItem(data.shortcut);
		keyItem->setTextAlignment(Qt::AlignCenter);

		m_customModel->setItem(row, 0, nameItem);
		m_customModel->setItem(row, 1, keyItem);
		row++;
	}

	m_customTable->setModel(m_customModel);
	m_customTable->horizontalHeader()->setStretchLastSection(false);
	m_customTable->setColumnWidth(0, 300);
	m_customTable->setColumnWidth(1, 200);
	m_customTable->setSelectionBehavior(QAbstractItemView::SelectRows);

	m_customTable->verticalHeader()->setVisible(false);
	//m_customTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	m_customTable->setItemDelegateForColumn(1, new KeyCaptureDelegate(this));
	m_customTable->viewport()->installEventFilter(this);

	connect(m_customTable, &QTableView::clicked, this, [this](const QModelIndex& index) {
		if (index.isValid()) {
			QModelIndex targetIndex = m_customModel->index(index.row(), 1);
			m_oldShortcut = m_customModel->data(targetIndex).toString();

			m_customTable->edit(targetIndex);
		}
		});
	connect(m_customModel, &QStandardItemModel::dataChanged, this, &Mx2dShortcutSettingsDialog::onCustomDataChanged);

	mainLayout->addWidget(m_customTable, 1);

	QLabel* lblTip3 = new QLabel(tr("The following are some built-in software shortcuts (cannot be modified):"), this);
	lblTip3->setContentsMargins(0, 10, 0, 0);
	mainLayout->addWidget(lblTip3);

	struct FixedShortcut { QString name; QString key; };
	QList<FixedShortcut> fixedList = {
		{tr("Open"), "Ctrl+O"}, {tr("Undo"), "Ctrl+Z"}, {tr("Redo"), "Ctrl+Y"},
		{tr("Text Search"), "Ctrl+F"}
	};
	m_fixedTable = new QTableView(this);
	m_fixedModel = new QStandardItemModel(fixedList.size(), 2, this);
	m_fixedModel->setHorizontalHeaderLabels({ tr("Function Name"), tr("Shortcut") });

	
	

	for (int i = 0; i < fixedList.size(); ++i) {
		QStandardItem* nameItem = new QStandardItem(fixedList[i].name);
		nameItem->setTextAlignment(Qt::AlignCenter);
		QStandardItem* keyItem = new QStandardItem(fixedList[i].key);
		keyItem->setTextAlignment(Qt::AlignCenter);

		m_fixedModel->setItem(i, 0, nameItem);
		m_fixedModel->setItem(i, 1, keyItem);
	}

	m_fixedTable->setModel(m_fixedModel);
	m_fixedTable->horizontalHeader()->setStretchLastSection(false);
	m_fixedTable->setColumnWidth(0, 200);
	m_fixedTable->setColumnWidth(1, 200);
	m_fixedTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_fixedTable->setSelectionBehavior(QAbstractItemView::SelectRows);

	m_fixedTable->verticalHeader()->setVisible(false);
	//m_fixedTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	m_fixedTable->setFixedHeight(220);

	mainLayout->addWidget(m_fixedTable, 0);

	QHBoxLayout* btnLayout = new QHBoxLayout();
	btnLayout->addStretch();
	QPushButton* btnSave = new QPushButton(tr("Save"), this);
	btnSave->setMinimumWidth(80);
	btnSave->setMinimumHeight(30);
	btnLayout->addWidget(btnSave);
	mainLayout->addLayout(btnLayout);

	connect(btnSave, &QPushButton::clicked, this, &QDialog::accept);
}

bool Mx2dShortcutSettingsDialog::checkConflict(const QString& shortcut, int currentRowToExclude)
{
	if (shortcut.isEmpty()) return false;

	for (int i = 0; i < m_fixedModel->rowCount(); ++i) {
		qDebug() << "i = " << i;
		if (m_fixedModel->item(i, 1)->text() == shortcut) return true;
	}

	for (int i = 0; i < m_customModel->rowCount(); ++i) {
		if (i == currentRowToExclude) continue;
		if (m_customModel->item(i, 1)->text() == shortcut) return true;
	}
	return false;
}

void Mx2dShortcutSettingsDialog::onCustomDataChanged(const QModelIndex& topLeft, const QModelIndex&/*bottomRight*/)
{
	if (topLeft.column() != 1) return;

	QString newShortcut = m_customModel->data(topLeft).toString();

	if (newShortcut.isEmpty() || newShortcut == m_oldShortcut) {
		return;
	}

	if (checkConflict(newShortcut, topLeft.row())) {
		QMessageBox::warning(this, tr("Prompt"), tr("This shortcut key already exists. Please reset it!"));


		m_customModel->blockSignals(true);
		m_customModel->setData(topLeft, m_oldShortcut);
		m_customModel->blockSignals(false);
	}
}

bool Mx2dShortcutSettingsDialog::eventFilter(QObject* watched, QEvent* event)
{
	if (watched == m_customTable->viewport() && (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonDblClick)) {
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
		QModelIndex index = m_customTable->indexAt(mouseEvent->pos());

		if (!index.isValid()) {
			m_customTable->clearSelection();
			m_customTable->clearFocus();
		}
	}
	return QDialog::eventFilter(watched, event);
}

QMap<QString, QString> Mx2dShortcutSettingsDialog::getSavedShortcuts() const
{
	QMap<QString, QString> result;
	for (int i = 0; i < m_customModel->rowCount(); ++i) {
		QString id = m_customModel->item(i, 0)->data(Qt::UserRole).toString();
		QString shortcut = m_customModel->item(i, 1)->text();

		result.insert(id, shortcut);
	}
	return result;
}