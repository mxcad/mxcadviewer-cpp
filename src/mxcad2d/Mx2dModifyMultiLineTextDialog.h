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

class QTextEdit;
class QLabel;
class QPushButton;

class Mx2dModifyMultiLineTextDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Mx2dModifyMultiLineTextDialog(QWidget* parent = nullptr);
	~Mx2dModifyMultiLineTextDialog();

	QString getText() const;
	void setTextId(const McDbObjectId& id);
	void setTextContent(const QString& content);
private slots:
	void onTextChanged();
	void onAgreeClicked();

private:
	void setupUi();
	void setupConnections();

	QTextEdit* m_pTextEdit;
	QLabel* m_pLineCountLabel;
	QPushButton* m_pInsertButton;

	const int m_maxLines = 10;
	QString m_text;

	McDbObjectId m_textId;
};