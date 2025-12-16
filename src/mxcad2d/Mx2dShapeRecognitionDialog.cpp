/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dShapeRecognitionDialog.h"

#include <QtWidgets>


ShapePreviewWidget::ShapePreviewWidget(QWidget* parent)
	: QWidget(parent)
{
	setFixedSize(220, 220);
	setAutoFillBackground(false);
}

void ShapePreviewWidget::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event);
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	painter.fillRect(rect(), Qt::black);
	painter.setPen(QPen(Qt::white, 1.5));

	int margin = 20;
	QRect drawingRect = rect().adjusted(margin, margin, -margin, -margin);

	QPainterPath path;
	path.moveTo(drawingRect.bottomLeft());
	path.lineTo(drawingRect.topLeft());
	path.arcTo(drawingRect, 180.0, -180.0);
	path.lineTo(drawingRect.bottomRight());

	painter.drawPath(path);
}

Mx2dShapeRecognitionDialog::Mx2dShapeRecognitionDialog(QWidget* parent)
	: QDialog(parent)
{
	Qt::WindowFlags flags = Qt::Dialog;
	flags |= Qt::CustomizeWindowHint;
	flags |= Qt::WindowTitleHint;
	flags |= Qt::WindowCloseButtonHint;
	setWindowFlags(flags);

	setWindowTitle(tr("Shape Recognition"));
	setupUi();
	setupConnections();
	onScopeChanged();
}

Mx2dShapeRecognitionDialog::~Mx2dShapeRecognitionDialog()
{
}

void Mx2dShapeRecognitionDialog::setupUi()
{
	QVBoxLayout* pMainLayout = new QVBoxLayout(this);
	pMainLayout->setSpacing(15);
	pMainLayout->setContentsMargins(15, 15, 15, 15);

	m_pShapeGroup = new QGroupBox(tr("Shape"));
	QVBoxLayout* pShapeLayout = new QVBoxLayout(m_pShapeGroup);

	QHBoxLayout* pNameLayout = new QHBoxLayout();
	m_pNameLabel = new QLabel(tr("Enter shape name:"));
	m_pNameLineEdit = new QLineEdit();
	pNameLayout->addWidget(m_pNameLabel);
	pNameLayout->addWidget(m_pNameLineEdit);

	m_pNameHintLabel = new QLabel(tr("Shape name can be used to view the shape later"));
	m_pNameHintLabel->setStyleSheet("color: grey;");

	m_pShapePreview = new ShapePreviewWidget();

	m_pReselectButton = new QPushButton(tr("Reselect Shape"));

	pShapeLayout->addLayout(pNameLayout);
	pShapeLayout->addWidget(m_pNameHintLabel, 0, Qt::AlignRight);
	pShapeLayout->addSpacing(10);
	pShapeLayout->addWidget(m_pShapePreview, 0, Qt::AlignCenter);
	pShapeLayout->addSpacing(10);
	pShapeLayout->addWidget(m_pReselectButton, 0, Qt::AlignCenter);

	pMainLayout->addWidget(m_pShapeGroup);

	m_pScopeGroup = new QGroupBox(tr("Search Scope"));
	QVBoxLayout* pScopeLayout = new QVBoxLayout(m_pScopeGroup);

	m_pEntireDrawingRadio = new QRadioButton(tr("Entire drawing"));
	m_pEntireDrawingRadio->setChecked(true);
	pScopeLayout->addWidget(m_pEntireDrawingRadio);

	QHBoxLayout* pAreaSearchLayout = new QHBoxLayout();
	m_pRectangularSearchRadio = new QRadioButton(tr("Rectangular area search"));
	m_pIrregularSearchRadio = new QRadioButton(tr("Irregular area search"));
	m_pSelectAreaButton = new QPushButton(tr("Select Area"));
	pAreaSearchLayout->addWidget(m_pRectangularSearchRadio);
	pAreaSearchLayout->addWidget(m_pIrregularSearchRadio);
	pAreaSearchLayout->addSpacing(20);
	pAreaSearchLayout->addWidget(m_pSelectAreaButton);

	pScopeLayout->addLayout(pAreaSearchLayout);
	pMainLayout->addWidget(m_pScopeGroup);

	m_pColorGroup = new QGroupBox(tr("Set color for recognized shapes"));
	QHBoxLayout* pColorLayout = new QHBoxLayout(m_pColorGroup);
	m_pCurrentColorLabel = new QLabel(tr("Current color:"));
	m_pColorSwatch = new QWidget();
	m_pColorSwatch->setFixedSize(50, 30);
	m_pColorSwatch->setStyleSheet("background-color: #4A6EB6; border: 1px solid #A0A0A0;");

	m_pSelectColorButton = new QPushButton(tr("Select Color"));
	m_pSelectColorButton->setIcon(QIcon(":/resources/images2d/2d_btnSelColor.svg"));

	pColorLayout->addWidget(m_pCurrentColorLabel);
	pColorLayout->addWidget(m_pColorSwatch);
	pColorLayout->addStretch();
	pColorLayout->addWidget(m_pSelectColorButton);
	pColorLayout->addStretch();
	pMainLayout->addWidget(m_pColorGroup);

	QHBoxLayout* pOptionsLayout = new QHBoxLayout();
	m_pFilterCheckBox = new QCheckBox(tr("Filter out shapes contained within already recognized shapes during recognition"));
	m_pFilterCheckBox->setChecked(true);

	m_pHelpButton = new QPushButton();
	m_pHelpButton->setIcon(QIcon(":/resources/images2d/2d_btnHelp.svg"));
	m_pHelpButton->setToolTip(tr("Click here for detailed instructions"));
	m_pHelpButton->setText("");
	m_pHelpButton->setIconSize(QSize(24, 24));
	m_pHelpButton->setFixedSize(32, 32);
	m_pHelpButton->setStyleSheet(R"(
		QPushButton {
			border: none;
			background: transparent;
		}
		QPushButton:hover {
			background-color: #e0e0e0;
			border-radius: 4px;
		}
		QPushButton:pressed {
			background-color: #c0c0c0;
		}
	)");

	pOptionsLayout->addWidget(m_pFilterCheckBox);
	pOptionsLayout->addStretch();
	pOptionsLayout->addWidget(m_pHelpButton);
	pMainLayout->addLayout(pOptionsLayout);

	QHBoxLayout* pBottomButtonLayout = new QHBoxLayout();
	m_pStartButton = new QPushButton(tr("Start Recognition"));
	m_pStartButton->setDefault(true);
	m_pStartButton->setMinimumSize(100, 30);
	m_pStartButton->setStyleSheet("background-color: #0078D7; color: white; border: none; padding: 10px 20px; border-radius: 5px;");

	pBottomButtonLayout->addStretch();
	pBottomButtonLayout->addWidget(m_pStartButton);
	pBottomButtonLayout->addStretch();

	pMainLayout->addLayout(pBottomButtonLayout);

	setFixedWidth(500);
}

void Mx2dShapeRecognitionDialog::setupConnections()
{
	connect(m_pEntireDrawingRadio, &QRadioButton::toggled, this, &Mx2dShapeRecognitionDialog::onScopeChanged);
	connect(m_pRectangularSearchRadio, &QRadioButton::toggled, this, &Mx2dShapeRecognitionDialog::onScopeChanged);
	connect(m_pIrregularSearchRadio, &QRadioButton::toggled, this, &Mx2dShapeRecognitionDialog::onScopeChanged);

	connect(m_pSelectColorButton, &QPushButton::clicked, this, &Mx2dShapeRecognitionDialog::onSelectColor);

	connect(m_pStartButton, &QPushButton::clicked, this, &QDialog::accept);
}

void Mx2dShapeRecognitionDialog::onScopeChanged()
{
	bool enableSelectArea = !m_pEntireDrawingRadio->isChecked();
	m_pSelectAreaButton->setEnabled(enableSelectArea);
}

void Mx2dShapeRecognitionDialog::onSelectColor()
{
	QColor currentColor = m_pColorSwatch->palette().color(QPalette::Window);

	QColor newColor = QColorDialog::getColor(currentColor, this, tr("Select Color"));

	if (newColor.isValid()) {
		m_pColorSwatch->setStyleSheet(QString("background-color: %1; border: 1px solid #A0A0A0;").arg(newColor.name()));
	}
}