/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx3dSetBgColorDialog.h"
#include "Mx3dBgColorManager.h"
#include "Mx3dUtils.h"

#include <QColorDialog>
#include <QTabWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QSpacerItem>
#include <QDebug>
#include "Mx3dSignalTransfer.h"

Mx3dSetBgColorDialog::Mx3dSetBgColorDialog(QWidget* parent)
    : QDialog(parent)
{
    setupUi();
    loadInitialConfig();
    connectSignals();
}

Mx3dSetBgColorDialog::~Mx3dSetBgColorDialog()
{
}

void Mx3dSetBgColorDialog::setupUi()
{
    setWindowTitle(tr("Set Background Color"));
    setFixedSize(300, 220);
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);

    auto* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setStyleSheet("font-size: 14px;");
    mainLayout->addWidget(m_tabWidget);

    QWidget* solidTab = createSolidColorTab();
    QWidget* gradientTab = createGradientColorTab();

    m_tabWidget->addTab(solidTab, tr("Solid Color"));
    m_tabWidget->addTab(gradientTab, tr("Gradient"));
}

QWidget* Mx3dSetBgColorDialog::createSolidColorTab()
{
    auto* tab = new QWidget();
    auto* layout = new QVBoxLayout(tab);

    auto* colorRowLayout = new QHBoxLayout();
    m_solidColorPreview = new QWidget();
    m_solidColorPreview->setFixedSize(40, 40);

    auto* selectButton = new QPushButton(tr("Select Color..."));

    colorRowLayout->addStretch();
    colorRowLayout->addWidget(m_solidColorPreview);
    colorRowLayout->addSpacing(20);
    colorRowLayout->addWidget(selectButton);
    colorRowLayout->addStretch();

    auto* restoreButton = new QPushButton(tr("Restore Default"));

    layout->addStretch();
    layout->addLayout(colorRowLayout);
    layout->addSpacing(20);
    layout->addWidget(restoreButton, 0, Qt::AlignCenter);
    layout->addStretch();

    connect(selectButton, &QPushButton::clicked, this, &Mx3dSetBgColorDialog::onSelectSolidBgColor);
    connect(restoreButton, &QPushButton::clicked, this, &Mx3dSetBgColorDialog::onRestoreSolidBgDefault);

    return tab;
}

QWidget* Mx3dSetBgColorDialog::createGradientColorTab()
{
    auto* tab = new QWidget();
    auto* mainLayout = new QVBoxLayout(tab);
    mainLayout->setSpacing(15);
    mainLayout->addStretch(1);

    auto* formLayout = new QFormLayout();
    formLayout->setHorizontalSpacing(15);
    formLayout->setVerticalSpacing(10);
    formLayout->setLabelAlignment(Qt::AlignRight);

    auto* color1Container = new QWidget();
    auto* color1RowLayout = new QHBoxLayout(color1Container);
    color1RowLayout->setContentsMargins(0, 0, 0, 0);
    m_gradientColor1Preview = new QWidget();
    m_gradientColor1Preview->setFixedSize(40, 30);
    auto* selectButton1 = new QPushButton(tr("Select..."));
    color1RowLayout->addWidget(m_gradientColor1Preview);
    color1RowLayout->addWidget(selectButton1);
    color1RowLayout->addStretch();

    formLayout->addRow(tr("Start Color:"), color1Container);

    auto* color2Container = new QWidget();
    auto* color2RowLayout = new QHBoxLayout(color2Container);
    color2RowLayout->setContentsMargins(0, 0, 0, 0);
    m_gradientColor2Preview = new QWidget();
    m_gradientColor2Preview->setFixedSize(40, 30);
    auto* selectButton2 = new QPushButton(tr("Select..."));
    color2RowLayout->addWidget(m_gradientColor2Preview);
    color2RowLayout->addWidget(selectButton2);
    color2RowLayout->addStretch();

    formLayout->addRow(tr("End Color:"), color2Container);


    m_gradientModeComboBox = new QComboBox();
    m_gradientModeComboBox->addItems({
        tr("Horizontal"), tr("Vertical"), tr("Diagonal 1"), tr("Diagonal 2"),
        tr("Corner 1"), tr("Corner 2"), tr("Corner 3"), tr("Corner 4")
        });
    formLayout->addRow(tr("Gradient Mode:"), m_gradientModeComboBox);

    auto* restoreButton = new QPushButton(tr("Restore Default"));

    mainLayout->addLayout(formLayout);
    mainLayout->addStretch(2);
    mainLayout->addWidget(restoreButton, 0, Qt::AlignCenter);
    mainLayout->addStretch(1);

    connect(selectButton1, &QPushButton::clicked, this, &Mx3dSetBgColorDialog::onSelectGradientBgColor1);
    connect(selectButton2, &QPushButton::clicked, this, &Mx3dSetBgColorDialog::onSelectGradientBgColor2);
    connect(restoreButton, &QPushButton::clicked, this, &Mx3dSetBgColorDialog::onRestoreGradientBgDefault);

    return tab;
}
void Mx3dSetBgColorDialog::loadInitialConfig()
{
    int bgColorMode, gradientMode;
    QColor bgColor, gradientBgColor1, gradientBgColor2;

    Mx3dBgColorManager::instance()->loadConfig(bgColorMode, gradientMode, bgColor, gradientBgColor1, gradientBgColor2);

    m_solidBgColor = bgColor;
    m_gradientMode = gradientMode;
    m_gradientBgColor1 = gradientBgColor1;
    m_gradientBgColor2 = gradientBgColor2;

    updateColorPreview(m_solidColorPreview, m_solidBgColor);
    updateColorPreview(m_gradientColor1Preview, m_gradientBgColor1);
    updateColorPreview(m_gradientColor2Preview, m_gradientBgColor2);

    m_tabWidget->setCurrentIndex(bgColorMode);
    m_gradientModeComboBox->setCurrentIndex(qMax(0, gradientMode - 1));
}

void Mx3dSetBgColorDialog::connectSignals()
{
    connect(m_gradientModeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Mx3dSetBgColorDialog::onSetGradientMode);
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &Mx3dSetBgColorDialog::onTabChanged);
}

void Mx3dSetBgColorDialog::updateColorPreview(QWidget* previewWidget, const QColor& color)
{
    if (!previewWidget) return;
    previewWidget->setStyleSheet(QString(
        "border: 1px solid #e0e0e0;"
        "background-color: %1;"
    ).arg(color.name()));
}

void Mx3dSetBgColorDialog::onSelectSolidBgColor()
{
    QColor color = QColorDialog::getColor(m_solidBgColor, this, tr("Select Background Color"));
    if (!color.isValid()) return;

    m_solidBgColor = color;
    updateColorPreview(m_solidColorPreview, color);
    emit Mx3dSignalTransfer::getInstance().signalSetBgColor(color);
}

void Mx3dSetBgColorDialog::onSelectGradientBgColor1()
{
    QColor color = QColorDialog::getColor(m_gradientBgColor1, this, tr("Select Gradient Start Color"));
    if (!color.isValid()) return;

    m_gradientBgColor1 = color;
    updateColorPreview(m_gradientColor1Preview, color);
    emit Mx3dSignalTransfer::getInstance().signalSetGradientBgColor1(color);
}

void Mx3dSetBgColorDialog::onSelectGradientBgColor2()
{
    QColor color = QColorDialog::getColor(m_gradientBgColor2, this, tr("Select Gradient End Color"));
    if (!color.isValid()) return;

    m_gradientBgColor2 = color;
    updateColorPreview(m_gradientColor2Preview, color);
    emit Mx3dSignalTransfer::getInstance().signalSetGradientBgColor2(color);
}

void Mx3dSetBgColorDialog::onSetGradientMode(int index)
{
    m_gradientMode = index + 1;
    emit Mx3dSignalTransfer::getInstance().signalSetGradientMode(m_gradientMode);
}

void Mx3dSetBgColorDialog::onRestoreSolidBgDefault()
{
    const QColor defaultColor(160, 160, 160);
    m_solidBgColor = defaultColor;
    updateColorPreview(m_solidColorPreview, defaultColor);
    emit Mx3dSignalTransfer::getInstance().signalSetBgColor(defaultColor);
}

void Mx3dSetBgColorDialog::onRestoreGradientBgDefault()
{
    const QColor defaultColor1(100, 100, 100);
    const QColor defaultColor2(200, 200, 200);

    m_gradientBgColor1 = defaultColor1;
    m_gradientBgColor2 = defaultColor2;
    m_gradientModeComboBox->setCurrentIndex(1);

    updateColorPreview(m_gradientColor1Preview, defaultColor1);
    updateColorPreview(m_gradientColor2Preview, defaultColor2);

    emit Mx3dSignalTransfer::getInstance().signalSetGradientBgColor1(defaultColor1);
    emit Mx3dSignalTransfer::getInstance().signalSetGradientBgColor2(defaultColor2);
}

void Mx3dSetBgColorDialog::onTabChanged(int index)
{
}