/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QDialog>
#include <QToolButton>
#include <QPushButton>
#include <QLabel>
#include <QString>

class Mx2dDrawingCompareSetColorDialog : public QDialog
{
	Q_OBJECT

public:
	Mx2dDrawingCompareSetColorDialog(QWidget* parent = nullptr);

private slots:
	void onResetColors();

private:
	QToolButton* m_btnColor1;
	QToolButton* m_btnColor2;
	QToolButton* m_btnColorSame;
	QLabel* m_lblPath1;
	QLabel* m_lblPath2;
	QLabel* m_lblSame;
	QPushButton* m_btnReset;
};
