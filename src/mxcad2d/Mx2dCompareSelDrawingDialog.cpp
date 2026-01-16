/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dCompareSelDrawingDialog.h"

#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QColorDialog>
#include <QMessageBox>
#include "Mx2dDrawingCompareDialog.h"

Mx2dCompareSelDrawingDialog::Mx2dCompareSelDrawingDialog(QWidget* parent)
	: QDialog(parent),
	m_color1(Qt::yellow),
	m_color2(Qt::cyan)
{
	Qt::WindowFlags flags = Qt::Dialog;
	flags |= Qt::CustomizeWindowHint;
	flags |= Qt::WindowTitleHint;
	flags |= Qt::WindowCloseButtonHint;
	setWindowFlags(flags);

	setWindowTitle(tr("Drawing Comparison"));
	setFixedSize(600, 200);
	setupUi();
	setupConnections();
}

Mx2dCompareSelDrawingDialog::~Mx2dCompareSelDrawingDialog()
{
}

void Mx2dCompareSelDrawingDialog::setupUi()
{
	m_pDrawingCompareDialog = new Mx2dDrawingCompareDialog(this);

	m_pInstructionLabel = new QLabel(tr("Please select the two drawings to compare and their display colors from the list of opened drawings below"), this);

	m_pDrawing1Label = new QLabel(tr("Drawing 1"), this);
	m_pDrawing1ComboBox = new QComboBox(this);
	m_pDrawing1ComboBox->setStyleSheet("QComboBox{ combobox-popup: 0; }");
	m_pColor1Button = new QPushButton(this);

	m_pDrawing2Label = new QLabel(tr("Drawing 2"), this);
	m_pDrawing2ComboBox = new QComboBox(this);
	m_pDrawing2ComboBox->setStyleSheet("QComboBox{ combobox-popup: 0; }");
	m_pColor2Button = new QPushButton(this);

	m_pStartButton = new QPushButton(tr("Start Comparison"), this);
	m_pCancelButton = new QPushButton(tr("Cancel"), this);

	m_pDrawing1ComboBox->addItem("C:\\Users\\MxDev2\\Downloads\\TestDrawing.dwg" + tr("(Model)"));
	m_pDrawing1ComboBox->addItem("C:\\Users\\MxDev2\\Downloads\\ExternalInternalMold.dwg" + tr("(Model)"));
	m_pDrawing2ComboBox->addItem("C:\\Users\\MxDev2\\Downloads\\ExternalInternalMold.dwg" + tr("(Model)"));
	m_pDrawing2ComboBox->addItem("C:\\Users\\MxDev2\\Downloads\\TestDrawing.dwg" + tr("(Model)"));

	m_pColor1Button->setFixedSize(28, 28);
	m_pColor2Button->setFixedSize(28, 28);
	updateColorButton(m_pColor1Button, m_color1);
	updateColorButton(m_pColor2Button, m_color2);

	// Create main vertical layout
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	// First row: instruction label
	mainLayout->addWidget(m_pInstructionLabel);

	// Second row: drawing 1 selection
	QHBoxLayout* drawing1Layout = new QHBoxLayout();
	drawing1Layout->addWidget(m_pDrawing1Label);
	drawing1Layout->addWidget(m_pDrawing1ComboBox);
	drawing1Layout->addWidget(m_pColor1Button);
	drawing1Layout->setStretch(1, 1); // Let ComboBox take remaining space
	mainLayout->addLayout(drawing1Layout);

	// Third row: drawing 2 selection
	QHBoxLayout* drawing2Layout = new QHBoxLayout();
	drawing2Layout->addWidget(m_pDrawing2Label);
	drawing2Layout->addWidget(m_pDrawing2ComboBox);
	drawing2Layout->addWidget(m_pColor2Button);
	drawing2Layout->setStretch(1, 1); // Let ComboBox take remaining space
	mainLayout->addLayout(drawing2Layout);

	// Fourth row: buttons
	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addStretch();
	buttonLayout->addWidget(m_pStartButton);
	buttonLayout->addWidget(m_pCancelButton);
	buttonLayout->addStretch();
	mainLayout->addLayout(buttonLayout);

	//mainLayout->setContentsMargins(10, 10, 10, 20);
	setLayout(mainLayout);
}

void Mx2dCompareSelDrawingDialog::setupConnections()
{
	connect(m_pColor1Button, &QPushButton::clicked, this, &Mx2dCompareSelDrawingDialog::onColor1ButtonClicked);
	connect(m_pColor2Button, &QPushButton::clicked, this, &Mx2dCompareSelDrawingDialog::onColor2ButtonClicked);
	connect(m_pStartButton, &QPushButton::clicked, this, &Mx2dCompareSelDrawingDialog::onStartCompareClicked);
	connect(m_pCancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void Mx2dCompareSelDrawingDialog::updateColorButton(QPushButton* button, const QColor& color)
{
	button->setStyleSheet(QString("background-color: %1; border: 1px solid black;").arg(color.name()));
}

void Mx2dCompareSelDrawingDialog::onColor1ButtonClicked()
{
	const QColor newColor = QColorDialog::getColor(m_color1, this, tr("Select color for drawing 1"));
	if (newColor.isValid()) {
		m_color1 = newColor;
		updateColorButton(m_pColor1Button, m_color1);
	}
}

void Mx2dCompareSelDrawingDialog::onColor2ButtonClicked()
{
	const QColor newColor = QColorDialog::getColor(m_color2, this, tr("Select color for drawing 2"));
	if (newColor.isValid()) {
		m_color2 = newColor;
		updateColorButton(m_pColor2Button, m_color2);
	}
}

void Mx2dCompareSelDrawingDialog::onStartCompareClicked()
{
	// TODO: Need to check if the two drawings can be compared before proceeding to the next step.

	accept(); // Close dialog
	m_pDrawingCompareDialog->show();
}

QString Mx2dCompareSelDrawingDialog::getDrawing1Path() const
{
	return m_pDrawing1ComboBox->currentText();
}

QString Mx2dCompareSelDrawingDialog::getDrawing2Path() const
{
	return m_pDrawing2ComboBox->currentText();
}

QColor Mx2dCompareSelDrawingDialog::getDrawing1Color() const
{
	return m_color1;
}

QColor Mx2dCompareSelDrawingDialog::getDrawing2Color() const
{
	return m_color2;
}