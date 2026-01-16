/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dMeasurementStatDialog.h"

#include <QGroupBox>
#include <QRadioButton>
#include <QPushButton>
#include <QCheckBox>
#include <QTabWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QDebug>
#include <QLabel>

Mx2dMeasurementStatDialog::Mx2dMeasurementStatDialog(QWidget* parent)
	: QDialog(parent)
{
	Qt::WindowFlags flags = Qt::Dialog;
	flags |= Qt::CustomizeWindowHint;
	flags |= Qt::WindowTitleHint;
	flags |= Qt::WindowCloseButtonHint;
	setWindowFlags(flags);

	setWindowTitle(tr("Measurement Statistics"));

	setupUi();
	setupConnections();
	populateTables();
	updateTotals();

	// Set initial disabled state for buttons
	onScopeChanged();
	onFilterByColorToggled(false);
	onFilterByCategoryToggled(false);
	
	adjustSize();
}

Mx2dMeasurementStatDialog::~Mx2dMeasurementStatDialog()
{
}

void Mx2dMeasurementStatDialog::setupUi()
{
	// --- 1. Statistics Scope Section ---
	m_pScopeGroupBox = new QGroupBox(tr("Statistics Scope"), this);
	m_pAllAnnotationsRadio = new QRadioButton(tr("All Annotations"), this);
	m_pManualSelectRadio = new QRadioButton(tr("Manually Select Annotations"), this);
	m_pSelectButton = new QPushButton(tr("Select Annotations"), this);
	m_pClearButton = new QPushButton(tr("Clear Selection"), this);
	m_pAllAnnotationsRadio->setChecked(true);

	QGridLayout* scopeLayout = new QGridLayout;
	scopeLayout->addWidget(m_pAllAnnotationsRadio, 0, 0);
	scopeLayout->addWidget(m_pManualSelectRadio, 1, 0);
	scopeLayout->addWidget(m_pSelectButton, 1, 1);
	scopeLayout->addWidget(m_pClearButton, 1, 2);
	scopeLayout->setColumnStretch(3, 1);
	m_pScopeGroupBox->setLayout(scopeLayout);

	// --- 2. Filter Section ---
	m_pFilterByColorCheck = new QCheckBox(tr("Filter by Annotation Color"), this);
	m_pOpenColorFilterButton = new QPushButton(tr("Open Color Selector"), this);
	m_pFilterByCategoryCheck = new QCheckBox(tr("Filter by Annotation Category"), this);
	m_pOpenCategoryFilterButton = new QPushButton(tr("Open Category Selector"), this);

	QGridLayout* filterLayout = new QGridLayout;
	filterLayout->addWidget(m_pFilterByColorCheck, 0, 0);
	filterLayout->addWidget(m_pOpenColorFilterButton, 0, 1);
	filterLayout->addWidget(m_pFilterByCategoryCheck, 1, 0);
	filterLayout->addWidget(m_pOpenCategoryFilterButton, 1, 1);
	filterLayout->setColumnStretch(2, 1);

	// --- 3. Tab and Table Section ---
	m_pTabWidget = new QTabWidget(this);
	m_pLengthTable = new QTableWidget(this);
	m_pAreaTable = new QTableWidget(this);
	m_pTabWidget->addTab(m_pLengthTable, tr("Length"));
	m_pTabWidget->addTab(m_pAreaTable, tr("Area"));

	// Configure length table
	m_pLengthTable->setColumnCount(4);
	m_pLengthTable->setHorizontalHeaderLabels({ tr("No."), tr("Color"), tr("Length"), tr("Category") });
	m_pLengthTable->verticalHeader()->setVisible(false);
	m_pLengthTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_pLengthTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pLengthTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	m_pLengthTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	m_pLengthTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	m_pLengthTable->verticalHeader()->setDefaultSectionSize(24);

	// Configure area table
	m_pAreaTable->setColumnCount(5);
	m_pAreaTable->setHorizontalHeaderLabels({ tr("No."), tr("Color"), tr("Area"), tr("Perimeter"), tr("Category") });
	m_pAreaTable->verticalHeader()->setVisible(false);
	m_pAreaTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_pAreaTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pAreaTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	m_pAreaTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	m_pAreaTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	m_pAreaTable->verticalHeader()->setDefaultSectionSize(24);

	// --- 4. Totals and Bottom Buttons ---
	m_pCopyTotalsButton = new QPushButton(tr("Copy Total Results"), this);
	m_pTotalsTextEdit = new QTextEdit(this);
	m_pTotalsTextEdit->setReadOnly(true);
	m_pTotalsTextEdit->setFixedHeight(80); // Fixed height

	QHBoxLayout* totalsHeaderLayout = new QHBoxLayout;
	totalsHeaderLayout->addWidget(new QLabel(tr("Totals:"), this));
	totalsHeaderLayout->addStretch();
	totalsHeaderLayout->addWidget(m_pCopyTotalsButton);

	m_pShowGrayCheck = new QCheckBox(tr("Show CAD Drawing in Gray"), this);
	m_pExportButton = new QPushButton(tr("Export Table to Excel"), this);
	m_pExportButton->setDefault(true);

	QHBoxLayout* showGrayLayout = new QHBoxLayout;
	showGrayLayout->addWidget(m_pShowGrayCheck);
	showGrayLayout->addStretch();

	QHBoxLayout* bottomLayout = new QHBoxLayout;
	bottomLayout->addStretch();
	bottomLayout->addWidget(m_pExportButton);
	bottomLayout->addStretch();

	// --- 5. Main Layout ---
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(m_pScopeGroupBox);
	mainLayout->addLayout(filterLayout);
	mainLayout->addWidget(m_pTabWidget);
	mainLayout->addLayout(totalsHeaderLayout);
	mainLayout->addWidget(m_pTotalsTextEdit);
	mainLayout->addLayout(showGrayLayout);
	mainLayout->addLayout(bottomLayout);

	setLayout(mainLayout);
}

void Mx2dMeasurementStatDialog::setupConnections()
{
	connect(m_pManualSelectRadio, &QRadioButton::toggled, this, &Mx2dMeasurementStatDialog::onScopeChanged);
	connect(m_pFilterByColorCheck, &QCheckBox::toggled, this, &Mx2dMeasurementStatDialog::onFilterByColorToggled);
	connect(m_pFilterByCategoryCheck, &QCheckBox::toggled, this, &Mx2dMeasurementStatDialog::onFilterByCategoryToggled);
	connect(m_pExportButton, &QPushButton::clicked, this, &QDialog::accept);
}

void Mx2dMeasurementStatDialog::populateTables()
{
	const QColor color = QColor(220, 80, 20); // Example color

	// Populate length table
	const QList<int> lengths = { 21758, 16361, 5336, 13549, 8176, 28568, 49458, 28592, 38379, 60523, 17077 };
	m_pLengthTable->setRowCount(lengths.size());
	for (int i = 0; i < lengths.size(); ++i) {
		m_pLengthTable->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
		m_pLengthTable->setCellWidget(i, 1, createColorWidget(color));
		m_pLengthTable->setItem(i, 2, new QTableWidgetItem(QString::number(lengths[i])));
		m_pLengthTable->setItem(i, 3, new QTableWidgetItem(tr("Default")));
	}

	// Populate area table
	const QList<QPair<double, double>> areas = {
		{731545785.96, 127074.67}, {1133792.31, 5455.81}, {131265143.03, 49510.27},
		{1241973815.68, 142479.88}, {747974149.04, 114033.84}, {343726610.58, 75606.65}
	};
	m_pAreaTable->setRowCount(areas.size());
	for (int i = 0; i < areas.size(); ++i) {
		m_pAreaTable->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
		m_pAreaTable->setCellWidget(i, 1, createColorWidget(color));
		m_pAreaTable->setItem(i, 2, new QTableWidgetItem(QString::number(areas[i].first, 'f', 2)));
		m_pAreaTable->setItem(i, 3, new QTableWidgetItem(QString::number(areas[i].second, 'f', 2)));
		m_pAreaTable->setItem(i, 4, new QTableWidgetItem(tr("Default")));
	}
}

QWidget* Mx2dMeasurementStatDialog::createColorWidget(const QColor& color)
{
	// Create a container QWidget that will be placed in the cell
	QWidget* colorWidget = new QWidget();

	// Create QLabel to display color
	QLabel* colorLabel = new QLabel();
	colorLabel->setFixedSize(30, 16); // Size can be adjusted as needed
	colorLabel->setStyleSheet(QString("background-color: %1; border: 1px solid #a9a9a9;")
		.arg(color.name()));
	colorLabel->setToolTip(color.name()); // Show color value on hover

	// Use layout to center QLabel in container
	QHBoxLayout* hLayout = new QHBoxLayout(colorWidget);
	hLayout->addWidget(colorLabel, 0, Qt::AlignCenter);
	hLayout->setContentsMargins(0, 0, 0, 0); // Remove margins for compactness

	colorWidget->setLayout(hLayout);

	return colorWidget;
}

void Mx2dMeasurementStatDialog::updateTotals()
{
	QString totalsText = tr("Total Length:")+"419502\n";
	totalsText += (tr("Total Area:") + "3197619296.6;" + tr("Total Perimeter:") + "514161.12");
	m_pTotalsTextEdit->setText(totalsText);
}

void Mx2dMeasurementStatDialog::onScopeChanged()
{
	bool manualMode = m_pManualSelectRadio->isChecked();
	m_pSelectButton->setEnabled(manualMode);
	m_pClearButton->setEnabled(manualMode);
}

void Mx2dMeasurementStatDialog::onFilterByColorToggled(bool checked)
{
	m_pOpenColorFilterButton->setEnabled(checked);
}

void Mx2dMeasurementStatDialog::onFilterByCategoryToggled(bool checked)
{
	m_pOpenCategoryFilterButton->setEnabled(checked);
}