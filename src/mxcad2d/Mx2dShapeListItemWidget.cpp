/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dShapeListItemWidget.h"
#include <QtWidgets>

Mx2dShapeListItemWidget::Mx2dShapeListItemWidget(const QString& name, int count, QWidget* parent)
	: QWidget(parent), m_sName(name), m_nCount(count)
{
	QHBoxLayout* pItemLayout = new QHBoxLayout(this);
	pItemLayout->setContentsMargins(15, 10, 15, 10);

	QVBoxLayout* pInfoLayout = new QVBoxLayout();
	QLabel* pNameLabel = new QLabel(m_sName);
	pNameLabel->setStyleSheet("font-size: 16px; color: #333;");
	QLabel* pCountLabel = new QLabel(QString(tr("Count: %1")).arg(m_nCount));
	pCountLabel->setStyleSheet("color: #D9534F;");
	pInfoLayout->addWidget(pNameLabel);
	pInfoLayout->addWidget(pCountLabel);

	m_pViewButton = new QPushButton(tr("View"));
	m_pDeleteButton = new QPushButton(tr("Delete"));
	m_pViewButton->setFixedSize(60, 28);
	m_pDeleteButton->setFixedSize(60, 28);

	m_pViewButton->setStyleSheet("QPushButton { border: 1px solid #337AB7; color: #337AB7; background-color: white; border-radius: 4px; }"
		"QPushButton:hover { background-color: #E6F0F8; }");
	m_pDeleteButton->setStyleSheet("QPushButton { border: 1px solid #C9302C; color: #C9302C; background-color: white; border-radius: 4px; }"
		"QPushButton:hover { background-color: #FAEBEA; }");

	pItemLayout->addLayout(pInfoLayout);
	pItemLayout->addStretch();
	pItemLayout->addWidget(m_pViewButton);
	pItemLayout->addWidget(m_pDeleteButton);

	connect(m_pViewButton, &QPushButton::clicked, this, [this]() {
		emit viewClicked(m_sName, m_nCount);
		});
	connect(m_pDeleteButton, &QPushButton::clicked, this, &Mx2dShapeListItemWidget::deleteClicked);

	m_pViewButton->installEventFilter(this);
	m_pDeleteButton->installEventFilter(this);
}

bool Mx2dShapeListItemWidget::eventFilter(QObject* watched, QEvent* event)
{
	const QString defaultHint = tr("Hint: Double-click table item or click \"View\" button to see detailed positions of recognized shapes");

	if (watched == m_pViewButton) {
		if (event->type() == QEvent::Enter) { // Mouse enter
			emit buttonHoverChanged(tr("Hint: Click button to view detailed positions of the shape"));
		}
		else if (event->type() == QEvent::Leave) { // Mouse leave
			emit buttonHoverChanged(defaultHint);
		}
	}
	else if (watched == m_pDeleteButton) {
		if (event->type() == QEvent::Enter) {
			emit buttonHoverChanged(tr("Hint: Click button to delete the shape"));
		}
		else if (event->type() == QEvent::Leave) {
			emit buttonHoverChanged(defaultHint);
		}
	}

	return QWidget::eventFilter(watched, event);
}