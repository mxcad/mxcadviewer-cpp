/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dNumberedTextDialog.h"

#include <QtWidgets>
#include "MxUtils.h"
#include "Mx2dUtils.h"

Mx2dNumberedTextDialog::Mx2dNumberedTextDialog(QWidget* parent)
	: QDialog(parent), m_startNumber(1), m_isPrefix(false)
{
	Qt::WindowFlags flags = Qt::Dialog;
	flags |= Qt::CustomizeWindowHint;
	flags |= Qt::WindowTitleHint;
	flags |= Qt::WindowCloseButtonHint;
	setWindowFlags(flags);
	setupUi();
	setupConnections();
}

Mx2dNumberedTextDialog::~Mx2dNumberedTextDialog()
{
}

QString Mx2dNumberedTextDialog::getTextContent() const
{
	return m_textContent;
}

int Mx2dNumberedTextDialog::getStartNumber() const
{
	return m_startNumber;
}

bool Mx2dNumberedTextDialog::isNumberPrefix() const
{
	return m_isPrefix;
}


void Mx2dNumberedTextDialog::setupUi()
{
	setWindowTitle(tr("Numbered Text"));
	setFixedSize(560, 280);

	QVBoxLayout* pMainLayout = new QVBoxLayout(this);
	pMainLayout->setContentsMargins(20, 20, 20, 20);

	m_pInstructionLabel = new QLabel(tr("Please enter text content and number:"));
	pMainLayout->addWidget(m_pInstructionLabel);

	QFormLayout* pFormLayout = new QFormLayout();
	pFormLayout->setSpacing(15);
	pFormLayout->setLabelAlignment(Qt::AlignLeft);
	pFormLayout->setContentsMargins(0, 15, 0, 15);

	QHBoxLayout* pContentLayout = new QHBoxLayout();
	m_pContentLineEdit = new QLineEdit();
	m_pContentHintLabel = new QLabel(tr("(Enter 0-20 characters)"));
	m_pContentHintLabel->setStyleSheet("color: grey;");
	pContentLayout->addWidget(m_pContentLineEdit);
	pContentLayout->addWidget(m_pContentHintLabel);
	pFormLayout->addRow(tr("Text Content:"), pContentLayout);

	QHBoxLayout* pStartNumLayout = new QHBoxLayout();
	m_pStartNumLineEdit = new QLineEdit("1");
	m_pStartNumLineEdit->setValidator(new QIntValidator(0, INT_MAX, this));
	m_pStartNumHintLabel = new QLabel(tr("(Number auto-increments by 1)"));
	m_pStartNumHintLabel->setStyleSheet("color: grey;");
	pStartNumLayout->addWidget(m_pStartNumLineEdit);
	pStartNumLayout->addWidget(m_pStartNumHintLabel);
	pStartNumLayout->addStretch();
	pFormLayout->addRow(tr("Start Number:"), pStartNumLayout);

	QHBoxLayout* pPositionLayout = new QHBoxLayout();
	m_pPrefixRadio = new QRadioButton(tr("Number as prefix"));
	m_pSuffixRadio = new QRadioButton(tr("Number as suffix"));
	m_pSuffixRadio->setChecked(true);
	pPositionLayout->addWidget(m_pPrefixRadio);
	pPositionLayout->addWidget(m_pSuffixRadio);
	pPositionLayout->addStretch();
	pFormLayout->addRow(tr("Number Position:"), pPositionLayout);

	pMainLayout->addLayout(pFormLayout);
	pMainLayout->addStretch();

	QHBoxLayout* pButtonLayout = new QHBoxLayout();
	m_pOkButton = new QPushButton(tr("OK"));
	m_pOkButton->setFixedSize(100, 32);
	pButtonLayout->addStretch();
	pButtonLayout->addWidget(m_pOkButton);
	pMainLayout->addLayout(pButtonLayout);
}

void Mx2dNumberedTextDialog::setupConnections()
{
	connect(m_pOkButton, &QPushButton::clicked, this, &Mx2dNumberedTextDialog::onOkClicked);
}

void Mx2dNumberedTextDialog::onOkClicked()
{
	QString content = m_pContentLineEdit->text();
	QString numStr = m_pStartNumLineEdit->text();

	if (content.length() > 20) {
		QMessageBox::information(this, tr("Prompt"), tr("Text content cannot exceed 20 characters."));
		return;
	}

	bool isInt;
	int number = numStr.toInt(&isInt);

	if (!isInt || numStr.isEmpty()) {
		QMessageBox::information(this, tr("Prompt"), tr("Start number must be a valid integer."));
		return;
	}

	if (number < 0) {
		QMessageBox::information(this, tr("Prompt"), tr("Start number must be a non-negative integer."));
		return;
	}

	m_textContent = content;
	m_startNumber = number;
	m_isPrefix = m_pPrefixRadio->isChecked();

	std::string sAscii = m_textContent.toLocal8Bit().constData();
	MrxDbgRbList spParam = Mx::mcutBuildList(RTSTR, sAscii.c_str(), RTLONG, m_startNumber, RTSHORT, m_isPrefix ? 0 : 1, 0);
	Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_DrawNumberedText", spParam.orphanData());
	accept();
}