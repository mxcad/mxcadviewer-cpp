/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dShortcutSettingsDialog.h"

#include <QtWidgets>
#include <QKeyEvent>
#include <QHeaderView>

Mx2dShortcutSettingsDialog::Mx2dShortcutSettingsDialog(QWidget* parent)
	: QDialog(parent)
{
	Qt::WindowFlags flags = Qt::Dialog;
	flags |= Qt::CustomizeWindowHint;
	flags |= Qt::WindowTitleHint;
	flags |= Qt::WindowCloseButtonHint;
	setWindowFlags(flags);

	setWindowTitle(tr("Shortcut Settings"));
	setupUi();
	populateTables();
	setupConnections();

	setFixedSize(600, 850);
}

Mx2dShortcutSettingsDialog::~Mx2dShortcutSettingsDialog()
{
}

void Mx2dShortcutSettingsDialog::setupUi()
{
	QVBoxLayout* pMainLayout = new QVBoxLayout(this);
	pMainLayout->setSpacing(10);
	pMainLayout->setContentsMargins(15, 15, 15, 15);

	m_pInfoLabel1 = new QLabel(tr("You can click a row in the table to set a shortcut for the function"));
	m_pInfoLabel2 = new QLabel(tr("(Shortcut setting method: Press keys directly on the keyboard)"));
	QFont boldFont = m_pInfoLabel2->font();
	boldFont.setBold(true);
	boldFont.setPointSize(boldFont.pointSize() + 1);
	m_pInfoLabel2->setFont(boldFont);

	pMainLayout->addWidget(m_pInfoLabel1);
	pMainLayout->addWidget(m_pInfoLabel2, 0, Qt::AlignHCenter);
	pMainLayout->addSpacing(5);

	m_pEditableTable = new QTableWidget();
	m_pEditableTable->setColumnCount(2);
	m_pEditableTable->setHorizontalHeaderLabels({ tr("Function Name"), tr("Shortcut") });
	m_pEditableTable->verticalHeader()->hide();
	m_pEditableTable->horizontalHeader()->setStretchLastSection(true);
	m_pEditableTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pEditableTable->setSelectionMode(QAbstractItemView::SingleSelection);

	m_pEditableTable->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::AnyKeyPressed);

	m_pEditableTable->setItemDelegateForColumn(1, new ShortcutDelegate(m_pEditableTable));

	pMainLayout->addWidget(m_pEditableTable);

	m_pFixedLabel = new QLabel(tr("The following are some built-in software shortcuts (cannot be modified):"));
	pMainLayout->addWidget(m_pFixedLabel);

	m_pFixedTable = new QTableWidget();
	m_pFixedTable->setColumnCount(2);
	m_pFixedTable->setHorizontalHeaderLabels({ tr("Function Name"), tr("Shortcut") });
	m_pFixedTable->verticalHeader()->hide();
	m_pFixedTable->horizontalHeader()->setStretchLastSection(true);
	m_pFixedTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_pFixedTable->setFocusPolicy(Qt::NoFocus);
	m_pFixedTable->setSelectionMode(QAbstractItemView::NoSelection);

	pMainLayout->addWidget(m_pFixedTable);

	m_pSaveButton = new QPushButton(tr("Save"));
	m_pSaveButton->setFixedSize(80, 28);

	QHBoxLayout* pButtonLayout = new QHBoxLayout();
	pButtonLayout->addStretch();
	pButtonLayout->addWidget(m_pSaveButton);

	pMainLayout->addLayout(pButtonLayout);
}

void Mx2dShortcutSettingsDialog::populateTables()
{
	QList<QPair<QString, QString>> editableShortcuts = {
		{tr("Draw Line"), ""},
		{tr("Continuous Measurement"), "L"},
		{tr("Batch Measurement"), ""},
		{tr("Arc Length"), ""},
		{tr("Distance from Point to Line"), ""},
		{tr("Area (with arcs)"), ""},
		{tr("Measure Circle"), ""},
		{tr("Measure Angle"), ""},
		{tr("Measurement Statistics"), ""},
		{tr("Measure Hatch Area"), ""},
		{tr("Modify Single Annotation Properties"), ""},
		{tr("Extract Text"), ""}
	};

	m_pEditableTable->setRowCount(editableShortcuts.size());
	for (int i = 0; i < editableShortcuts.size(); ++i) {
		// Function name column (non-editable)
		QTableWidgetItem* pFuncNameItem = new QTableWidgetItem(editableShortcuts[i].first);
		pFuncNameItem->setFlags(pFuncNameItem->flags() & ~Qt::ItemIsEditable); // Remove editable flag
		m_pEditableTable->setItem(i, 0, pFuncNameItem);

		// Shortcut column (keep default editable flag)
		QTableWidgetItem* pShortcutItem = new QTableWidgetItem(editableShortcuts[i].second);
		pShortcutItem->setTextAlignment(Qt::AlignCenter);
		m_pEditableTable->setItem(i, 1, pShortcutItem);
	}
	m_pEditableTable->resizeColumnToContents(0);

	QList<QPair<QString, QString>> fixedShortcuts = {
		{tr("Open"), "Ctrl+O"},
		{tr("Undo"), "Ctrl+Z"},
		{tr("Redo"), "Ctrl+Y"},
		{tr("Find Text"), "Ctrl+F"},
		{tr("Print"), "Ctrl+P"},
		{tr("Exit Full Screen"), "Esc"}
	};

	m_pFixedTable->setRowCount(fixedShortcuts.size());
	for (int i = 0; i < fixedShortcuts.size(); ++i) {
		QTableWidgetItem* pFuncNameItem = new QTableWidgetItem(fixedShortcuts[i].first);
		QTableWidgetItem* pShortcutItem = new QTableWidgetItem(fixedShortcuts[i].second);
		pShortcutItem->setTextAlignment(Qt::AlignCenter);

		pFuncNameItem->setFlags(pFuncNameItem->flags() & ~Qt::ItemIsEditable);
		pShortcutItem->setFlags(pShortcutItem->flags() & ~Qt::ItemIsEditable);

		m_pFixedTable->setItem(i, 0, pFuncNameItem);
		m_pFixedTable->setItem(i, 1, pShortcutItem);
	}
	m_pFixedTable->resizeColumnToContents(0);
}

void Mx2dShortcutSettingsDialog::setupConnections()
{
	connect(m_pSaveButton, &QPushButton::clicked, this, &QDialog::accept);
}