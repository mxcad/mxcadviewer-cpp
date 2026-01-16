/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dDrawingCompareSetColorDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSpacerItem>
#include <QPalette>

Mx2dDrawingCompareSetColorDialog::Mx2dDrawingCompareSetColorDialog(QWidget* parent)
	: QDialog(parent)
{
	Qt::WindowFlags flags = Qt::Dialog;
	flags |= Qt::CustomizeWindowHint;
	flags |= Qt::WindowTitleHint;
	flags |= Qt::WindowCloseButtonHint;
	setWindowFlags(flags);

	setWindowTitle(tr("Color Settings"));

	QLabel* labelHeader1 = new QLabel(tr("Color"));
	QLabel* labelHeader2 = new QLabel(tr("Drawing Name"));

	m_btnColor1 = new QToolButton();
	m_btnColor1->setFixedSize(30, 30);
	m_btnColor1->setStyleSheet("background-color: yellow;");

	m_lblPath1 = new QLabel("C:\\Users\\MxDev2\\Downloads\\ExternalInternalMold.dwg" + tr("Model"));

	m_btnColor2 = new QToolButton();
	m_btnColor2->setFixedSize(30, 30);
	m_btnColor2->setStyleSheet("background-color: cyan;");

	m_lblPath2 = new QLabel("C:\\Users\\MxDev2\\Downloads\\TestDrawing.dwg" + tr("Model"));

	m_btnColorSame = new QToolButton();
	m_btnColorSame->setFixedSize(30, 30);
	m_btnColorSame->setStyleSheet("background-color: gray;");

	m_lblSame = new QLabel(tr("Same Parts"));

	m_btnReset = new QPushButton(tr("Restore Default Colors"));
	connect(m_btnReset, &QPushButton::clicked, this, &Mx2dDrawingCompareSetColorDialog::onResetColors);

	QLabel* labelTip = new QLabel(tr("Click color buttons to set colors for different parts and same parts of the two drawings respectively"));

	QGridLayout* gridLayout = new QGridLayout();
	gridLayout->addWidget(labelHeader1, 0, 0);
	gridLayout->addWidget(labelHeader2, 0, 1);
	gridLayout->addWidget(m_btnColor1, 1, 0);
	gridLayout->addWidget(m_lblPath1, 1, 1);
	gridLayout->addWidget(m_btnColor2, 2, 0);
	gridLayout->addWidget(m_lblPath2, 2, 1);
	gridLayout->addWidget(m_btnColorSame, 3, 0);
	gridLayout->addWidget(m_lblSame, 3, 1);

	QHBoxLayout* bottomLayout = new QHBoxLayout();
	bottomLayout->addStretch();
	bottomLayout->addWidget(m_btnReset);

	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->addLayout(gridLayout);
	mainLayout->addSpacing(10);
	mainLayout->addWidget(labelTip);
	mainLayout->addSpacing(10);
	mainLayout->addLayout(bottomLayout);

	resize(520, 200);
}

void Mx2dDrawingCompareSetColorDialog::onResetColors()
{
	m_btnColor1->setStyleSheet("background-color: yellow;");
	m_btnColor2->setStyleSheet("background-color: cyan;");
	m_btnColorSame->setStyleSheet("background-color: gray;");
}