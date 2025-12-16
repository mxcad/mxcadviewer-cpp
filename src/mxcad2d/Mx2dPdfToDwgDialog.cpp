/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dPdfToDwgDialog.h"

#include <QtWidgets>

Mx2dPdfToDwgDialog::Mx2dPdfToDwgDialog(QWidget* parent)
	: QDialog(parent)
{
	Qt::WindowFlags flags = Qt::Dialog;
	flags |= Qt::CustomizeWindowHint;
	flags |= Qt::WindowTitleHint;
	flags |= Qt::WindowCloseButtonHint;
	setWindowFlags(flags);

	setupUi();
	setupConnections();

	updateButtonStates();
	onConversionOptionChanged();
}

Mx2dPdfToDwgDialog::~Mx2dPdfToDwgDialog()
{
}

void Mx2dPdfToDwgDialog::setupUi()
{
	setWindowTitle(tr("Batch PDF to DWG Conversion"));
	setFixedSize(1000, 800);

	setStyleSheet("QDialog { background-color: #f0f0f0; }");

	QHBoxLayout* pMainLayout = new QHBoxLayout(this);
	pMainLayout->setSpacing(15);
	pMainLayout->setContentsMargins(15, 15, 15, 15);

	QVBoxLayout* pLeftLayout = new QVBoxLayout();
	QLabel* pListHeaderLabel = new QLabel(tr("Add PDF files to convert"));
	m_pFileListWidget = new QListWidget();
	m_pFileListWidget->setAlternatingRowColors(true);
	m_pFileListWidget->setStyleSheet("QListWidget { border: 1px solid #c0c0c0; }");

	pLeftLayout->addWidget(pListHeaderLabel);
	pLeftLayout->addWidget(m_pFileListWidget, 1);

	QVBoxLayout* pRightLayout = new QVBoxLayout();
	pRightLayout->setSpacing(30);
	m_pAddButton = new QPushButton(tr("Add"));
	m_pDeleteButton = new QPushButton(tr("Delete"));
	m_pClearButton = new QPushButton(tr("Clear"));
	m_pConvertButton = new QPushButton(tr("Convert"));
	m_pAddButton->setFixedWidth(100);
	m_pDeleteButton->setFixedWidth(100);
	m_pClearButton->setFixedWidth(100);
	m_pConvertButton->setFixedWidth(100);

	const QString blueButtonStyle = tr("QPushButton { background-color: #337AB7; color: white; border: none; padding: 8px; border-radius: 3px; } QPushButton:disabled { background-color: #A0B8D0; }");
	const QString grayButtonStyle = tr("QPushButton { background-color: #FFFFFF; border: 1px solid #ADADAD; padding: 8px; border-radius: 3px; } QPushButton:disabled { background-color: #F0F0F0; color: #a0a0a0; }");
	m_pAddButton->setStyleSheet(blueButtonStyle);
	m_pConvertButton->setStyleSheet(blueButtonStyle);
	m_pDeleteButton->setStyleSheet(grayButtonStyle);
	m_pClearButton->setStyleSheet(grayButtonStyle);

	pRightLayout->addStretch(1);
	pRightLayout->addWidget(m_pAddButton);
	pRightLayout->addWidget(m_pDeleteButton);
	pRightLayout->addWidget(m_pClearButton);
	pRightLayout->addSpacing(30);
	pRightLayout->addWidget(m_pConvertButton);
	pRightLayout->addStretch(2);

	m_pPageByPageRadio = new QRadioButton(tr("Convert page by page: Save each page as a separate DWG file"));
	m_pPageByPageRadio->setChecked(true);

	QHBoxLayout* pMergeLayout = new QHBoxLayout();
	m_pMergeRadio = new QRadioButton(tr("Merge conversion: When converting each PDF file, merge every"));
	m_pMergeCountSpinBox = new QSpinBox();
	m_pMergeCountSpinBox->setRange(1, 999);
	m_pMergeCountSpinBox->setValue(10);
	QLabel* pMergeSuffixLabel = new QLabel(tr("pages into one DWG file"));
	pMergeLayout->addWidget(m_pMergeRadio);
	pMergeLayout->addWidget(m_pMergeCountSpinBox);
	pMergeLayout->addWidget(pMergeSuffixLabel);
	pMergeLayout->addStretch();

	QHBoxLayout* pSaveLayout = new QHBoxLayout();
	QLabel* pSaveLabel = new QLabel(tr("Save location:"));
	m_pSaveLocationEdit = new QLineEdit("C:\\Users\\MxDev2\\Desktop\\convert_result");
	m_pBrowseButton = new QPushButton("...");
	pSaveLayout->addWidget(pSaveLabel);
	pSaveLayout->addWidget(m_pSaveLocationEdit, 1);
	pSaveLayout->addWidget(m_pBrowseButton);

	pLeftLayout->addSpacing(10);
	pLeftLayout->addWidget(m_pPageByPageRadio);
	pLeftLayout->addLayout(pMergeLayout);
	pLeftLayout->addLayout(pSaveLayout);

	pMainLayout->addLayout(pLeftLayout, 1);
	pMainLayout->addLayout(pRightLayout);
}

void Mx2dPdfToDwgDialog::setupConnections()
{
	connect(m_pAddButton, &QPushButton::clicked, this, &Mx2dPdfToDwgDialog::onAddClicked);
	connect(m_pDeleteButton, &QPushButton::clicked, this, &Mx2dPdfToDwgDialog::onDeleteClicked);
	connect(m_pClearButton, &QPushButton::clicked, this, &Mx2dPdfToDwgDialog::onClearClicked);
	connect(m_pConvertButton, &QPushButton::clicked, this, &Mx2dPdfToDwgDialog::onConvertClicked);
	connect(m_pBrowseButton, &QPushButton::clicked, this, &Mx2dPdfToDwgDialog::onBrowseClicked);
	connect(m_pFileListWidget, &QListWidget::itemSelectionChanged, this, &Mx2dPdfToDwgDialog::onSelectionChanged);
	connect(m_pPageByPageRadio, &QRadioButton::toggled, this, &Mx2dPdfToDwgDialog::onConversionOptionChanged);
	// m_pMergeRadio's toggled signal will also trigger onConversionOptionChanged
}

void Mx2dPdfToDwgDialog::updateButtonStates()
{
	bool hasItems = m_pFileListWidget->count() > 0;
	bool hasSelection = !m_pFileListWidget->selectedItems().isEmpty();

	m_pDeleteButton->setEnabled(hasItems && hasSelection);
	m_pClearButton->setEnabled(hasItems);
	m_pConvertButton->setEnabled(hasItems);
}

void Mx2dPdfToDwgDialog::onAddClicked()
{
	QStringList files = QFileDialog::getOpenFileNames(
		this,
		tr("Select PDF files to convert"),
		QDir::homePath(),
		tr("PDF files (*.pdf);;All files (*.*)")
	);

	if (!files.isEmpty()) {
		for (const QString& filePath : files) {
			int pageCount = QRandomGenerator::global()->bounded(5, 20);
			QString displayText = QString(tr("(%1 pages) %2")).arg(pageCount).arg(QDir::toNativeSeparators(filePath));
			m_pFileListWidget->addItem(displayText);
		}
		updateButtonStates();
	}
}

void Mx2dPdfToDwgDialog::onDeleteClicked()
{
	qDeleteAll(m_pFileListWidget->selectedItems());
	updateButtonStates();
}

void Mx2dPdfToDwgDialog::onClearClicked()
{
	m_pFileListWidget->clear();
	updateButtonStates();
}

void Mx2dPdfToDwgDialog::onConvertClicked()
{
	QMessageBox::information(this, tr("Prompt"), tr("Conversion function not implemented yet!"));
}

void Mx2dPdfToDwgDialog::onBrowseClicked()
{
	QString dir = QFileDialog::getExistingDirectory(
		this,
		tr("Select save location"),
		m_pSaveLocationEdit->text(),
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
	);

	if (!dir.isEmpty()) {
		m_pSaveLocationEdit->setText(QDir::toNativeSeparators(dir));
	}
}

void Mx2dPdfToDwgDialog::onConversionOptionChanged()
{
	m_pMergeCountSpinBox->setEnabled(m_pMergeRadio->isChecked());
}

void Mx2dPdfToDwgDialog::onSelectionChanged()
{
	updateButtonStates();
}