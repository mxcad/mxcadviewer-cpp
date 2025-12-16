/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dDrawingCompareDialog.h"

#include <QToolButton>
#include <QLabel>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPalette>
#include <QIcon>
#include "Mx2dDrawingCompareSetColorDialog.h"

Mx2dDrawingCompareDialog::Mx2dDrawingCompareDialog(QWidget* parent)
	: QDialog(parent)
{
	setWindowFlags(Qt::Dialog | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
	setWindowTitle(tr("Drawing Comparison"));
	resize(1200, 700);
	initUI();
	connectSignals();
	initLayout();
}

void Mx2dDrawingCompareDialog::initUI()
{
	m_pDlgSetColor = new Mx2dDrawingCompareSetColorDialog(this);

	m_pBtnSetColor = new QToolButton(this);
	m_pBtnSetColor->setIcon(QIcon(":/resources/images2d/2d_drawingCompareSetColor.svg"));
	m_pBtnSetColor->setToolTip(tr("Set Color"));
	m_pBtnSetColor->setAutoRaise(true);
	m_pBtnSetColor->setIconSize(QSize(32, 32));
	m_pBtnSetColor->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	m_pBtnSetColor->setText(tr("Set Color"));

	m_pBtnSetBasePoint = new QToolButton(this);
	m_pBtnSetBasePoint->setIcon(QIcon(":/resources/images2d/2d_drawingCompareSetBasePoint.svg"));
	m_pBtnSetBasePoint->setToolTip(tr("Set Base Point"));
	m_pBtnSetBasePoint->setAutoRaise(true);
	m_pBtnSetBasePoint->setIconSize(QSize(32, 32));
	m_pBtnSetBasePoint->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	m_pBtnSetBasePoint->setText(tr("Set Base Point"));

	m_pBtnResetBasePoint = new QToolButton(this);
	m_pBtnResetBasePoint->setIcon(QIcon(":/resources/images2d/2d_drawingCompareRestoreBasePoint.svg"));
	m_pBtnResetBasePoint->setToolTip(tr("Restore Base Point"));
	m_pBtnResetBasePoint->setAutoRaise(true);
	m_pBtnResetBasePoint->setIconSize(QSize(32, 32));
	m_pBtnResetBasePoint->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	m_pBtnResetBasePoint->setText(tr("Restore Base Point"));

	m_pCheckPaper1 = new QCheckBox(tr("Drawing 1"));
	m_pColorLabel1 = new QLabel();
	m_pColorLabel1->setFixedSize(20, 20);
	m_pColorLabel1->setStyleSheet("background-color: yellow; border: 1px solid gray;");
	m_pPathLabel1 = new QLabel(tr("C:\\Users\\MxDev2\\Downloads\\ExternalInternalMold.dwg"));

	m_pCheckPaper2 = new QCheckBox(tr("Drawing 2"));
	m_pColorLabel2 = new QLabel();
	m_pColorLabel2->setFixedSize(20, 20);
	m_pColorLabel2->setStyleSheet("background-color: cyan; border: 1px solid gray;");
	m_pPathLabel2 = new QLabel(tr("C:\\Users\\MxDev2\\Downloads\\TestDrawing.dwg"));

	m_pCheckSyncDisplay = new QCheckBox(tr("Sync display to main window"));

	m_pDrawingArea = new QWidget();
	m_pDrawingArea->setAutoFillBackground(true);
	QPalette pal;
	pal.setColor(QPalette::Window, Qt::black);
	m_pDrawingArea->setPalette(pal);
}

void Mx2dDrawingCompareDialog::initLayout()
{
	QHBoxLayout* pBtnLayout = new QHBoxLayout();
	pBtnLayout->addWidget(m_pBtnSetColor);
	pBtnLayout->addWidget(m_pBtnSetBasePoint);
	pBtnLayout->addWidget(m_pBtnResetBasePoint);

	QHBoxLayout* pPaper1Layout = new QHBoxLayout();
	pPaper1Layout->addWidget(m_pCheckPaper1);
	pPaper1Layout->addWidget(m_pColorLabel1);
	pPaper1Layout->addWidget(m_pPathLabel1);

	QHBoxLayout* pPaper2Layout = new QHBoxLayout();
	pPaper2Layout->addWidget(m_pCheckPaper2);
	pPaper2Layout->addWidget(m_pColorLabel2);
	pPaper2Layout->addWidget(m_pPathLabel2);

	QVBoxLayout* pPaperCheckAreaLayout = new QVBoxLayout();
	pPaperCheckAreaLayout->addLayout(pPaper1Layout);
	pPaperCheckAreaLayout->addLayout(pPaper2Layout);


	QHBoxLayout* pTopLayout = new QHBoxLayout();
	pTopLayout->addLayout(pBtnLayout);

	QFrame* verticalLine1 = new QFrame(this);
	verticalLine1->setFrameShape(QFrame::VLine);
	verticalLine1->setFrameShadow(QFrame::Sunken);
	pTopLayout->addWidget(verticalLine1);

	pTopLayout->addLayout(pPaperCheckAreaLayout);

	pTopLayout->addStretch();

	QFrame* verticalLine2 = new QFrame(this);
	verticalLine2->setFrameShape(QFrame::VLine);
	verticalLine2->setFrameShadow(QFrame::Sunken);
	pTopLayout->addWidget(verticalLine2);
	pTopLayout->addWidget(m_pCheckSyncDisplay);

	QVBoxLayout* pMainLayout = new QVBoxLayout(this);
	pMainLayout->addLayout(pTopLayout);
	pMainLayout->addWidget(m_pDrawingArea);
	pMainLayout->setContentsMargins(2, 10, 2, 2);
}

void Mx2dDrawingCompareDialog::connectSignals()
{
	connect(m_pBtnSetColor, &QToolButton::clicked, this, [this]() {
		m_pDlgSetColor->exec();
		});
}