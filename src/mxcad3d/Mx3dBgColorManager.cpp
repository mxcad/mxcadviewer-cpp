/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx3dBgColorManager.h"
#include <QDebug>
#include "Mx3dUtils.h"
#include <QColor>
#include "Mx3dSignalTransfer.h"

void Mx3dBgColorManager::loadFromSettings()
{
    m_bgColorMode = m_settings.value("BgColorMode", 1).value<int>();
    m_gradientMode = m_settings.value("GradientMode", 1).value<int>();
    m_bgColor = m_settings.value("BgColor", QColor(160, 160, 160)).value<QColor>();
	m_gradientBgColor1 = m_settings.value("GradientBgColor1", QColor(35, 159, 151)).value<QColor>();
	m_gradientBgColor2 = m_settings.value("GradientBgColor2", QColor(80, 194, 160)).value<QColor>();
    
	
    connect(&Mx3dSignalTransfer::getInstance(), &Mx3dSignalTransfer::signalSetBgColor, this, &Mx3dBgColorManager::onSetBgColor);
    connect(&Mx3dSignalTransfer::getInstance(), &Mx3dSignalTransfer::signalSetGradientBgColor1, this, &Mx3dBgColorManager::onSetGradientBgColor1);
    connect(&Mx3dSignalTransfer::getInstance(), &Mx3dSignalTransfer::signalSetGradientBgColor2, this, &Mx3dBgColorManager::onSetGradientBgColor2);
    connect(&Mx3dSignalTransfer::getInstance(), &Mx3dSignalTransfer::signalSetGradientMode, this, &Mx3dBgColorManager::onSetGradientMode);
}

void Mx3dBgColorManager::saveToSettings()
{
    m_settings.setValue("BgColorMode", m_bgColorMode);
    m_settings.setValue("GradientMode", m_gradientMode);
    m_settings.setValue("BgColor", m_bgColor);
	m_settings.setValue("GradientBgColor1", m_gradientBgColor1);
	m_settings.setValue("GradientBgColor2", m_gradientBgColor2);
    emit Mx3dSignalTransfer::getInstance().signalViewSetBgColor();
}

void Mx3dBgColorManager::saveConfig(int bgColorMode, int gradientMode, QColor bgColor, QColor gradientBgColor1, QColor gradientBgColor2)
{
    m_bgColorMode = bgColorMode;
    m_gradientMode = gradientMode;
    m_bgColor = bgColor;
	m_gradientBgColor1 = gradientBgColor1;
	m_gradientBgColor1 = gradientBgColor2;
    saveToSettings();
}

void Mx3dBgColorManager::loadConfig(int& bgColorMode, int& gradientMode, QColor& bgColor, QColor& gradientBgColor1, QColor& gradientBgColor2)
{
    bgColorMode = m_bgColorMode;
    gradientMode = m_gradientMode;
    bgColor = m_bgColor;
	gradientBgColor1 = m_gradientBgColor1;
	gradientBgColor2 = m_gradientBgColor2;
}

void Mx3dBgColorManager::onSetBgColor(const QColor& bgColor)
{
    m_bgColorMode = 0;
    m_bgColor = bgColor;
    saveToSettings();
}

void Mx3dBgColorManager::onSetGradientBgColor1(const QColor& gradientBgColor1)
{
    m_bgColorMode = 1;
    m_gradientBgColor1 = gradientBgColor1;
    saveToSettings();
}

void Mx3dBgColorManager::onSetGradientBgColor2(const QColor& gradientBgColor2)
{
    m_bgColorMode = 1;
    m_gradientBgColor2 = gradientBgColor2;
    saveToSettings();
}

void Mx3dBgColorManager::onSetGradientMode(int gradientMode)
{
    m_bgColorMode = 1;
    m_gradientMode = gradientMode;
    saveToSettings();
}

Mx3dBgColorManager::Mx3dBgColorManager()
{
    loadFromSettings();
}
