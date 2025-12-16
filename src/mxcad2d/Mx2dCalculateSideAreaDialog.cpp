/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dCalculateSideAreaDialog.h"
#include "AreaShapePreviewWidget.h"
#include <QtWidgets>
#include <QClipboard>
#include "Mx2dUtils.h"

Mx2dCalculateSideAreaDialog::Mx2dCalculateSideAreaDialog(QWidget* parent)
	: QDialog(parent)
{
	Qt::WindowFlags flags = Qt::Dialog;
	flags |= Qt::CustomizeWindowHint;
	flags |= Qt::WindowTitleHint;
	flags |= Qt::WindowCloseButtonHint;
	setWindowFlags(flags);

	setupUi();
	setupConnections();

	// Initial state update
	onSeparateHeightsToggled(m_pSeparateHeightsCheckBox->isChecked());
}

Mx2dCalculateSideAreaDialog::~Mx2dCalculateSideAreaDialog() {}

void Mx2dCalculateSideAreaDialog::setPolylineData(const Mx2d::PLVertexList& vertices)
{
	m_pShapePreview->setPolylineData(vertices);
	populateTable();
	updateLengthLabel();
}

void Mx2dCalculateSideAreaDialog::setupUi()
{
	setWindowTitle(tr("Calculate Side Area"));
	setMinimumSize(900, 900);

	// --- Main Layout (Horizontal) ---
	QHBoxLayout* pMainLayout = new QHBoxLayout(this);
	pMainLayout->setSpacing(15);

	// --- Left Panel ---
	QWidget* pLeftWidget = new QWidget();
	QVBoxLayout* pLeftLayout = new QVBoxLayout(pLeftWidget);
	pLeftLayout->setContentsMargins(0, 0, 0, 0);

	m_pShapePreview = new AreaShapePreviewWidget();
	m_pFullViewButton = new QPushButton(tr("Full View"));

	QWidget* pInfoWidget = new QWidget();
	QHBoxLayout* pInfoLayout = new QHBoxLayout(pInfoWidget);
	pInfoLayout->setContentsMargins(0, 5, 0, 0);
	m_pSelectedLengthLabel = new QLabel(tr("Length of selected shape:"));
	m_pSelectedLengthValueLabel = new QLabel("0.000");
	m_pSelectedLengthValueLabel->setStyleSheet("color: red;");
	pInfoLayout->addWidget(m_pSelectedLengthLabel);
	pInfoLayout->addWidget(m_pSelectedLengthValueLabel);
	pInfoLayout->addStretch();

	m_pHintLabel = new QLabel(tr("Hint: To exclude an edge from calculation, click on it in the preview. Red indicates selected, gray indicates deselected."));
	m_pHintLabel->setWordWrap(true);
	m_pHintLabel->setStyleSheet("color: grey;");

	pLeftLayout->addWidget(m_pShapePreview, 1);
	pLeftLayout->addWidget(m_pFullViewButton, 0, Qt::AlignLeft);
	pLeftLayout->addWidget(pInfoWidget);
	pLeftLayout->addWidget(m_pHintLabel);

	// --- Right Panel ---
	QWidget* pRightWidget = new QWidget();
	pRightWidget->setFixedWidth(320);
	QVBoxLayout* pRightLayout = new QVBoxLayout(pRightWidget);
	pRightLayout->setSpacing(10);

	QHBoxLayout* pHeightLayout = new QHBoxLayout();
	m_pHeightLabel = new QLabel(tr("Enter Height:"));
	m_pHeightEdit = new QLineEdit();
	pHeightLayout->addWidget(m_pHeightLabel);
	pHeightLayout->addWidget(m_pHeightEdit);

	m_pSeparateHeightsCheckBox = new QCheckBox(tr("Set height for each edge separately"));
	m_pSeparateHeightsCheckBox->setChecked(false);

	m_pHeightTable = new QTableWidget();
	m_pHeightTable->setColumnCount(2);
	m_pHeightTable->setHorizontalHeaderLabels({ tr("Edge ID"), tr("Height") });
	m_pHeightTable->horizontalHeader()->setStretchLastSection(false);
	m_pHeightTable->verticalHeader()->setVisible(false);
	m_pHeightTable->setColumnWidth(0, 100);
	m_pHeightTable->setColumnWidth(1, 75);

	m_pCalculateButton = new QPushButton(tr("Calculate"));

	QHBoxLayout* pResultLayout = new QHBoxLayout();
	m_pAreaLabel = new QLabel(tr("Side Area:"));
	m_pAreaResultEdit = new QLineEdit();
	m_pAreaResultEdit->setReadOnly(true);
	m_pCopyResultButton = new QPushButton(tr("Copy"));
	pResultLayout->addWidget(m_pAreaLabel);
	pResultLayout->addWidget(m_pAreaResultEdit);
	pResultLayout->addWidget(m_pCopyResultButton);

	m_pProcessLabel = new QLabel(tr("Calculation Process:"));
	m_pProcessTextEdit = new QTextEdit();
	m_pProcessTextEdit->setReadOnly(true);
	m_pProcessTextEdit->setFixedHeight(220);

	m_pCopyProcessButton = new QPushButton(tr("Copy"));

	pRightLayout->addLayout(pHeightLayout);
	pRightLayout->addWidget(m_pSeparateHeightsCheckBox);
	pRightLayout->addWidget(m_pHeightTable, 1);
	pRightLayout->addWidget(m_pCalculateButton, 0, Qt::AlignCenter);
	pRightLayout->addLayout(pResultLayout);
	pRightLayout->addWidget(m_pProcessLabel);
	pRightLayout->addWidget(m_pProcessTextEdit);
	pRightLayout->addWidget(m_pCopyProcessButton, 0, Qt::AlignRight);
	pRightLayout->addStretch();

	// --- Combine Main Layout ---
	pMainLayout->addWidget(pLeftWidget, 1);
	pMainLayout->addWidget(pRightWidget);
}

void Mx2dCalculateSideAreaDialog::setupConnections()
{
	connect(m_pShapePreview, &AreaShapePreviewWidget::selectionChanged, this, &Mx2dCalculateSideAreaDialog::onShapeSelectionChanged);
	connect(m_pSeparateHeightsCheckBox, &QCheckBox::toggled, this, &Mx2dCalculateSideAreaDialog::onSeparateHeightsToggled);
	connect(m_pCalculateButton, &QPushButton::clicked, this, &Mx2dCalculateSideAreaDialog::onCalculate);
	connect(m_pCopyResultButton, &QPushButton::clicked, this, &Mx2dCalculateSideAreaDialog::onCopyResult);
	connect(m_pCopyProcessButton, &QPushButton::clicked, this, &Mx2dCalculateSideAreaDialog::onCopyProcess);
	connect(m_pFullViewButton, &QPushButton::clicked, this, &Mx2dCalculateSideAreaDialog::onFullViewClicked);
	connect(m_pHeightEdit, &QLineEdit::textChanged, this, &Mx2dCalculateSideAreaDialog::onHeightEditTextChanged);
}

void Mx2dCalculateSideAreaDialog::updateLengthLabel()
{
	m_pSelectedLengthValueLabel->setText(QString::number(m_pShapePreview->getTotalSelectedLength(), 'f', 3));
}

void Mx2dCalculateSideAreaDialog::populateTable()
{
	const auto& segments = m_pShapePreview->getSegments();
	m_pHeightTable->setRowCount(segments.size());
	for (int i = 0; i < segments.size(); ++i) {
		// Edge ID column
		QTableWidgetItem* pIdItem = new QTableWidgetItem(QString::number(segments[i].id));
		pIdItem->setFlags(pIdItem->flags() & ~Qt::ItemIsEditable); // Not editable
		pIdItem->setTextAlignment(Qt::AlignCenter);

		// Height column
		QTableWidgetItem* pHeightItem = new QTableWidgetItem("0.0");
		pHeightItem->setTextAlignment(Qt::AlignCenter);
		pHeightItem->setData(Qt::UserRole, segments[i].selected);

		m_pHeightTable->setItem(i, 0, pIdItem);
		m_pHeightTable->setItem(i, 1, pHeightItem);

		// Set row color based on selection status
		QColor rowColor = segments[i].selected ? Qt::white : QColor("#f0f0f0");
		pIdItem->setBackground(rowColor);
		pHeightItem->setBackground(rowColor);
	}
}

void Mx2dCalculateSideAreaDialog::onShapeSelectionChanged()
{
	updateLengthLabel();
	// Refresh the status of table rows
	const auto& segments = m_pShapePreview->getSegments();
	for (int i = 0; i < segments.size(); ++i) {
		QColor rowColor = segments[i].selected ? Qt::white : QColor("#f0f0f0");
		m_pHeightTable->item(i, 0)->setBackground(rowColor);
		m_pHeightTable->item(i, 1)->setBackground(rowColor);
		m_pHeightTable->item(i, 1)->setData(Qt::UserRole, segments[i].selected);
	}
}

void Mx2dCalculateSideAreaDialog::onSeparateHeightsToggled(bool checked)
{
	m_pHeightTable->setVisible(checked);
}

void Mx2dCalculateSideAreaDialog::onCalculate()
{
	if (!m_pSeparateHeightsCheckBox->isChecked())
	{
		if (m_pHeightEdit->text().isEmpty())
		{
			QMessageBox::information(this, tr("Hint"), tr("Height cannot be empty."));
			return;
		}
		double height = m_pHeightEdit->text().toDouble();
		QVector<EdgeSegment> segments = m_pShapePreview->getSegments();

		QStringList calculateProcessList;

		double length = 0.0;
		for (int i = 0; i < segments.size(); i++)
		{
			EdgeSegment seg = segments[i];
			if (seg.selected)
			{
				double segLength = seg.length;
				length += segLength;
				calculateProcessList.append(QString("%1").arg(QString::number(segLength, 'f', 3)));
			}
		}
		double area = length * height;
		QString calculateString = calculateProcessList.join(" + ");
		calculateString = QString("(%1) * %2 = %3").arg(calculateString).arg(height).arg(QString::number(area, 'f', 3));
		m_pAreaResultEdit->setText(QString::number(area, 'f', 3));
		m_pProcessTextEdit->setText(calculateString);
	}
	else
	{
		auto getSegmentLengthById = [](int id, const QVector<EdgeSegment>& segments) {
			for (int i = 0; i < segments.length(); ++i) {
				const auto& seg = segments[i];
				if (seg.id == id)
				{
					return seg.length;
				}
			}
			return 0.0;
			};

		const auto& segments = m_pShapePreview->getSegments();
		QStringList calculateProcessList;
		double area = 0.0;
		for (int i = 0; i < m_pHeightTable->rowCount(); ++i) {
			bool isSel = m_pHeightTable->item(i, 1)->data(Qt::UserRole).toBool();
			if (isSel)
			{
				int id = m_pHeightTable->item(i, 0)->text().toInt();
				double length = getSegmentLengthById(id, segments);
				double height = m_pHeightTable->item(i, 1)->text().toDouble();
				area += length * height;
				calculateProcessList.append(QString("%1*%2").arg(QString::number(length, 'f', 3)).arg(height));
			}
		}
		QString calculateString = calculateProcessList.join(" + ");
		calculateString += " = " + QString::number(area, 'f', 3);
		m_pAreaResultEdit->setText(QString::number(area, 'f', 3));
		m_pProcessTextEdit->setText(calculateString);
	}
}

void Mx2dCalculateSideAreaDialog::onCopyResult()
{
	QApplication::clipboard()->setText(m_pAreaResultEdit->text());
}

void Mx2dCalculateSideAreaDialog::onCopyProcess()
{
	QApplication::clipboard()->setText(m_pProcessTextEdit->toPlainText());
}

void Mx2dCalculateSideAreaDialog::onFullViewClicked()
{
	if (m_pShapePreview) {
		m_pShapePreview->fitToView();
	}
}

void Mx2dCalculateSideAreaDialog::onHeightEditTextChanged(const QString& text)
{
	QString t = text;
	if (t.isEmpty())
	{
		t = "0.0";
	}
	for (int i = 0; i < m_pHeightTable->rowCount(); ++i) {
		m_pHeightTable->item(i, 1)->setText(t);
	}
}