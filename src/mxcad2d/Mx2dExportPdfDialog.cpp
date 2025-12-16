/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dExportPdfDialog.h"
#include <QtWidgets>
#include <QRegularExpression>


DrawingPreviewWidget::DrawingPreviewWidget(QWidget* parent)
	: QWidget(parent), m_pageSize(841, 1189) // Default A0 portrait
{
	// Set minimum size to prevent window from being too small
	setMinimumSize(200, 200);
	// Set policy to allow free expansion
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void DrawingPreviewWidget::setPageSize(const QSize& size)
{
	if (size.isValid() && m_pageSize != size) {
		m_pageSize = size;
		update(); // Request repaint
	}
}

void DrawingPreviewWidget::resizeEvent(QResizeEvent* event)
{
	// When window size changes, need to repaint to recalculate aspect ratio
	QWidget::resizeEvent(event);
	update();
}

void DrawingPreviewWidget::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event);
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	// Draw gray background
	painter.fillRect(rect(), QColor("#F0F0F0"));

	if (!m_pageSize.isValid() || m_pageSize.width() <= 0 || m_pageSize.height() <= 0) {
		return; // Don't draw for invalid size
	}

	// --- Core logic: calculate rectangle maintaining aspect ratio ---
	QRect widgetRect = rect().adjusted(10, 10, -10, -10); // Leave margins
	QRect drawingRect;

	double pageAspectRatio = (double)m_pageSize.width() / m_pageSize.height();
	double widgetAspectRatio = (double)widgetRect.width() / widgetRect.height();

	if (pageAspectRatio > widgetAspectRatio) {
		// Page is wider than widget, use widget width as base
		int height = widgetRect.width() / pageAspectRatio;
		drawingRect.setRect(widgetRect.left(), widgetRect.top() + (widgetRect.height() - height) / 2, widgetRect.width(), height);
	}
	else {
		// Page is taller than widget (or same ratio), use widget height as base
		int width = widgetRect.height() * pageAspectRatio;
		drawingRect.setRect(widgetRect.left() + (widgetRect.width() - width) / 2, widgetRect.top(), width, widgetRect.height());
	}

	// Draw white paper and black border
	painter.fillRect(drawingRect, Qt::white);
	painter.setPen(Qt::black);
	painter.drawRect(drawingRect);

	// Draw some placeholder content on the paper
	painter.setPen(Qt::darkGray);
	painter.drawText(drawingRect, Qt::AlignCenter, QString(tr("Drawing Preview\n(%1 x %2)")).arg(m_pageSize.width()).arg(m_pageSize.height()));
}



Mx2dExportPdfDialog::Mx2dExportPdfDialog(QWidget* parent)
	: QDialog(parent)
{
	setWindowFlags(Qt::Dialog | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);

	setWindowTitle(tr("Export PDF"));

	setupUi();
	setupConnections();

	// Initialize UI state
	onPaperSizeChanged(m_pPageSizeComboBox->currentIndex());
	setMinimumSize(800, 920);
}

Mx2dExportPdfDialog::~Mx2dExportPdfDialog() {}

void Mx2dExportPdfDialog::setupUi()
{
	// --- Main layout ---
	QHBoxLayout* pMainLayout = new QHBoxLayout(this);

	// --- Left panel ---
	QWidget* pLeftPanel = new QWidget();
	pLeftPanel->setFixedWidth(350);
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

	pLeftLayout->addStretch();

	// Export button
	m_pExportButton = new QPushButton(tr("Export"));
	m_pExportButton->setEnabled(false); // Initially disabled
	pLeftLayout->addWidget(m_pExportButton);

	// --- Right panel ---
	QWidget* pRightPanel = new QWidget();
	QVBoxLayout* pRightLayout = new QVBoxLayout(pRightPanel);
	QHBoxLayout* pTopButtonLayout = new QHBoxLayout();
	m_pFullViewButton = new QPushButton(tr("Full View"));
	m_pSelectRangeButton = new QPushButton(tr("Select Export Range"));
	pTopButtonLayout->addWidget(m_pFullViewButton);
	pTopButtonLayout->addWidget(m_pSelectRangeButton);
	pTopButtonLayout->addStretch();

	m_pPreviewWidget = new DrawingPreviewWidget();

	pRightLayout->addLayout(pTopButtonLayout);
	pRightLayout->addWidget(m_pPreviewWidget);

	// --- Combine left and right panels ---
	pMainLayout->addWidget(pLeftPanel);
	pMainLayout->addWidget(pRightPanel, 1); // Second parameter '1' indicates this widget will take more stretch space
}

void Mx2dExportPdfDialog::setupConnections()
{
	connect(m_pPageSizeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Mx2dExportPdfDialog::onPaperSizeChanged);
	connect(m_pScaleComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Mx2dExportPdfDialog::onScaleChanged);
	connect(m_pLandscapeRadio, &QRadioButton::toggled, this, &Mx2dExportPdfDialog::onOrientationChanged);
	// Portrait button doesn't need connection because its state is mutually exclusive with landscape

	// Update preview when size changes
	connect(m_pWidthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &Mx2dExportPdfDialog::updatePreview);
	connect(m_pHeightSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &Mx2dExportPdfDialog::updatePreview);
}

void Mx2dExportPdfDialog::onPaperSizeChanged(int index)
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

void Mx2dExportPdfDialog::onScaleChanged(int index)
{
	onPaperSizeChanged(m_pPageSizeComboBox->currentIndex());
}

void Mx2dExportPdfDialog::onOrientationChanged()
{
	// Orientation switching only takes effect in non-custom size mode
	if (m_pPageSizeComboBox->currentText() != tr("Custom Size...")) {
		updateUiState();
	}
}

void Mx2dExportPdfDialog::updateUiState()
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

void Mx2dExportPdfDialog::updatePreview()
{
	int width = m_pWidthSpinBox->value();
	int height = m_pHeightSpinBox->value();
	m_pPreviewWidget->setPageSize(QSize(width, height));
}