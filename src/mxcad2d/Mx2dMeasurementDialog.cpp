/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dMeasurementDialog.h"
#include <QHBoxLayout>
#include <QIcon>
#include "MxUtils.h"
#include "Mx2dUtils.h"
#include "Mx2dGuiDocument.h"

Mx2dMeasurementDialog::Mx2dMeasurementDialog(Mx2dGuiDocument* guiDoc, QWidget* parent)
	: QDialog(parent), m_guiDoc(guiDoc)
{
	initWindowFlags();

	// Main layout
	m_mainLayout = new QVBoxLayout(this);
	m_mainLayout->setContentsMargins(0, 0, 0, 0);
	m_mainLayout->setSpacing(0);

	m_buttonWidget = new QWidget(this);
	m_buttonLayout = new QVBoxLayout(m_buttonWidget);
	m_buttonLayout->setSpacing(0);
	m_buttonLayout->setContentsMargins(0, 0, 0, 0);

	// Add buttons
	addButton(":/resources/images2d/2d_aligned.svg", tr("Aligned"), SLOT(onButtonAlignedDimClicked()));
	addButton(":/resources/images2d/2d_linear.svg", tr("Linear"), SLOT(onButtonLinearDimClicked()));
	addButton(":/resources/images2d/2d_area.svg", tr("Area"), SLOT(onButtonPolyAreaClicked()));
	addButton(":/resources/images2d/2d_rectArea.svg", tr("Rectangular Area"), SLOT(onButtonRectAreaClicked()));
	addButton(":/resources/images2d/2d_coordinate.svg", tr("Coordinate"), SLOT(onButtonCoordMarkClicked()));
	
	addButton(":/resources/images2d/2d_arcLength.svg", tr("Arc Length"), SLOT(onButtonArcLengthClicked()));
	addButton(":/resources/images2d/2d_pt2lineDist.svg", tr("Point to Line Distance"), SLOT(onButtonDistPointToLineClicked()));
	addButton(":/resources/images2d/2d_continuousMeasurement.svg", tr("Continuous Measurement"), SLOT(onButtonContinuousMeasurementClicked()));
	addButton(":/resources/images2d/2d_batchMeasurement.svg", tr("Batch Measurement"), SLOT(onButtonBatchMeasurementClicked()));
	addButton(":/resources/images2d/2d_segLength.svg", tr("Show Segment Lengths"), SLOT(onButtonShowSegmentLengthClicked()));
	
	addButton(":/resources/images2d/2d_areaWithArc.svg", tr("Area(with arcs)"), SLOT(onButtonArcPolyAreaClicked()));
	addButton(":/resources/images2d/2d_fillArea.svg", tr("Hatch Area"), SLOT(onButtonHatchAreaClicked()));
	addButton(":/resources/images2d/2d_sideArea.svg", tr("Calculate Side Area"), SLOT(onButtonCalculateSiderAreaClicked()));
	addButton(":/resources/images2d/2d_areaOffset.svg", tr("Area Offset"), SLOT(onButtonAreaOffsetClicked()));
	addButton(":/resources/images2d/2d_measureCircle.svg", tr("Circle"), SLOT(onButtonCircleMeasurementClicked()));
	addButton(":/resources/images2d/2d_radius.svg", tr("Radius"), SLOT(onButtonRadiusDimClicked()));
	addButton(":/resources/images2d/2d_measureAngle.svg", tr("Angle"), SLOT(onButtonAngleMeasurementClicked()));
	
#ifdef MX_DEVELOPING
	addButton(":/resources/images2d/2d_measure.svg", tr("Set Dimension Ratio"));
	addButton(":/resources/images2d/2d_measure.svg", tr("Modify Single Dimension Properties"));
	addButton(":/resources/images2d/2d_measure.svg", tr("Measurement Statistics"));
#endif
	m_mainLayout->addWidget(m_buttonWidget);

	adjustSize();
	setFixedSize(size());
}

void Mx2dMeasurementDialog::initWindowFlags()
{
	setWindowTitle(tr("Measurement"));
	Qt::WindowFlags flags = Qt::Dialog;
	flags |= Qt::CustomizeWindowHint;
	flags |= Qt::WindowTitleHint;
	flags |= Qt::WindowCloseButtonHint;
	setWindowFlags(flags);
}

void Mx2dMeasurementDialog::addButton(const QString& iconPath, const QString& text, const char* slot)
{
	QPushButton* button = new QPushButton(QIcon(iconPath), text, this);
	button->setIconSize(QSize(24, 24));
	button->setStyleSheet("text-align: left; padding: 5px; padding-right: 10px;");
	m_buttonLayout->addWidget(button);
	if (slot)
		connect(button, SIGNAL(clicked()), this, slot);
}

void Mx2dMeasurementDialog::onButtonArcPolyAreaClicked()
{
	MxUtils::doAction([this]() {
		m_guiDoc->executeCommand("Mx_DrawArcPolyAreaMark");
		});
}

void Mx2dMeasurementDialog::onButtonRectAreaClicked()
{
	m_guiDoc->executeCommand("Mx_DrawRectAreaMark");
}

void Mx2dMeasurementDialog::onButtonCoordMarkClicked()
{
	m_guiDoc->executeCommand("Mx_DrawCartesianCoordMark");
}

void Mx2dMeasurementDialog::onButtonAlignedDimClicked()
{
	m_guiDoc->executeCommand("Mx_DrawAlignedDimMark");
}

void Mx2dMeasurementDialog::onButtonLinearDimClicked()
{
	m_guiDoc->executeCommand("Mx_DrawLinearDimMark");
}

void Mx2dMeasurementDialog::onButtonContinuousMeasurementClicked()
{
	MxUtils::doAction([this]() {
		m_guiDoc->executeCommand("Mx_DrawContinuousMeasurementMark");
		});
}

void Mx2dMeasurementDialog::onButtonShowSegmentLengthClicked()
{
	MxUtils::doAction([this]() {
		m_guiDoc->executeCommand("Mx_ShowSegmentLengths");
		});
}

void Mx2dMeasurementDialog::onButtonRadiusDimClicked()
{
	m_guiDoc->executeCommand("Mx_DrawRadiusDimMark");
}

void Mx2dMeasurementDialog::onButtonArcLengthClicked()
{
	MxUtils::doAction([this]() {
		m_guiDoc->executeCommand("Mx_DrawArcLengthDimMark");
		});
}

void Mx2dMeasurementDialog::onButtonCircleMeasurementClicked()
{
	m_guiDoc->executeCommand("Mx_DrawCircleMeasurementMark");
}

void Mx2dMeasurementDialog::onButtonDistPointToLineClicked()
{
	MxUtils::doAction([this]() {
		m_guiDoc->executeCommand("Mx_DrawDistPointToLineMark");
		});
}

void Mx2dMeasurementDialog::onButtonAngleMeasurementClicked()
{
	MxUtils::doAction([this]() {
		m_guiDoc->executeCommand("Mx_DrawAngleMeasurementMark");
		});
}

void Mx2dMeasurementDialog::onButtonCalculateSiderAreaClicked()
{
	MxUtils::doAction([this]() {
		m_guiDoc->executeCommand("Mx_CalculateSiderArea");
		});
}

void Mx2dMeasurementDialog::onButtonHatchAreaClicked()
{
	MxUtils::doAction([this]() {
		m_guiDoc->executeCommand("Mx_DrawHatchArea2Mark");
		});
}

void Mx2dMeasurementDialog::onButtonAreaOffsetClicked()
{
	MxUtils::doAction([this]() {
		m_guiDoc->executeCommand("Mx_SelectAreaToOffset");
		});
}

void Mx2dMeasurementDialog::onButtonBatchMeasurementClicked()
{
	MxUtils::doAction([this]() {
		m_guiDoc->executeCommand("Mx_BatchMeasure");
		});
}

void Mx2dMeasurementDialog::onButtonPolyAreaClicked()
{
	m_guiDoc->executeCommand("Mx_DrawPolyAreaMark");
}