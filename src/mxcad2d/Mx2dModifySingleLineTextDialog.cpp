/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dModifySingleLineTextDialog.h"
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include "MxUtils.h"
#include "Mx2dSignalTransfer.h"

Mx2dModifySingleLineTextDialog::Mx2dModifySingleLineTextDialog(QWidget* parent)
	: QDialog(parent)
{
	Qt::WindowFlags flags = Qt::Dialog;
	flags |= Qt::CustomizeWindowHint;
	flags |= Qt::WindowTitleHint;
	flags |= Qt::WindowCloseButtonHint;
	setWindowFlags(flags);

	setWindowTitle(tr("Modify Single Line Text"));
	setFixedHeight(150);
	initUI();
	initLayout();
	connectSignals();
}

void Mx2dModifySingleLineTextDialog::onAgree()
{
	auto inputText = getInputText();
	if (inputText.isEmpty()) {
		QMessageBox::information(this, tr("Prompt"), tr("Please enter text content."));
		return;
	}
	accept();
	emit Mx2dSignalTransfer::getInstance().signalModifyText(MxUtils::gCurrentTab, m_textId, inputText);
}

void Mx2dModifySingleLineTextDialog::onCancel()
{
	reject();
}

QString Mx2dModifySingleLineTextDialog::getInputText() const
{
	return m_pEditContent->text().trimmed();
}

void Mx2dModifySingleLineTextDialog::setTextId(const McDbObjectId& id)
{
	m_textId = id;
}

void Mx2dModifySingleLineTextDialog::setTextContent(const QString& content)
{
	m_pEditContent->setText(content);
}

void Mx2dModifySingleLineTextDialog::initUI()
{
	m_pEditContent = new QLineEdit();
	m_pBtnAgree = new QPushButton(tr("Confirm"));
	m_pBtnAgree->setFixedWidth(100);
	m_pBtnCancel = new QPushButton(tr("Cancel"));
	m_pBtnCancel->setFixedWidth(100);
}

void Mx2dModifySingleLineTextDialog::initLayout()
{
	QHBoxLayout* pBtnLayout = new QHBoxLayout();
	pBtnLayout->addStretch();
	pBtnLayout->addWidget(m_pBtnAgree, Qt::AlignRight);
	pBtnLayout->addWidget(m_pBtnCancel, Qt::AlignRight);

	QVBoxLayout* pMainLayout = new QVBoxLayout();
	pMainLayout->addWidget(new QLabel(tr("Enter new text content")), Qt::AlignLeft);
	pMainLayout->addWidget(m_pEditContent);
	pMainLayout->addLayout(pBtnLayout);
	this->setLayout(pMainLayout);
}

void Mx2dModifySingleLineTextDialog::connectSignals()
{
	connect(m_pBtnAgree, &QPushButton::clicked, this, &Mx2dModifySingleLineTextDialog::onAgree);
	connect(m_pBtnCancel, &QPushButton::clicked, this, &Mx2dModifySingleLineTextDialog::onCancel);
}