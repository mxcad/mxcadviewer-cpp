/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dDimCategoryManagerDialog.h"

#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>

Mx2dDimCategoryManagerDialog::Mx2dDimCategoryManagerDialog(QWidget* parent)
	: QDialog(parent)
{
	Qt::WindowFlags flags = Qt::Dialog;
	flags |= Qt::CustomizeWindowHint;
	flags |= Qt::WindowTitleHint;
	flags |= Qt::WindowCloseButtonHint;
	setWindowFlags(flags);

	setWindowTitle(tr("Dimension Category Management"));
	setMinimumSize(800, 600);

	initUI();
	initLayout();
}

void Mx2dDimCategoryManagerDialog::initUI()
{
	m_pBtnNewCategory = new QPushButton(QIcon(":/resources/images2d/2d_newDimCategory.svg"), tr("New Category"));
	m_pLblCurrentCategory = new QLabel(tr("Current Category: Unclassified"));
	m_pBtnShowAll = new QPushButton(tr("Show All"));
	m_pBtnHideAll = new QPushButton(tr("Hide All"));

	m_pEditSearch = new QLineEdit();
	m_pEditSearch->setPlaceholderText(tr("Please enter the category name to search"));
	m_pBtnSearch = new QPushButton(tr("Search"));

	m_pTable = new QTableWidget(0, 4);
	m_pTable->setHorizontalHeaderLabels({ tr("Name"), tr("Color"), tr("Show"), tr("Current") });
	m_pTable->horizontalHeader()->setStretchLastSection(true);
	m_pTable->verticalHeader()->setVisible(false);
	m_pTable->setSelectionMode(QAbstractItemView::NoSelection);
	m_pTable->setEditTriggers(QAbstractItemView::DoubleClicked);

	m_pLblTip = new QLabel(tr("Double-click table items to modify category name and color, or set current category"));
}

void Mx2dDimCategoryManagerDialog::initLayout()
{
	QHBoxLayout* pTopLayout = new QHBoxLayout();
	pTopLayout->addWidget(m_pBtnNewCategory);
	pTopLayout->addWidget(m_pLblCurrentCategory);
	pTopLayout->addStretch();
	pTopLayout->addWidget(m_pBtnShowAll);
	pTopLayout->addWidget(m_pBtnHideAll);

	QHBoxLayout* pSearchLayout = new QHBoxLayout();
	pSearchLayout->addWidget(m_pEditSearch);
	pSearchLayout->addWidget(m_pBtnSearch);

	QVBoxLayout* pMainLayout = new QVBoxLayout(this);
	pMainLayout->addLayout(pTopLayout);
	pMainLayout->addLayout(pSearchLayout);
	pMainLayout->addWidget(m_pTable);
	pMainLayout->addWidget(m_pLblTip);
}