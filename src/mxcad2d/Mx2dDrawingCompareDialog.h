/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QDialog>

class QToolButton;
class QLabel;
class QCheckBox;
class QWidget;
class Mx2dDrawingCompareSetColorDialog;
class Mx2dDrawingCompareDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Mx2dDrawingCompareDialog(QWidget* parent = nullptr);

private:
	void initUI();
	void initLayout();
	void connectSignals();
	Mx2dDrawingCompareSetColorDialog* m_pDlgSetColor;
	QToolButton* m_pBtnSetColor;
	QToolButton* m_pBtnSetBasePoint;
	QToolButton* m_pBtnResetBasePoint;

	QCheckBox* m_pCheckPaper1;
	QLabel* m_pColorLabel1;
	QLabel* m_pPathLabel1;

	QCheckBox* m_pCheckPaper2;
	QLabel* m_pColorLabel2;
	QLabel* m_pPathLabel2;

	QCheckBox* m_pCheckSyncDisplay;

	QWidget* m_pDrawingArea;
};
