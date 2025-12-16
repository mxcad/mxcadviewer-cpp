/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QDialog>
#include <QStringList>

class QTextEdit;
class QPushButton;

class Mx2dExtractTextDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Mx2dExtractTextDialog(QWidget* parent = nullptr);
	~Mx2dExtractTextDialog();

	void setTexts(const QStringList& texts);

private slots:

	void onCopyButtonClicked();

private:
	void setupUi();

	void setupConnections();

	QTextEdit* m_pTextEdit;
	QPushButton* m_pCopyButton;
};