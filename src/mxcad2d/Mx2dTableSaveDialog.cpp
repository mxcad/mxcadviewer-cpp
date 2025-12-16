/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dTableSaveDialog.h"

#include <QGroupBox>
#include <QRadioButton>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QDebug>

Mx2dTableSaveDialog::Mx2dTableSaveDialog(QWidget* parent)
	: QDialog(parent)
{
	Qt::WindowFlags flags = Qt::Dialog;
	flags |= Qt::CustomizeWindowHint;
	flags |= Qt::WindowTitleHint;
	flags |= Qt::WindowCloseButtonHint;
	setWindowFlags(flags);
	setWindowTitle(tr("Save Results"));
	setFixedSize(450, 400);

	setupUi();
	setupConnections();
}

Mx2dTableSaveDialog::~Mx2dTableSaveDialog()
{
	qDebug() << "Mx2dTableSaveDialog destroyed.";
}

void Mx2dTableSaveDialog::setupUi()
{
	m_pSaveModeGroupBox = new QGroupBox(tr("Save Method"), this);
	m_pNewFileRadio = new QRadioButton(tr("Save to new Excel file"), this);
	m_pExistingFileRadio = new QRadioButton(tr("Save to new sheet in existing Excel file"), this);
	m_pNewFileRadio->setChecked(true);

	QVBoxLayout* saveModeLayout = new QVBoxLayout;
	saveModeLayout->addWidget(m_pNewFileRadio);
	saveModeLayout->addWidget(m_pExistingFileRadio);
	m_pSaveModeGroupBox->setLayout(saveModeLayout);

	m_pSavePathGroupBox = new QGroupBox(tr("Save Path"), this);
	m_pPathLineEdit = new QLineEdit("C:\\Users\\MxDev2\\Desktop\\table_extraction.xls", this);
	m_pBrowseButton = new QPushButton("...", this);
	m_pBrowseButton->setFixedSize(30, m_pPathLineEdit->sizeHint().height());

	QHBoxLayout* savePathLayout = new QHBoxLayout;
	savePathLayout->addWidget(m_pPathLineEdit);
	savePathLayout->addWidget(m_pBrowseButton);
	m_pSavePathGroupBox->setLayout(savePathLayout);

	m_pOpenAfterSaveCheck = new QCheckBox(tr("Open Excel file after saving"), this);
	m_pOpenAfterSaveCheck->setChecked(true);

	m_pSaveButton = new QPushButton(tr("Save"), this);
	m_pCancelButton = new QPushButton(tr("Cancel"), this);
	m_pSaveButton->setDefault(true);

	QHBoxLayout* buttonLayout = new QHBoxLayout;
	buttonLayout->addStretch();
	buttonLayout->addWidget(m_pSaveButton);
	buttonLayout->addWidget(m_pCancelButton);

	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(m_pSaveModeGroupBox);
	mainLayout->addWidget(m_pSavePathGroupBox);
	mainLayout->addWidget(m_pOpenAfterSaveCheck);
	mainLayout->addStretch();
	mainLayout->addLayout(buttonLayout);

	setLayout(mainLayout);
}

void Mx2dTableSaveDialog::setupConnections()
{
	connect(m_pBrowseButton, &QPushButton::clicked, this, &Mx2dTableSaveDialog::onBrowseButtonClicked);
	connect(m_pSaveButton, &QPushButton::clicked, this, &QDialog::accept);
	connect(m_pCancelButton, &QPushButton::clicked, this, &QDialog::reject);

	connect(m_pNewFileRadio, &QRadioButton::toggled, this, &Mx2dTableSaveDialog::onSaveModeChanged);
}

void Mx2dTableSaveDialog::onBrowseButtonClicked()
{
	QString selectedPath;
	const QString excelFilter = tr("Excel files (*.xls *.xlsx);;All files (*.*)");

	if (m_pNewFileRadio->isChecked()) {
		selectedPath = QFileDialog::getSaveFileName(
			this,
			tr("Save to new file"),
			m_pPathLineEdit->text(),
			excelFilter
		);
	}
	else {
		selectedPath = QFileDialog::getOpenFileName(
			this,
			tr("Select an existing Excel file"),
			m_pPathLineEdit->text(),
			excelFilter
		);
	}

	if (!selectedPath.isEmpty()) {
		m_pPathLineEdit->setText(selectedPath);
	}
}

void Mx2dTableSaveDialog::onSaveModeChanged()
{
	// This function can be used to perform specific operations when switching modes
	// For example, you can change the default filename or path based on the mode
	qDebug() << "Save mode changed. New file mode is:" << m_pNewFileRadio->isChecked();
}

Mx2dTableSaveDialog::SaveMode Mx2dTableSaveDialog::getSaveMode() const
{
	if (m_pNewFileRadio->isChecked()) {
		return SaveMode::SaveToNewFile;
	}
	else {
		return SaveMode::SaveToExistingFile;
	}
}

QString Mx2dTableSaveDialog::getFilePath() const
{
	return m_pPathLineEdit->text();
}

bool Mx2dTableSaveDialog::shouldOpenFileAfterSave() const
{
	return m_pOpenAfterSaveCheck->isChecked();
}