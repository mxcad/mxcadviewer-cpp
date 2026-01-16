/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dExtractTextDialog.h"

#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include "MxLogger.h"

Mx2dExtractTextDialog::Mx2dExtractTextDialog(QWidget* parent)
	: QDialog(parent)
{
	Qt::WindowFlags flags = Qt::Dialog;
	flags |= Qt::CustomizeWindowHint;
	flags |= Qt::WindowTitleHint;
	flags |= Qt::WindowCloseButtonHint;
	setWindowFlags(flags);

	setWindowTitle(tr("Selected Text"));
	setMinimumSize(500, 400);

	setupUi();
	setupConnections();
}

Mx2dExtractTextDialog::~Mx2dExtractTextDialog()
{
}

void Mx2dExtractTextDialog::setTexts(const QStringList& texts)
{
	m_pTextEdit->setPlainText(texts.join('\n'));
}

void Mx2dExtractTextDialog::setupUi()
{
	m_pTextEdit = new QTextEdit(this);
	m_pCopyButton = new QPushButton(tr("Copy to Clipboard"), this);


	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(m_pTextEdit);

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addStretch();
	buttonLayout->addWidget(m_pCopyButton);

	mainLayout->addLayout(buttonLayout);

	setLayout(mainLayout);
}

void Mx2dExtractTextDialog::setupConnections()
{
	connect(m_pCopyButton, &QPushButton::clicked, this, &Mx2dExtractTextDialog::onCopyButtonClicked);
}

void Mx2dExtractTextDialog::onCopyButtonClicked()
{
	QClipboard* clipboard = QApplication::clipboard();
	if (!clipboard) {
		LOG_ERROR(QString("Could not get the clipboard."));
		return;
	}

	QString textToCopy = m_pTextEdit->toPlainText();
	clipboard->setText(textToCopy);

	accept();
}