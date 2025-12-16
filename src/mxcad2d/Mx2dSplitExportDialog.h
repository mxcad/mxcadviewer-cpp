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
class QRadioButton;
class QPushButton;

class Mx2dSplitExportDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Mx2dSplitExportDialog(QWidget* parent = nullptr);
	~Mx2dSplitExportDialog();

private:
	void setupUi();

	QLabel* m_pInstructionsLabel;

	QLabel* m_pAreaTypeLabel;
	QRadioButton* m_pRectAreaRadio;
	QRadioButton* m_pIrregularAreaRadio;

	QPushButton* m_pSelectAreaButton;
	QPushButton* m_pExportButton;
};