/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dInsertSingleLineTextDialog.h"
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include "MxUtils.h"
#include "Mx2dUtils.h"

Mx2dInsertSingleLineTextDialog::Mx2dInsertSingleLineTextDialog(QWidget* parent)
	: QDialog(parent)
{
	Qt::WindowFlags flags = Qt::Dialog;
	flags |= Qt::CustomizeWindowHint;
	flags |= Qt::WindowTitleHint;
	flags |= Qt::WindowCloseButtonHint;
	setWindowFlags(flags);

	setWindowTitle(tr("Single Line Text"));
	setFixedHeight(150);
	initUI();
	initLayout();
	connectSignals();
}

void Mx2dInsertSingleLineTextDialog::onInsert()
{
	auto inputText = getInputText();
	if (inputText.isEmpty()) {
		QMessageBox::information(this, tr("Prompt"), tr("Please enter text content."));
		return;
	}
	accept();
	std::string sAscii = inputText.toLocal8Bit().constData(); // Content
	MrxDbgRbList spParam = Mx::mcutBuildList(RTSTR, sAscii.c_str(), 0);
	Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_DrawText", spParam.orphanData());
}

void Mx2dInsertSingleLineTextDialog::onCancel()
{
	reject();
}

QString Mx2dInsertSingleLineTextDialog::getInputText() const
{
	return m_pEditContent->text().trimmed();
}

void Mx2dInsertSingleLineTextDialog::initUI()
{
	m_pEditContent = new QLineEdit();
	m_pBtnInsert = new QPushButton(tr("Insert"));
	m_pBtnInsert->setFixedWidth(100);
	m_pBtnCancel = new QPushButton(tr("Cancel"));
	m_pBtnCancel->setFixedWidth(100);
}

void Mx2dInsertSingleLineTextDialog::initLayout()
{
	QHBoxLayout* pBtnLayout = new QHBoxLayout();
	pBtnLayout->addStretch();
	pBtnLayout->addWidget(m_pBtnInsert, Qt::AlignRight);
	pBtnLayout->addWidget(m_pBtnCancel, Qt::AlignRight);

	QVBoxLayout* pMainLayout = new QVBoxLayout();
	pMainLayout->addWidget(new QLabel(tr("Enter text content")), Qt::AlignLeft);
	pMainLayout->addWidget(m_pEditContent);
	pMainLayout->addLayout(pBtnLayout);
	this->setLayout(pMainLayout);
}

void Mx2dInsertSingleLineTextDialog::connectSignals()
{
	connect(m_pBtnInsert, &QPushButton::clicked, this, &Mx2dInsertSingleLineTextDialog::onInsert);
	connect(m_pBtnCancel, &QPushButton::clicked, this, &Mx2dInsertSingleLineTextDialog::onCancel);
}