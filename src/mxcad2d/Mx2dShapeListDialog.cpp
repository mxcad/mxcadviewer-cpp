/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dShapeListDialog.h"
#include "Mx2dShapeListItemWidget.h"
#include <QtWidgets>
#include <QDebug>
#include <QLabel>

Mx2dShapeListDialog::Mx2dShapeListDialog(QWidget* parent)
	: QDialog(parent)
{
	Qt::WindowFlags flags = Qt::Dialog;
	flags |= Qt::CustomizeWindowHint;
	flags |= Qt::WindowTitleHint;
	flags |= Qt::WindowCloseButtonHint;
	setWindowFlags(flags);

	setWindowTitle(tr("Shape List"));
	setupUi();

	addShapeItem(tr("Shape 1"), 2);
	addShapeItem(tr("Shape 2"), 15);

	switchToMainView();
}

Mx2dShapeListDialog::~Mx2dShapeListDialog() {}

void Mx2dShapeListDialog::setupUi()
{
	QVBoxLayout* pMainLayout = new QVBoxLayout(this);
	pMainLayout->setContentsMargins(0, 0, 0, 0);
	pMainLayout->setSpacing(0);

	m_pStackedWidget = new QStackedWidget();
	m_pStackedWidget->addWidget(createMainPage());
	m_pStackedWidget->addWidget(createDetailPage());

	pMainLayout->addWidget(m_pStackedWidget);

	QWidget* pBottomWidget = new QWidget();
	pBottomWidget->setAutoFillBackground(true);
	QPalette pal = pBottomWidget->palette();
	pal.setColor(QPalette::Window, QColor("#F7F7F7"));
	pBottomWidget->setPalette(pal);
	pBottomWidget->setStyleSheet("QWidget { border-top: 1px solid #E5E5E5; }");

	QVBoxLayout* pBottomLayout = new QVBoxLayout(pBottomWidget);
	pBottomLayout->setContentsMargins(10, 10, 10, 10);

	QHBoxLayout* pExportButtonLayout = new QHBoxLayout();
	m_pExportButton = new QPushButton(tr("Export List to Excel"));
	pExportButtonLayout->addStretch();
	pExportButtonLayout->addWidget(m_pExportButton);
	pExportButtonLayout->addStretch();

	m_pHintLabel = new QLabel();
	m_pHintLabel->setWordWrap(true);

	pBottomLayout->addLayout(pExportButtonLayout);
	pBottomLayout->addWidget(m_pHintLabel);

	pMainLayout->addWidget(pBottomWidget);

	resize(380, 600);
}

QWidget* Mx2dShapeListDialog::createMainPage()
{
	QWidget* pPage = new QWidget();
	QVBoxLayout* pPageLayout = new QVBoxLayout(pPage);
	pPageLayout->setContentsMargins(0, 0, 0, 0);
	pPageLayout->setSpacing(0);

	QLabel* pHeaderLabel = new QLabel(tr("All Shapes"));
	pHeaderLabel->setAlignment(Qt::AlignCenter);
	pHeaderLabel->setMinimumHeight(50);
	pHeaderLabel->setStyleSheet("background-color: #F7F7F7; font-size: 18px; font-weight: bold; color: #333; border-bottom: 1px solid #E5E5E5;");

	m_pListWidget = new QListWidget();
	m_pListWidget->setFrameShape(QFrame::NoFrame);
	m_pListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	connect(m_pListWidget, &QListWidget::itemDoubleClicked, this, &Mx2dShapeListDialog::onListItemDoubleClicked);

	pPageLayout->addWidget(pHeaderLabel);
	pPageLayout->addWidget(m_pListWidget, 1);

	return pPage;
}

QWidget* Mx2dShapeListDialog::createDetailPage()
{
	QWidget* pPage = new QWidget();
	QVBoxLayout* pPageLayout = new QVBoxLayout(pPage);
	pPageLayout->setContentsMargins(0, 0, 0, 0);
	pPageLayout->setSpacing(0);

	QWidget* pHeaderWidget = new QWidget();
	pHeaderWidget->setMinimumHeight(50);
	pHeaderWidget->setStyleSheet("background-color: #F7F7F7; border-bottom: 1px solid #E5E5E5;");
	QHBoxLayout* pHeaderLayout = new QHBoxLayout(pHeaderWidget);

	QPushButton* pBackButton = new QPushButton(tr("←"));
	pBackButton->setFixedSize(40, 40);
	pBackButton->setStyleSheet("QPushButton { font-size: 24px; color: #337AB7; border: none; background: transparent; } QPushButton:hover { color: #23527C; }");
	connect(pBackButton, &QPushButton::clicked, this, &Mx2dShapeListDialog::switchToMainView);

	m_pDetailTitleLabel = new QLabel();
	m_pDetailTitleLabel->setAlignment(Qt::AlignCenter);
	m_pDetailTitleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #333;");

	pHeaderLayout->addWidget(pBackButton);
	pHeaderLayout->addWidget(m_pDetailTitleLabel, 1);

	m_pDetailTable = new QTableWidget();
	m_pDetailTable->setColumnCount(2);
	m_pDetailTable->setHorizontalHeaderLabels({ tr("No."), tr("Coordinates") });
	m_pDetailTable->setFrameShape(QFrame::NoFrame);
	m_pDetailTable->verticalHeader()->hide();
	m_pDetailTable->horizontalHeader()->setStretchLastSection(true);
	m_pDetailTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pDetailTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pDetailTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_pDetailTable->setShowGrid(false);
	m_pDetailTable->setStyleSheet("QTableWidget { border: none; } QHeaderView::section { background-color: #F7F7F7; padding: 5px; border: none; border-bottom: 1px solid #E5E5E5; font-weight: bold; } QTableWidget::item { padding-left: 10px; } QTableWidget::item:selected { background-color: #AEEEEE; color: black; }");

	pPageLayout->addWidget(pHeaderWidget);
	pPageLayout->addWidget(m_pDetailTable, 1);

	return pPage;
}

void Mx2dShapeListDialog::addShapeItem(const QString& name, int count)
{
	QListWidgetItem* pListItem = new QListWidgetItem(m_pListWidget);
	Mx2dShapeListItemWidget* pItemWidget = new Mx2dShapeListItemWidget(name, count);

	connect(pItemWidget, &Mx2dShapeListItemWidget::viewClicked, this, &Mx2dShapeListDialog::switchToDetailView);
	connect(pItemWidget, &Mx2dShapeListItemWidget::deleteClicked, this, [this, pListItem]() {
		this->onDeleteItem(pListItem);
		});
	connect(pItemWidget, &Mx2dShapeListItemWidget::buttonHoverChanged, this, &Mx2dShapeListDialog::updateHint);

	pListItem->setSizeHint(pItemWidget->sizeHint());
	m_pListWidget->addItem(pListItem);
	m_pListWidget->setItemWidget(pListItem, pItemWidget);
}

void Mx2dShapeListDialog::switchToDetailView(const QString& shapeName, int count)
{
	m_pDetailTitleLabel->setText(QString(tr("%1 (%2 items)")).arg(shapeName).arg(count));

	m_pDetailTable->setRowCount(0); // Clear table
	m_pDetailTable->setRowCount(count);

	if (shapeName == tr("Shape 1")) {
		m_pDetailTable->setItem(0, 0, new QTableWidgetItem("1"));
		m_pDetailTable->setItem(0, 1, new QTableWidgetItem("(63066.160, -233571.550)"));
		m_pDetailTable->setItem(1, 0, new QTableWidgetItem("2"));
		m_pDetailTable->setItem(1, 1, new QTableWidgetItem("(114600.252, 64280.671)"));
	}
	else {
		for (int i = 0; i < count; ++i) {
			m_pDetailTable->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
			m_pDetailTable->setItem(i, 1, new QTableWidgetItem(QString(tr("(%1, %2)")).arg(qrand() % 200000).arg(qrand() % 200000)));
		}
	}

	for (int i = 0; i < m_pDetailTable->rowCount(); ++i) {
		m_pDetailTable->item(i, 0)->setTextAlignment(Qt::AlignCenter);
		m_pDetailTable->item(i, 1)->setTextAlignment(Qt::AlignCenter);
	}
	m_pDetailTable->selectRow(0);

	m_pStackedWidget->setCurrentIndex(1);
	updateHint(tr("Hint: Left-click table item or press ↑ ↓ keys to switch"));
	m_pExportButton->setVisible(false);
}

void Mx2dShapeListDialog::switchToMainView()
{
	m_pStackedWidget->setCurrentIndex(0);
	updateHint(tr("Hint: Double-click table item or click \"View\" button to see detailed positions of recognized shapes"));
	m_pExportButton->setVisible(true);
}

void Mx2dShapeListDialog::updateHint(const QString& hint)
{
	if (hint.startsWith(tr("Hint:"))) {
		m_pHintLabel->setText(QString("<span style='color:#E67E22;'>%1</span>").arg(hint));
	}
	else {
		m_pHintLabel->setText("<span style='color:#E67E22;'>" + tr("Hint:") + "</span> " + hint);
	}
}

void Mx2dShapeListDialog::onListItemDoubleClicked(QListWidgetItem* pItem)
{
	if (!pItem) return;
	auto pItemWidget = static_cast<Mx2dShapeListItemWidget*>(m_pListWidget->itemWidget(pItem));
	if (pItemWidget) {
		switchToDetailView(pItemWidget->shapeName(), pItemWidget->shapeCount());
	}
}

void Mx2dShapeListDialog::onDeleteItem(QListWidgetItem* pItem)
{
	if (!pItem) return;

	auto pItemWidget = static_cast<Mx2dShapeListItemWidget*>(m_pListWidget->itemWidget(pItem));
	QString shapeName = pItemWidget->shapeName();

	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(this, tr("Confirm Delete"), QString(tr("Are you sure you want to delete \"%1\"?")).arg(shapeName),
		QMessageBox::Yes | QMessageBox::No);
	if (reply == QMessageBox::Yes) {
		int row = m_pListWidget->row(pItem);
		m_pListWidget->takeItem(row);
		delete pItem;
	}
}