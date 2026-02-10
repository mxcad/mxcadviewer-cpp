/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QWidget>
class QHBoxLayout;
class QToolButton;
class MxViewToolBar : public QWidget
{
	Q_OBJECT
public:
	MxViewToolBar(QWidget* parent = nullptr);
    ~MxViewToolBar();

	void setTranslucent(bool isTranslucent);
	bool isTranslucent() const;
protected:
	void paintEvent(QPaintEvent* event) override;
public:
	QHBoxLayout* m_pHBoxLayout;
public:
	QToolButton* addAction(QAction* action);
	void addWidget(QWidget* widget);
private:
    bool m_isTranslucent = false;
};

