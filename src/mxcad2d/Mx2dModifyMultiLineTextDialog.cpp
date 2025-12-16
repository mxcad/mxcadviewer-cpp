/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dModifyMultiLineTextDialog.h"

#include <QtWidgets>
#include "MxUtils.h"
#include "Mx2dSignalTransfer.h"

Mx2dModifyMultiLineTextDialog::Mx2dModifyMultiLineTextDialog(QWidget* parent)
	: QDialog(parent)
{
	Qt::WindowFlags flags = Qt::Dialog;
	flags |= Qt::CustomizeWindowHint;
	flags |= Qt::WindowTitleHint;
	flags |= Qt::WindowCloseButtonHint;
	setWindowFlags(flags);
	setupUi();
	setupConnections();

	// Initialize line count display
	onTextChanged();
}

Mx2dModifyMultiLineTextDialog::~Mx2dModifyMultiLineTextDialog()
{
}

QString Mx2dModifyMultiLineTextDialog::getText() const
{
	return m_text;
}

void Mx2dModifyMultiLineTextDialog::setTextId(const McDbObjectId& id)
{
	m_textId = id;
}

void Mx2dModifyMultiLineTextDialog::setTextContent(const QString& content)
{
	m_pTextEdit->setText(content);
	m_text = content;
}

void Mx2dModifyMultiLineTextDialog::setupUi()
{
	setWindowTitle(tr("Modify Multi-line Text"));
	resize(450, 300); // Set appropriate default size

	// --- Main layout ---
	QVBoxLayout* pMainLayout = new QVBoxLayout(this);
	pMainLayout->setContentsMargins(10, 10, 10, 10);
	pMainLayout->setSpacing(10);

	// --- Text edit box ---
	m_pTextEdit = new QTextEdit();
	pMainLayout->addWidget(m_pTextEdit, 1);

	// --- Bottom layout ---
	QHBoxLayout* pBottomLayout = new QHBoxLayout();
	pBottomLayout->setContentsMargins(0, 0, 0, 0);

	// Line count label
	m_pLineCountLabel = new QLabel();
	pBottomLayout->addWidget(m_pLineCountLabel);

	// Add a spring to push buttons to the right
	pBottomLayout->addStretch(1);

	// Confirm button
	m_pInsertButton = new QPushButton(tr("Confirm"));
	m_pInsertButton->setFixedSize(100, 28);
	pBottomLayout->addWidget(m_pInsertButton);

	// Add bottom layout to main layout
	pMainLayout->addLayout(pBottomLayout);
}

void Mx2dModifyMultiLineTextDialog::setupConnections()
{
	// Connect text edit box content change signal to onTextChanged slot
	connect(m_pTextEdit, &QTextEdit::textChanged, this, &Mx2dModifyMultiLineTextDialog::onTextChanged);

	// Connect confirm button click signal to onAgreeClicked slot
	connect(m_pInsertButton, &QPushButton::clicked, this, &Mx2dModifyMultiLineTextDialog::onAgreeClicked);
}

void Mx2dModifyMultiLineTextDialog::onTextChanged()
{
	// QTextEdit's document() returns a QTextDocument object that manages text content
	// lineCount() can get the current line count
	int currentLineCount = m_pTextEdit->document()->lineCount();

	// Update label text
	m_pLineCountLabel->setText(QString(tr("Lines: %1/%2")).arg(currentLineCount).arg(m_maxLines));
}

void Mx2dModifyMultiLineTextDialog::onAgreeClicked()
{
	QString currentText = m_pTextEdit->toPlainText();

	// Validation 1: Check if empty or only contains spaces
	if (currentText.trimmed().isEmpty())
	{
		QMessageBox::information(this, tr("Prompt"), tr("Input content cannot be empty or contain only spaces."));
		return; // Validation failed, function returns
	}

	// Validation 2: Check if line count exceeds maximum limit
	int currentLineCount = m_pTextEdit->document()->lineCount();
	if (currentLineCount > m_maxLines)
	{
		QMessageBox::information(this, tr("Prompt"), QString(tr("Input text cannot exceed %1 lines.")).arg(m_maxLines));
		return; // Validation failed, function returns
	}

	// All validations passed, save text and close dialog
	m_text = currentText;
	accept();

	// Execute command to confirm modification of multi-line text
	emit Mx2dSignalTransfer::getInstance().signalModifyText(MxUtils::gCurrentTab, m_textId, m_text);
}