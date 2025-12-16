/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dBatchExportPdfDialog.h"
#include <QtWidgets>
#include <QRegularExpression>
#include "Mx2dBatchExportPdfPreviewWidget.h"
#include <QListWidget>
#include "Mx2dFrameRegionWidget.h"



Mx2dBatchExportPdfDialog::Mx2dBatchExportPdfDialog(QWidget* parent)
	: QDialog(parent)
{
	setWindowFlags(Qt::Dialog | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);

	setWindowTitle(tr("Batch Export PDF"));

	setupUi();
	setupConnections();

	// Initialize UI state
	onPaperSizeChanged(m_pPageSizeComboBox->currentIndex());
	//setMinimumSize(800, 920);
}

Mx2dBatchExportPdfDialog::~Mx2dBatchExportPdfDialog() {}

void Mx2dBatchExportPdfDialog::setupUi()
{
	QHBoxLayout* pMainLayout = new QHBoxLayout(this);

	QWidget* pLeftPanel = new QWidget();
	//pLeftPanel->setFixedWidth(450);
	QVBoxLayout* pLeftLayout = new QVBoxLayout(pLeftPanel);

	// Page size
	m_pPageSizeGroup = new QGroupBox(tr("Page Size"));
	QVBoxLayout* pPageSizeLayout = new QVBoxLayout(m_pPageSizeGroup);
	QHBoxLayout* pStdPageSizeLayout = new QHBoxLayout();
	QHBoxLayout* pPageWidthLayout = new QHBoxLayout();
	QHBoxLayout* pPageHeightLayout = new QHBoxLayout();

	m_pPageSizeComboBox = new QComboBox();
	m_pPageSizeComboBox->setStyleSheet("QComboBox{ combobox-popup: 0; }");
	m_pPageSizeComboBox->addItems({
		tr("A0 (841 x 1189 mm)"),
		tr("A1 (594 x 841 mm)"),
		tr("A2 (420 x 594 mm)"),
		tr("A3 (297 x 420 mm)"),
		tr("A4 (210 x 297 mm)"),
		tr("Custom Size...")
		});
	m_pScaleComboBox = new QComboBox();
	m_pScaleComboBox->setStyleSheet("QComboBox{ combobox-popup: 0; }");
	m_pScaleComboBox->addItems({ tr("None"), tr("1/8"), tr("1/4"), tr("3/8"), tr("1/2"), tr("5/8"), tr("3/4"), tr("7/8"), tr("1"), tr("5/4"), tr("3/2"), tr("7/4"), tr("2"), tr("3"), tr("4") });

	m_pWidthSpinBox = new QSpinBox();
	m_pWidthSpinBox->setRange(1, 10000);
	m_pWidthSpinBox->setSuffix(tr(" mm"));
	m_pHeightSpinBox = new QSpinBox();
	m_pHeightSpinBox->setRange(1, 10000);
	m_pHeightSpinBox->setSuffix(tr(" mm"));

	pStdPageSizeLayout->addWidget(m_pPageSizeComboBox);
	pStdPageSizeLayout->addWidget(new QLabel("+"));
	pStdPageSizeLayout->addWidget(m_pScaleComboBox);
	pPageWidthLayout->addWidget(new QLabel(tr("Width (mm)")));
	pPageWidthLayout->addWidget(m_pWidthSpinBox, 1);
	pPageHeightLayout->addWidget(new QLabel(tr("Height (mm)")));
	pPageHeightLayout->addWidget(m_pHeightSpinBox, 1);
	pPageSizeLayout->addLayout(pStdPageSizeLayout);
	pPageSizeLayout->addLayout(pPageWidthLayout);
	pPageSizeLayout->addLayout(pPageHeightLayout);
	pLeftLayout->addWidget(m_pPageSizeGroup);

	// Page orientation
	m_pOrientationGroup = new QGroupBox(tr("Page Orientation"));
	QHBoxLayout* pOrientationLayout = new QHBoxLayout(m_pOrientationGroup);
	m_pLandscapeRadio = new QRadioButton(tr("Landscape"));
	m_pPortraitRadio = new QRadioButton(tr("Portrait"));
	m_pPortraitRadio->setChecked(true);
	pOrientationLayout->addWidget(m_pLandscapeRadio);
	pOrientationLayout->addWidget(m_pPortraitRadio);
	pLeftLayout->addWidget(m_pOrientationGroup);

	// PDF content color
	m_pColorGroup = new QGroupBox(tr("PDF Content Color"));
	QHBoxLayout* pColorLayout = new QHBoxLayout(m_pColorGroup);
	m_pColorRadio = new QRadioButton(tr("Color"));
	m_pBlackWhiteRadio = new QRadioButton(tr("Black"));
	m_pBlackWhiteRadio->setChecked(true);
	pColorLayout->addWidget(m_pColorRadio);
	pColorLayout->addWidget(m_pBlackWhiteRadio);
	pLeftLayout->addWidget(m_pColorGroup);

	// Other options
	m_pBoldGroup = new QGroupBox(tr("Bold"));
	QVBoxLayout* pBoldLayout = new QVBoxLayout(m_pBoldGroup);
	m_pBoldCheckBox = new QCheckBox(tr("Bold PDF lines"));
	pBoldLayout->addWidget(m_pBoldCheckBox);
	pLeftLayout->addWidget(m_pBoldGroup);

	m_pAnnotationGroup = new QGroupBox(tr("Annotation"));
	QVBoxLayout* pAnnotationLayout = new QVBoxLayout(m_pAnnotationGroup);
	m_pAnnotationCheckBox = new QCheckBox(tr("Export annotations"));
	pAnnotationLayout->addWidget(m_pAnnotationCheckBox);
	pLeftLayout->addWidget(m_pAnnotationGroup);

	m_pLayerGroup = new QGroupBox(tr("Layer"));
	QVBoxLayout* pLayerLayout = new QVBoxLayout(m_pLayerGroup);
	m_pLayerCheckBox = new QCheckBox(tr("Export layers"));
	m_pLayerCheckBox->setChecked(true);
	pLayerLayout->addWidget(m_pLayerCheckBox);
	pLeftLayout->addWidget(m_pLayerGroup);

	m_pMergeExportGroup = new QGroupBox(tr("Merge Export"));
	QVBoxLayout* pMergeExportLayout = new QVBoxLayout(m_pMergeExportGroup);
	m_pAllFrameOnePDF = new QRadioButton(tr("Export all as one PDF file, each frame as one page"));
	m_pOneFrameOnePDF = new QRadioButton(tr("Export each frame as a separate PDF file"));
	m_pAllFrameOnePDF->setChecked(true);
	pMergeExportLayout->addWidget(m_pAllFrameOnePDF);
	pMergeExportLayout->addWidget(m_pOneFrameOnePDF);
	pLeftLayout->addWidget(m_pMergeExportGroup);

	pLeftLayout->addStretch();

	// Export button
	m_pExportButton = new QPushButton(tr("Export"));
	m_pExportButton->setEnabled(false); // Initially disabled
	pLeftLayout->addWidget(m_pExportButton);
	// --- Middle preview area ---

	m_pMidPanel = new QListWidget();
	m_pMidPanel->setStyleSheet(R"(
		QListWidget::item:selected {
		    background-color: #5AAFFF;
		    border: 1px solid #0078D7;
		}
		QListWidget::item:hover {
		    background-color: #B0D9FF;
		    border: 1px solid #0078D7;
		}
	)");
	// Add two sample items
	for (int i = 0; i < 2; ++i) {
		QListWidgetItem* pItem = new QListWidgetItem(m_pMidPanel);

		QPixmap placeholderImage(200, 150);
		placeholderImage.fill(Qt::white);
		QPainter painter(&placeholderImage);
		painter.setPen(Qt::gray);
		painter.drawText(placeholderImage.rect(), Qt::AlignCenter, QString(tr("Preview %1").arg(i + 1)));

		Mx2dBatchExportPdfPreviewWidget* pCustomWidget = new Mx2dBatchExportPdfPreviewWidget(placeholderImage, QString(tr("Frame %1").arg(i + 1)));

		QWidget* pWrapperWidget = new QWidget();

		QHBoxLayout* pWrapperLayout = new QHBoxLayout(pWrapperWidget);

		pWrapperLayout->addStretch();
		pWrapperLayout->addWidget(pCustomWidget);
		pWrapperLayout->addStretch();

		pWrapperLayout->setContentsMargins(0, 5, 0, 5);
		pItem->setSizeHint(pCustomWidget->sizeHint());

		m_pMidPanel->setItemWidget(pItem, pWrapperWidget);
	}


	// --- Right panel ---
	QWidget* pRightPanel = new QWidget();
	QVBoxLayout* pRightLayout = new QVBoxLayout(pRightPanel);
	pRightLayout->setContentsMargins(5, 0, 5, 0);
	QHBoxLayout* pTopButtonLayout = new QHBoxLayout();
	m_pAddFramButton = new QPushButton(tr("Add Frame"));
	m_pRemoveFrameButton = new QPushButton(tr("Remove Frame"));
	m_pRecognizeFrameButton = new QPushButton(tr("Recognize Frame"));
	m_pFullViewButton = new QPushButton(tr("Full View"));
	m_pShowSelOnlyCheckBox = new QCheckBox(tr("Show selected frame only"));

	QMenu* pRemoveFrameMenu = new QMenu();
	pRemoveFrameMenu->addAction(tr("Select and remove"));
	m_pRemoveAllFrameAction = new QAction(tr("Remove all frames"));
	pRemoveFrameMenu->addAction(m_pRemoveAllFrameAction);
	QMenu* pRecognizeFrameMenu = new QMenu();
	pRecognizeFrameMenu->addAction(tr("Select and recognize"));
	pRecognizeFrameMenu->addAction(tr("Recognize all"));

	m_pRemoveFrameButton->setMenu(pRemoveFrameMenu);
	m_pRecognizeFrameButton->setMenu(pRecognizeFrameMenu);

	pTopButtonLayout->addWidget(m_pAddFramButton);
	pTopButtonLayout->addWidget(m_pRemoveFrameButton);
	pTopButtonLayout->addWidget(m_pRecognizeFrameButton);
	pTopButtonLayout->addWidget(m_pFullViewButton);
	pTopButtonLayout->addWidget(m_pShowSelOnlyCheckBox);

	pTopButtonLayout->addStretch();


	pRightLayout->addLayout(pTopButtonLayout);
	QFrame* pBorder = new QFrame();
	pBorder->setStyleSheet("background-color: white; border: 1px solid #333;");
	QHBoxLayout* pBorderLayout = new QHBoxLayout(pBorder);
	pBorderLayout->setContentsMargins(0, 0, 0, 0);
	m_pViewWidget = new Mx2dFrameRegionWidget();
	pBorderLayout->addWidget(m_pViewWidget);
	//m_pViewWidget->setStyleSheet("background-color: white; border: 1px solid #333;");
	pRightLayout->addWidget(pBorder);

	pMainLayout->addWidget(pLeftPanel);
	pMainLayout->addWidget(m_pMidPanel);
	pMainLayout->addWidget(pRightPanel, 1);
}

void Mx2dBatchExportPdfDialog::setupConnections()
{
	connect(m_pPageSizeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Mx2dBatchExportPdfDialog::onPaperSizeChanged);
	connect(m_pScaleComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Mx2dBatchExportPdfDialog::onScaleChanged);
	connect(m_pLandscapeRadio, &QRadioButton::toggled, this, &Mx2dBatchExportPdfDialog::onOrientationChanged);
	// Portrait button doesn't need connection because its state is mutually exclusive with landscape

	// Update preview when size changes
	connect(m_pWidthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &Mx2dBatchExportPdfDialog::updatePreview);
	connect(m_pHeightSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &Mx2dBatchExportPdfDialog::updatePreview);
	connect(m_pAddFramButton, &QPushButton::clicked, m_pViewWidget, &Mx2dFrameRegionWidget::startSelectionMode);
	connect(m_pRemoveAllFrameAction, &QAction::triggered, m_pViewWidget, &Mx2dFrameRegionWidget::clearAll);
}

void Mx2dBatchExportPdfDialog::onPaperSizeChanged(int index)
{
	QString selectedText = m_pPageSizeComboBox->itemText(index);

	if (selectedText == tr("Custom Size...")) {
		m_basePageSize = QSize(0, 0); // Invalidate base size
	}
	else {
		// Use regular expression to extract numbers from "A0 (841 x 1189 mm)"
		QRegularExpression re(tr("\\((\\d+)\\s*x\\s*(\\d+).*\\)"));
		QRegularExpressionMatch match = re.match(selectedText);
		if (match.hasMatch()) {
			int w = match.captured(1).toInt();
			int h = match.captured(2).toInt();
			int selectedScaleIndex = m_pScaleComboBox->currentIndex();
			if (selectedScaleIndex != 0)
			{
				/*   0        1        2        3        4        5        6        7      8        9        10       11     12      13     14 */
				/*tr("None"), tr("1/8"), tr("1/4"), tr("3/8"), tr("1/2"), tr("5/8"), tr("3/4"), tr("7/8"), tr("1"), tr("5/4"), tr("3/2"), tr("7/4"), tr("2"), tr("3"), tr("4") */
				switch (selectedScaleIndex)
				{
				case 1: h += h * (1 / 8.); break;	// "1/8"
				case 2: h += h * (1 / 4.); break;	// "1/4"
				case 3: h += h * (3 / 8.); break;	// "3/8"
				case 4: h += h * (1 / 2.); break;	// "1/2"
				case 5: h += h * (5 / 8.); break;	// "5/8"
				case 6: h += h * (3 / 4.); break;	// "3/4"
				case 7: h += h * (7 / 8.); break;	// "7/8"
				case 8: h += h * 1; break;			// "1"
				case 9: h += h * (5 / 4.); break;	// "5/4"
				case 10: h += h * (3 / 2.); break;	// "3/2"
				case 11: h += h * (7 / 4.); break;	// "7/4"
				case 12: h += h * 2; break;			// "2"
				case 13: h += h * 3; break;			// "3"
				case 14: h += h * 4; break;			// "4"
				}
			}
			m_basePageSize.setWidth(w);
			m_basePageSize.setHeight(h);
		}
	}

	updateUiState();
}

void Mx2dBatchExportPdfDialog::onScaleChanged(int index)
{
	onPaperSizeChanged(m_pPageSizeComboBox->currentIndex());
}

void Mx2dBatchExportPdfDialog::onOrientationChanged()
{
	// Orientation switching only takes effect in non-custom size mode
	if (m_pPageSizeComboBox->currentText() != tr("Custom Size...")) {
		updateUiState();
	}
}

void Mx2dBatchExportPdfDialog::updateUiState()
{
	bool isCustom = (m_pPageSizeComboBox->currentText() == tr("Custom Size..."));

	m_pWidthSpinBox->setEnabled(isCustom);
	m_pHeightSpinBox->setEnabled(isCustom);
	m_pScaleComboBox->setDisabled(isCustom);
	m_pOrientationGroup->setDisabled(isCustom);

	if (!isCustom) {
		// Non-custom size, set spinbox values based on orientation
		bool isPortrait = m_pPortraitRadio->isChecked();
		int w = m_basePageSize.width();
		int h = m_basePageSize.height();

		// Prevent signal loop triggering
		QSignalBlocker wBlocker(m_pWidthSpinBox);
		QSignalBlocker hBlocker(m_pHeightSpinBox);

		m_pWidthSpinBox->setValue(isPortrait ? w : h);
		m_pHeightSpinBox->setValue(isPortrait ? h : w);
	}

	updatePreview();
}

void Mx2dBatchExportPdfDialog::updatePreview()
{
	int width = m_pWidthSpinBox->value();
	int height = m_pHeightSpinBox->value();
}