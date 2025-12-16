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
	}
	
    painter.end();
}

void MxViewToolBar::addAction(QAction* action)
{
	QToolButton* button = new QToolButton();
	button->setStyleSheet(R"(
		QToolButton:checked {
			background-color: rgb(0,120,215);
			color: white;
		}
		QToolButton:hover {
			background-color: rgb(0, 140, 235);
			color: white;
		}
	)");

    button->setIconSize(QSize(40, 40));

    button->setToolButtonStyle(Qt::ToolButtonIconOnly);

	m_pHBoxLayout->addWidget(button);
	button->setDefaultAction(action);
	button->setPopupMode(QToolButton::InstantPopup);
	adjustSize();

}

void MxViewToolBar::addWidget(QWidget* widget)
{
	widget->setFixedHeight(44);
	m_pHBoxLayout->addWidget(widget);
	adjustSize();
}
