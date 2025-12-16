/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QDialog>

class QPushButton;
class QLabel;
class QLineEdit;
class QTableWidget;

class Mx2dDimCategoryManagerDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Mx2dDimCategoryManagerDialog(QWidget* parent = nullptr);

private:
	void initUI();
	void initLayout();

	QPushButton* m_pBtnNewCategory;
	QLabel* m_pLblCurrentCategory;
	QPushButton* m_pBtnShowAll;
	QPushButton* m_pBtnHideAll;

	QLineEdit* m_pEditSearch;
	QPushButton* m_pBtnSearch;

	QTableWidget* m_pTable;
	QLabel* m_pLblTip;
};
