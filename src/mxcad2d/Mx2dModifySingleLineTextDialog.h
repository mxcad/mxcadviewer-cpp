/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QDialog>
#include "MxCADInclude.h"
class QPushButton;
class QLineEdit;
class Mx2dModifySingleLineTextDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Mx2dModifySingleLineTextDialog(QWidget* parent = nullptr);

private slots:
	void onAgree();
	void onCancel();

public:
	QString getInputText() const;
	void setTextId(const McDbObjectId& id);
	void setTextContent(const QString& content);
private:
	void initUI();
	void initLayout();
	void connectSignals();

	QLineEdit* m_pEditContent;
	QPushButton* m_pBtnAgree;
	QPushButton* m_pBtnCancel;
	McDbObjectId m_textId;
};
