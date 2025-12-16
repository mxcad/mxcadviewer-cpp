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
class QLineEdit;
class Mx2dInsertSingleLineTextDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Mx2dInsertSingleLineTextDialog(QWidget* parent = nullptr);

private slots:
	void onInsert();
	void onCancel();

public:
	QString getInputText() const;

private:
	void initUI();
	void initLayout();
	void connectSignals();

	QLineEdit* m_pEditContent;
	QPushButton* m_pBtnInsert;
	QPushButton* m_pBtnCancel;
};
