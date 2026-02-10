/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "MxViewToolBar.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QPaintEvent>
#include <QToolButton>
#include <QDebug>

MxViewToolBar::MxViewToolBar(QWidget* parent)
	: QWidget(parent)
{
	m_pHBoxLayout = new QHBoxLayout();

	m_pHBoxLayout->setContentsMargins(2, 2, 2, 2);

    m_pHBoxLayout->setSpacing(2);

    m_pHBoxLayout->setSizeConstraint(QLayout::SetFixedSize);
	setLayout(m_pHBoxLayout);

	setCursor(Qt::ArrowCursor);
}

MxViewToolBar::~MxViewToolBar()
{

}

void MxViewToolBar::setTranslucent(bool isTranslucent)
{
    m_isTranslucent = isTranslucent;
}

bool MxViewToolBar::isTranslucent() const
{
	return m_isTranslucent;
}

void MxViewToolBar::paintEvent(QPaintEvent* event)
{
	QWidget::paintEvent(event);

    QPainter painter(this);
	if (m_isTranslucent)
	{
		painter.fillRect(rect(), QColor(200, 200, 200, 180));
	}
	else
	{
		painter.fillRect(rect(), QColor(200, 200, 200));
		QColor borderColor(50, 50, 50);
		QPen borderPen(borderColor);
		borderPen.setWidth(1);
		painter.setPen(borderPen);
		QRect borderRect = rect().adjusted(1, 1, -2, -2);
		painter.drawRect(borderRect);
	}
	
}

QToolButton* MxViewToolBar::addAction(QAction* action)
{
	QToolButton* button = new QToolButton();
	button->setStyleSheet(R"(
		QToolButton:checked {
			background-color: rgb(152, 212, 255);
		}
		QToolButton:hover {
			background-color: rgb(171, 220, 255);
		}
	)");

    button->setIconSize(QSize(32, 32));

    button->setToolButtonStyle(Qt::ToolButtonIconOnly);

	m_pHBoxLayout->addWidget(button);
	button->setDefaultAction(action);
	button->setPopupMode(QToolButton::InstantPopup);
	adjustSize();
	return button;
}

void MxViewToolBar::addWidget(QWidget* widget)
{
	widget->setFixedHeight(36);
	m_pHBoxLayout->addWidget(widget);
	adjustSize();
}
