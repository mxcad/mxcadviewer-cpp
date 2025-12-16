/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx3dExplodeSlider.h"
#include <QPainter>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>

Mx3dExplodeSlider::Mx3dExplodeSlider(QWidget* parent)
    : QWidget(parent)
    , horizontalLayout(nullptr)
    , horizontalSlider(nullptr)
    , spinBox(nullptr)
    , btn_restore(nullptr)
    , btn_explode(nullptr)
{
    initUi();

    setFixedSize(450, 50);
    setCursor(QCursor(Qt::ArrowCursor));
    connectSignals();
}

Mx3dExplodeSlider::~Mx3dExplodeSlider()
{
}

void Mx3dExplodeSlider::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    // Background color
    painter.fillRect(rect(), QColor(92, 92, 92, 100));
    painter.end();
}

void Mx3dExplodeSlider::initUi()
{
    if (objectName().isEmpty())
        setObjectName("Mx3dExplodeSliderClass");

    resize(350, 60);
    setWindowTitle(tr("Exploded View"));

    // --- Layout ---
    horizontalLayout = new QHBoxLayout(this);
    horizontalLayout->setSpacing(6);
    horizontalLayout->setContentsMargins(11, 11, 11, 11);
    horizontalLayout->setObjectName("horizontalLayout");

    // --- Slider ---
    horizontalSlider = new QSlider(this);
    horizontalSlider->setObjectName("horizontalSlider");
    horizontalSlider->setMaximum(100);
    horizontalSlider->setOrientation(Qt::Horizontal);

    // Slider Stylesheet
    horizontalSlider->setStyleSheet(R"(
        /* Basic setup */
        QSlider { padding: 10px 0; }
        
        /* Groove style */
        QSlider::groove:horizontal {
            border: 1px solid #cccccc;
            background: #f0f0f0;
            height: 8px;
            border-radius: 4px;
        }
        
        /* Sub-page (filled area) style */
        QSlider::sub-page:horizontal {
            background: rgb(56, 142, 138);
            border: 1px solid rgb(46, 122, 118);
            height: 8px;
            border-radius: 4px;
        }
        
        /* Add-page (empty area) style */
        QSlider::add-page:horizontal {
            background: #f0f0f0;
            border: 1px solid #cccccc;
            height: 8px;
            border-radius: 4px;
        }
        
        /* Handle style */
        QSlider::handle:horizontal {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, 
                                       stop:0 rgb(76, 162, 158), 
                                       stop:1 rgb(56, 142, 138));
            border: 1px solid rgb(46, 122, 118);
            width: 20px;
            margin: -6px 0;
            border-radius: 10px;
            box-shadow: 0 1px 3px rgba(0, 0, 0, 0.2);
        }
        
        /* Handle hover style */
        QSlider::handle:horizontal:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, 
                                       stop:0 rgb(86, 172, 168), 
                                       stop:1 rgb(66, 152, 148));
            border: 1px solid rgb(36, 112, 108);
            width: 22px;
            margin: -7px 0;
            box-shadow: 0 1px 4px rgba(0, 0, 0, 0.3);
        }
        
        /* Handle pressed style */
        QSlider::handle:horizontal:pressed {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, 
                                       stop:0 rgb(56, 142, 138), 
                                       stop:1 rgb(36, 122, 118));
            border: 1px solid rgb(26, 102, 98);
            width: 24px;
            margin: -8px 0;
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.4);
        }
        
        /* Tickmark style */
        QSlider::tickmark:horizontal {
            background: #bbbbbb;
            width: 1px;
            height: 6px;
            margin: 8px 0;
        }
    )");
    horizontalLayout->addWidget(horizontalSlider);

    // --- SpinBox ---
    spinBox = new QSpinBox(this);
    spinBox->setObjectName("spinBox");
    spinBox->setWrapping(false);
    spinBox->setMaximum(100);
    spinBox->setSuffix(tr("%"));
    horizontalLayout->addWidget(spinBox);

    // --- Restore Button ---
    btn_restore = new QPushButton(this);
    btn_restore->setObjectName("btn_restore");
    btn_restore->setCursor(QCursor(Qt::PointingHandCursor));
    btn_restore->setText(tr("Restore"));

    // Button Stylesheet
    QString btnStyle = R"(
        QPushButton {
            border: 1px solid rgb( 56,142,138); 
            background-color: rgb( 56,142,138); 
            color: white; 
            border-radius: 2px;
            font-size: 16px;
            padding: 5px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: rgba( 56,142,138, 0.8); 
        }
        QPushButton:pressed {
            background-color: rgb( 56,142,138); 
        }
    )";

    btn_restore->setStyleSheet(btnStyle);
    horizontalLayout->addWidget(btn_restore);

    // --- Explode Button ---
    btn_explode = new QPushButton(this);
    btn_explode->setObjectName("btn_explode");
    btn_explode->setCursor(QCursor(Qt::PointingHandCursor));
    btn_explode->setText(tr("Explode"));
    btn_explode->setStyleSheet(btnStyle); // Reusing same style
    horizontalLayout->addWidget(btn_explode);

    connect(horizontalSlider, SIGNAL(valueChanged(int)), spinBox, SLOT(setValue(int)));
    connect(spinBox, SIGNAL(valueChanged(int)), horizontalSlider, SLOT(setValue(int)));
}

void Mx3dExplodeSlider::connectSignals()
{
    connect(btn_restore, &QPushButton::clicked, [this]() {
        horizontalSlider->setValue(0);
        });

    connect(btn_explode, &QPushButton::clicked, [this]() {
        horizontalSlider->setValue(horizontalSlider->maximum());
        });

    connect(horizontalSlider, &QSlider::valueChanged, this, &Mx3dExplodeSlider::valueChanged);
}