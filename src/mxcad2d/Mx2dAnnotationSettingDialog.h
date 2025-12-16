/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QDialog>

class QLabel;
class QPushButton;
class QRadioButton;
class QLineEdit;
class QComboBox;
class QTabWidget;
class QButtonGroup;

class Mx2dAnnotationSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Mx2dAnnotationSettingDialog(QWidget* parent = nullptr);

private:
	void initUI();
	void initLayout();

private:
	QLabel* m_pLblColorPreview;
	QPushButton* m_pBtnPickColor;

	QRadioButton* m_pRadioAutoHeight;
	QRadioButton* m_pRadioManualHeight;
	QLineEdit* m_pEditHeight;

	QPushButton* m_pBtnPickOnScreen;

	QTabWidget* m_pTabWidget;
	QWidget* m_pTabArea;
	QWidget* m_pTabLength;
	QWidget* m_pTabCoordinate;

	QRadioButton* m_pRadioShowRealValue_Area;
	QRadioButton* m_pRadioConvertUnit_Area;
	QComboBox* m_pComboPrecision_Area;


	QRadioButton* m_pRadioShowRealValue_Length;
	QRadioButton* m_pRadioConvertUnit_Length;
	QComboBox* m_pComboPrecision_Length;

	QRadioButton* m_pRadioShowRealValue_Coordinate;
	QRadioButton* m_pRadioConvertUnit_Coordinate;
	QComboBox* m_pComboPrecision_Coordinate;

	QPushButton* m_pBtnSave;

	QButtonGroup* m_pHeightGroup;
};
