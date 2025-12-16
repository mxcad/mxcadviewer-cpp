/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QScrollArea>

class Mx2dMeasurementDialog : public QDialog
{
	Q_OBJECT
public:
	explicit Mx2dMeasurementDialog(QWidget* parent = nullptr);

private:
	QVBoxLayout* m_mainLayout;
	QWidget* m_buttonWidget;
	QVBoxLayout* m_buttonLayout;

	void initWindowFlags();
	void addButton(const QString& iconPath, const QString& text, const char* slot = nullptr);

protected slots:
	void onButtonPolyAreaClicked();
	void onButtonArcPolyAreaClicked();
	void onButtonRectAreaClicked();
	void onButtonCoordMarkClicked();
	void onButtonAlignedDimClicked();
	void onButtonLinearDimClicked();
	void onButtonContinuousMeasurementClicked();
	void onButtonShowSegmentLengthClicked();
	void onButtonRadiusDimClicked();
	void onButtonArcLengthClicked();
	void onButtonCircleMeasurementClicked();
	void onButtonDistPointToLineClicked();
	void onButtonAngleMeasurementClicked();
	void onButtonCalculateSiderAreaClicked();
	void onButtonHatchAreaClicked();
	void onButtonAreaOffsetClicked();
	void onButtonBatchMeasurementClicked();
};
