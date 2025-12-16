/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dPrintHintDialog.h"
#include <QtWidgets>
#include <QPrinter>
#include <QPrintDialog>

Mx2dPrintHintDialog::Mx2dPrintHintDialog(QWidget* parent)
	: QDialog(parent)
{
	Qt::WindowFlags flags = Qt::Dialog;
	flags |= Qt::CustomizeWindowHint;
	flags |= Qt::WindowTitleHint;
	flags |= Qt::WindowCloseButtonHint;
	setWindowFlags(flags);
	setWindowTitle(tr("Hint"));
	setupUi();

	connect(m_pOpenSettingsButton, &QPushButton::clicked, this, &Mx2dPrintHintDialog::onOpenPrintSettingsClicked);

	setFixedSize(480, 320);
}

Mx2dPrintHintDialog::~Mx2dPrintHintDialog()
{
}

void Mx2dPrintHintDialog::onOpenPrintSettingsClicked()
{
	QPrinter printer;

	QPrintDialog printDialog(&printer, this);
	printDialog.setWindowTitle(tr("Print Settings"));

	printDialog.exec();

	// TODO: Perform print settings configuration.

	this->accept();
}

void Mx2dPrintHintDialog::setupUi()
{
	QVBoxLayout* pMainLayout = new QVBoxLayout(this);
	pMainLayout->setContentsMargins(0, 0, 0, 0);
	pMainLayout->setSpacing(0);

	QWidget* pContentWidget = new QWidget();
	pContentWidget->setStyleSheet("background-color: white;");
	QHBoxLayout* pContentLayout = new QHBoxLayout(pContentWidget);
	pContentLayout->setContentsMargins(15, 20, 15, 20);
	pContentLayout->setSpacing(15);

	m_pIconLabel = new QLabel();
	QPixmap infoPixmap = style()->standardPixmap(QStyle::SP_MessageBoxInformation);
	m_pIconLabel->setPixmap(infoPixmap.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation));
	m_pIconLabel->setAlignment(Qt::AlignTop);

	m_pTextLabel = new QLabel();
	QString text = tr("<b>Print Steps:</b><br><br>"
		"1: Print Settings - Select printer and configure settings, click \"OK\" button to enter print interface.<br><br>"
		"2: Select Print Range - In the print interface toolbar, you can click \"Select Print Area\" button to choose the area you want to print");
	m_pTextLabel->setText(text);
	m_pTextLabel->setWordWrap(true); // Auto-wrap text
	m_pTextLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop); // Top-left alignment
	m_pTextLabel->setTextFormat(Qt::RichText); // Use rich text to support <b> and <br> tags

	pContentLayout->addWidget(m_pIconLabel);
	pContentLayout->addWidget(m_pTextLabel, 1);

	pMainLayout->addWidget(pContentWidget, 1);

	QWidget* pBottomBar = new QWidget();
	pBottomBar->setFixedHeight(80);
	pBottomBar->setAutoFillBackground(true);

	QPalette pal = pBottomBar->palette();
	pal.setColor(QPalette::Window, QColor(240, 240, 240)); // Light gray
	pBottomBar->setPalette(pal);

	QHBoxLayout* pBottomLayout = new QHBoxLayout(pBottomBar);
	pBottomLayout->setContentsMargins(12, 12, 12, 12);

	m_pOpenSettingsButton = new QPushButton(tr("Open Print Settings"));
	m_pOpenSettingsButton->setMinimumSize(120, 28);

	m_pOpenSettingsButton->setStyleSheet(
		R"(QPushButton {
				border: 1px solid #0078D7;
				border-radius: 2px;
				color: #0078D7;
				background-color: #F0F0F0;
				padding: 5px;
			}
			QPushButton:hover {
				background-color: #E0E0E0;
			}
			QPushButton:pressed {
				background-color: #D0D0D0;
				border-color: #005A9E;
			})"
	);

	pBottomLayout->addStretch(1);
	pBottomLayout->addWidget(m_pOpenSettingsButton);

	pMainLayout->addWidget(pBottomBar);
}