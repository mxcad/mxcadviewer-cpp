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
class QSlider;
class QSpinBox;
class QPushButton;

class Mx3dExplodeSlider : public QWidget
{
    Q_OBJECT

public:
    explicit Mx3dExplodeSlider(QWidget* parent = nullptr);
    ~Mx3dExplodeSlider();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void initUi();
    void connectSignals();

signals:
    void valueChanged(int value);

private:
    QHBoxLayout* horizontalLayout;
    QSlider* horizontalSlider;
    QSpinBox* spinBox;
    QPushButton* btn_restore;
    QPushButton* btn_explode;
};