/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QTreeWidget>


class QMouseEvent;

class MxTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    MxTreeWidget(QWidget* parent = nullptr);
    ~MxTreeWidget();

protected:
    void mousePressEvent(QMouseEvent* event) override;

};

