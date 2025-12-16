/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dSplitExportDialog.h"

#include <QtWidgets>
#include <QMessageBox>

Mx2dSplitExportDialog::Mx2dSplitExportDialog(QWidget* parent)
	: QDialog(parent)
{
	Qt::WindowFlags flags = Qt::Dialog;
	flags |= Qt::CustomizeWindowHint;
	flags |= Qt::WindowTitleHint;
	flags |= Qt::WindowCloseButtonHint;
	setWindowFlags(flags);

	setWindowTitle(tr("CAD Drawing Split Export"));

	setupUi();

	setFixedSize(530, 270);
}

Mx2dSplitExportDialog::~Mx2dSplitExportDialog()
{
}

void Mx2dSplitExportDialog::setupUi()
{
	QVBoxLayout* pMainLayout = new QVBoxLayout(this);
	pMainLayout->setSpacing(20);
	pMainLayout->setContentsMargins(15, 15, 15, 15);

	m_pInstructionsLabel = new QLabel(
		tr("First click the \"Select Area\" button, drag a rectangle to select the drawing area to export, "
			"then click the \"Export Drawing\" button to save. You can repeat this operation to export different "
			"parts of the drawing separately.")
	);
	m_pInstructionsLabel->setWordWrap(true);
	pMainLayout->addWidget(m_pInstructionsLabel);

	QHBoxLayout* pAreaTypeLayout = new QHBoxLayout();
	m_pAreaTypeLabel = new QLabel(tr("Split Area:"));
	m_pRectAreaRadio = new QRadioButton(tr("Rectangular Area"));
	m_pIrregularAreaRadio = new QRadioButton(tr("Irregular Area"));

	m_pRectAreaRadio->setChecked(true);

	pAreaTypeLayout->addWidget(m_pAreaTypeLabel);
	pAreaTypeLayout->addWidget(m_pRectAreaRadio);
	pAreaTypeLayout->addWidget(m_pIrregularAreaRadio);
	pAreaTypeLayout->addStretch();

	pMainLayout->addLayout(pAreaTypeLayout);

	QHBoxLayout* pButtonLayout = new QHBoxLayout();
	m_pSelectAreaButton = new QPushButton(tr("Select Area"));
	m_pExportButton = new QPushButton(tr("Export Drawing"));

	m_pSelectAreaButton->setStyleSheet(
		"QPushButton { "
		"  border: 1px solid #0078D7; "
		"  background-color: white; "
		"  padding: 5px 15px; "
		"  border-radius: 2px;"
		"} "
		"QPushButton:hover { background-color: #F0F8FF; } "
		"QPushButton:pressed { background-color: #E6F3FF; }"
	);
	m_pSelectAreaButton->setCursor(Qt::PointingHandCursor);

	m_pExportButton->setEnabled(false);
	m_pExportButton->setMinimumWidth(m_pSelectAreaButton->sizeHint().width());

	pButtonLayout->addStretch();
	pButtonLayout->addWidget(m_pSelectAreaButton);
	pButtonLayout->addSpacing(10);
	pButtonLayout->addWidget(m_pExportButton);
	pButtonLayout->addStretch();

	pMainLayout->addLayout(pButtonLayout);

	connect(m_pSelectAreaButton, &QPushButton::clicked, this, [this]() {
		// Simulate area selection
		QMessageBox::StandardButton btn = QMessageBox::question(this, tr("Prompt"),
			tr("Simulate area selection. Click Yes to select area, No to cancel"));
		if (btn == QMessageBox::Yes)
		{
			m_pExportButton->setEnabled(true);
		}
		else {
			m_pExportButton->setEnabled(false);
		}
		});

	connect(m_pRectAreaRadio, &QRadioButton::toggled, this, [this]() {
		m_pExportButton->setEnabled(false);
		});
}