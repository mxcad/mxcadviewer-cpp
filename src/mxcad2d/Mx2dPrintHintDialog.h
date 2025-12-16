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

class Mx2dPrintHintDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Mx2dPrintHintDialog(QWidget* parent = nullptr);
	~Mx2dPrintHintDialog();
private slots:
	void onOpenPrintSettingsClicked();
private:
	void setupUi();

	QLabel* m_pIconLabel;
	QLabel* m_pTextLabel;
	QPushButton* m_pOpenSettingsButton;
};