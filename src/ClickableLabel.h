/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QLabel>
#include <QWidget>
#include <QMouseEvent>

/**
 * @class ClickableLabel
 * @brief A QLabel that emits a `clicked()` signal when it is pressed.
 */
class ClickableLabel : public QLabel
{
	Q_OBJECT

public:
	explicit ClickableLabel(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	~ClickableLabel() override;

signals:
	void clicked();

protected:
	void mousePressEvent(QMouseEvent* event) override;
};