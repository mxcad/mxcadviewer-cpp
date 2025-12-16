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
class QLineEdit;
class QRadioButton;
class QPushButton;

class Mx2dNumberedTextDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Mx2dNumberedTextDialog(QWidget* parent = nullptr);
	~Mx2dNumberedTextDialog();

	QString getTextContent() const;

	int getStartNumber() const;

	bool isNumberPrefix() const;

private slots:
	void onOkClicked();

private:
	void setupUi();
	void setupConnections();

	QLabel* m_pInstructionLabel;
	QLabel* m_pContentLabel;
	QLineEdit* m_pContentLineEdit;
	QLabel* m_pContentHintLabel;
	QLabel* m_pStartNumLabel;
	QLineEdit* m_pStartNumLineEdit;
	QLabel* m_pStartNumHintLabel;
	QLabel* m_pPositionLabel;
	QRadioButton* m_pPrefixRadio;
	QRadioButton* m_pSuffixRadio;
	QPushButton* m_pOkButton;

	QString m_textContent;
	int m_startNumber;
	bool m_isPrefix;
};