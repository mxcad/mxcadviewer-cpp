/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dAnnotationSettingDialog.h"

#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QLineEdit>
#include <QComboBox>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPalette>
#include <QButtonGroup>

Mx2dAnnotationSettingDialog::Mx2dAnnotationSettingDialog(QWidget* parent)
	: QDialog(parent)
{
	Qt::WindowFlags flags = Qt::Dialog;
	flags |= Qt::CustomizeWindowHint;
	flags |= Qt::WindowTitleHint;
	flags |= Qt::WindowCloseButtonHint;
	setWindowFlags(flags);

	setWindowTitle(tr("Annotation Settings"));
	//setMinimumWidth(400);
	setFixedSize(500, 650);

	initUI();
	initLayout();
}

void Mx2dAnnotationSettingDialog::initUI()
{
	// Color preview and selection
	m_pLblColorPreview = new QLabel();
	m_pLblColorPreview->setFixedSize(60, 30);
	m_pLblColorPreview->setStyleSheet("background-color: #507FB9; border: 1px solid gray;");
	m_pBtnPickColor = new QPushButton(QIcon(":/resources/images2d/2d_btnSelColor.svg"), tr("Select Color"));

	// Text height setting
	m_pRadioAutoHeight = new QRadioButton(tr("Auto"));
	m_pRadioManualHeight = new QRadioButton(tr("Manual Setting"));
	m_pEditHeight = new QLineEdit();
	m_pEditHeight->setEnabled(false);
	m_pEditHeight->setMaximumWidth(150);
	m_pEditHeight->setText("0.00");

	m_pBtnPickOnScreen = new QPushButton(tr("Pick on Screen"));
	m_pBtnPickOnScreen->setEnabled(false);

	m_pHeightGroup = new QButtonGroup(this);
	m_pHeightGroup->addButton(m_pRadioAutoHeight);
	m_pHeightGroup->addButton(m_pRadioManualHeight);
	m_pRadioAutoHeight->setChecked(true);

	connect(m_pRadioManualHeight, &QRadioButton::toggled, [this](bool checked) {
		m_pEditHeight->setEnabled(checked);
		m_pBtnPickOnScreen->setEnabled(checked);
		});

	// Tab settings
	m_pTabWidget = new QTabWidget();
	m_pTabArea = new QWidget();
	m_pTabLength = new QWidget();
	m_pTabCoordinate = new QWidget();

	// TODO: Need to change to buttons instead of using tabWidget
	m_pTabWidget->addTab(m_pTabArea, tr("Area"));
	m_pTabWidget->addTab(m_pTabLength, tr("Length"));
	m_pTabWidget->addTab(m_pTabCoordinate, tr("Coordinate"));

	// Area tab
	m_pRadioShowRealValue_Area = new QRadioButton(tr("Display as actual value"));
	m_pRadioConvertUnit_Area = new QRadioButton(tr(u8"Drawing unit is mm, area value displayed as m²"));
	m_pRadioShowRealValue_Area->setChecked(true);

	m_pComboPrecision_Area = new QComboBox();
	m_pComboPrecision_Area->setStyleSheet("QComboBox{ combobox-popup: 0; }");
	m_pComboPrecision_Area->addItems({ tr("Auto"), tr("Integer(0)"), tr("One decimal(0.0)"), tr("Two decimals(0.00)") , tr("Three decimals(0.000)") });

	QVBoxLayout* pDisplayGroupArea = new QVBoxLayout();
	QHBoxLayout* pPrecisionLayoutArea = new QHBoxLayout();
	pPrecisionLayoutArea->addWidget(new QLabel(tr("Display Precision:")));
	pPrecisionLayoutArea->addWidget(m_pComboPrecision_Area);
	pPrecisionLayoutArea->addStretch();
	pDisplayGroupArea->addWidget(m_pRadioShowRealValue_Area);
	pDisplayGroupArea->addWidget(m_pRadioConvertUnit_Area);
	pDisplayGroupArea->addLayout(pPrecisionLayoutArea);
	m_pTabArea->setLayout(pDisplayGroupArea);

	// Length tab
	m_pRadioShowRealValue_Length = new QRadioButton(tr("Display as actual value"));
	m_pRadioConvertUnit_Length = new QRadioButton(tr("Drawing unit is mm, length value displayed as m"));
	m_pRadioShowRealValue_Length->setChecked(true);

	m_pComboPrecision_Length = new QComboBox();
	m_pComboPrecision_Length->setStyleSheet("QComboBox{ combobox-popup: 0; }");
	m_pComboPrecision_Length->addItems({ tr("Auto"), tr("Integer(0)"), tr("One decimal(0.0)"), tr("Two decimals(0.00)") , tr("Three decimals(0.000)") });

	QVBoxLayout* pDisplayGroupLength = new QVBoxLayout();
	QHBoxLayout* pPrecisionLayoutLength = new QHBoxLayout();
	pPrecisionLayoutLength->addWidget(new QLabel(tr("Display Precision:")));
	pPrecisionLayoutLength->addWidget(m_pComboPrecision_Length);
	pPrecisionLayoutLength->addStretch();
	pDisplayGroupLength->addWidget(m_pRadioShowRealValue_Length);
	pDisplayGroupLength->addWidget(m_pRadioConvertUnit_Length);
	pDisplayGroupLength->addLayout(pPrecisionLayoutLength);
	m_pTabLength->setLayout(pDisplayGroupLength);

	// Coordinate tab
	m_pRadioShowRealValue_Coordinate = new QRadioButton(tr("Display as actual value"));
	m_pRadioConvertUnit_Coordinate = new QRadioButton(tr("Drawing unit is mm, coordinate value displayed as m"));
	m_pRadioShowRealValue_Coordinate->setChecked(true);

	m_pComboPrecision_Coordinate = new QComboBox();
	m_pComboPrecision_Coordinate->setStyleSheet("QComboBox{ combobox-popup: 0; }");
	m_pComboPrecision_Coordinate->addItems({ tr("Auto"), tr("Integer(0)"), tr("One decimal(0.0)"), tr("Two decimals(0.00)") , tr("Three decimals(0.000)") });

	QVBoxLayout* pDisplayGroupCoordinate = new QVBoxLayout();
	QHBoxLayout* pPrecisionLayoutCoordinate = new QHBoxLayout();
	pPrecisionLayoutCoordinate->addWidget(new QLabel(tr("Display Precision:")));
	pPrecisionLayoutCoordinate->addWidget(m_pComboPrecision_Coordinate);
	pPrecisionLayoutCoordinate->addStretch();
	pDisplayGroupCoordinate->addWidget(m_pRadioShowRealValue_Coordinate);
	pDisplayGroupCoordinate->addWidget(m_pRadioConvertUnit_Coordinate);
	pDisplayGroupCoordinate->addLayout(pPrecisionLayoutCoordinate);
	m_pTabCoordinate->setLayout(pDisplayGroupCoordinate);

	// Save button
	m_pBtnSave = new QPushButton(tr("Save"));
}

void Mx2dAnnotationSettingDialog::initLayout()
{
	QVBoxLayout* pMainLayout = new QVBoxLayout(this);

	// Color section
	QGroupBox* pColorGroup = new QGroupBox(tr("Annotation Color"));
	QHBoxLayout* pColorLayout = new QHBoxLayout();
	pColorLayout->addWidget(new QLabel(tr("Current Color:")));
	pColorLayout->addWidget(m_pLblColorPreview);
	pColorLayout->addStretch();
	pColorLayout->addWidget(m_pBtnPickColor);
	pColorGroup->setLayout(pColorLayout);
	pMainLayout->addWidget(pColorGroup);

	// Text height setting
	QGroupBox* pHeightGroup = new QGroupBox(tr("Text Height"));
	QVBoxLayout* pHeightLayout = new QVBoxLayout();
	QHBoxLayout* pHeightManualLayout = new QHBoxLayout();
	pHeightManualLayout->addWidget(m_pRadioManualHeight);
	pHeightManualLayout->addWidget(m_pEditHeight);
	pHeightManualLayout->addStretch();
	pHeightManualLayout->addWidget(m_pBtnPickOnScreen);

	pHeightLayout->addWidget(m_pRadioAutoHeight);
	pHeightLayout->addLayout(pHeightManualLayout);
	QLabel* pHint = new QLabel(tr("(The above settings take effect for newly added annotations after modification)"));
	pHeightLayout->addWidget(pHint);
	pHeightGroup->setLayout(pHeightLayout);
	pMainLayout->addWidget(pHeightGroup);

	// Display and precision
	QGroupBox* pDisplayGroup = new QGroupBox(tr("Display and Precision Settings"));
	QVBoxLayout* pDisplayLayout = new QVBoxLayout();
	pDisplayLayout->addWidget(m_pTabWidget);
	pDisplayGroup->setLayout(pDisplayLayout);
	pMainLayout->addWidget(pDisplayGroup);

	// Save button
	QHBoxLayout* pBtnLayout = new QHBoxLayout();
	pBtnLayout->addStretch();
	pBtnLayout->addWidget(m_pBtnSave);
	pMainLayout->addLayout(pBtnLayout);
}