/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dLeaderTextInputDialog.h"

#include <QtWidgets>
#include "Mx2dUtils.h"
#include "MxUtils.h"
#include "Mx2dGuiDocument.h"

Mx2dLeaderTextInputDialog::Mx2dLeaderTextInputDialog(Mx2dGuiDocument* guiDoc, QWidget* parent)
	: QDialog(parent), m_guiDoc(guiDoc)
{
	Qt::WindowFlags flags = Qt::Dialog;
	flags |= Qt::CustomizeWindowHint;
	flags |= Qt::WindowTitleHint;
	flags |= Qt::WindowCloseButtonHint;
	setWindowFlags(flags);
	setupUi();
	setupConnections();

	onTextChanged();
}

Mx2dLeaderTextInputDialog::~Mx2dLeaderTextInputDialog()
{
}

QString Mx2dLeaderTextInputDialog::getText() const
{
	return m_text;
}

void Mx2dLeaderTextInputDialog::setupUi()
{
	setWindowTitle(tr("Leader Multi Line Text"));
	resize(450, 300);

	QVBoxLayout* pMainLayout = new QVBoxLayout(this);
	pMainLayout->setContentsMargins(10, 10, 10, 10);
	pMainLayout->setSpacing(10);

	m_pTextEdit = new QTextEdit();
	pMainLayout->addWidget(m_pTextEdit, 1);

	QHBoxLayout* pBottomLayout = new QHBoxLayout();
	pBottomLayout->setContentsMargins(0, 0, 0, 0);

	m_pLineCountLabel = new QLabel();
	pBottomLayout->addWidget(m_pLineCountLabel);

	pBottomLayout->addStretch(1);

	m_pStartButton = new QPushButton(tr("Start"));
	m_pStartButton->setFixedSize(100, 28);
	pBottomLayout->addWidget(m_pStartButton);

	pMainLayout->addLayout(pBottomLayout);
}

void Mx2dLeaderTextInputDialog::setupConnections()
{
	connect(m_pTextEdit, &QTextEdit::textChanged, this, &Mx2dLeaderTextInputDialog::onTextChanged);

	connect(m_pStartButton, &QPushButton::clicked, this, &Mx2dLeaderTextInputDialog::onInsertClicked);
}

void Mx2dLeaderTextInputDialog::onTextChanged()
{
	int currentLineCount = m_pTextEdit->document()->lineCount();

	m_pLineCountLabel->setText(QString(tr("Lines: %1/%2")).arg(currentLineCount).arg(m_maxLines));
}

void Mx2dLeaderTextInputDialog::onInsertClicked()
{
	QString currentText = m_pTextEdit->toPlainText();

	if (currentText.trimmed().isEmpty())
	{
        QMessageBox::information(this, tr("Tip"), tr("Input content cannot be empty or only contains spaces."));

		return;
	}

	int currentLineCount = m_pTextEdit->document()->lineCount();
	if (currentLineCount > m_maxLines)
	{
        QMessageBox::information(this, tr("Tip"), QString(tr("The input text cannot exceed %1 lines.")).arg(m_maxLines));
		return;
	}

	m_text = currentText;
	//accept();

	std::string sAscii = m_text.toLocal8Bit();
	MrxDbgRbList spParam = Mx::mcutBuildList(RTSTR, sAscii.c_str(), 0);
	m_guiDoc->executeCommand("Mx_DrawLeaderMark", spParam.orphanData());
}