/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QObject>
#include <QSettings>
#include <QColor>

class Mx3dBgColorManager  : public QObject
{
	Q_OBJECT

public:
	static Mx3dBgColorManager* instance() {
		static Mx3dBgColorManager instance;
		return &instance;
	}

	Mx3dBgColorManager(const Mx3dBgColorManager&) = delete;
	Mx3dBgColorManager& operator=(const Mx3dBgColorManager&) = delete;

	void loadFromSettings();
	void saveToSettings();

	void saveConfig(int bgColorMode, int gradientMode, QColor bgColor, QColor gradientBgColor1, QColor gradientBgColor2);
	void loadConfig(int& bgColorMode, int& gradientMode, QColor& bgColor, QColor& gradientBgColor1, QColor& gradientBgColor2);

private slots:
	void onSetBgColor(const QColor& bgColor);
    void onSetGradientBgColor1(const QColor& gradientBgColor1);
	void onSetGradientBgColor2(const QColor& gradientBgColor2);
	void onSetGradientMode(int gradientMode);
private:
	Mx3dBgColorManager();
	~Mx3dBgColorManager() = default;

private:
	int   m_bgColorMode;
	int   m_gradientMode;
	QColor		m_bgColor;
	QColor		m_gradientBgColor1;
	QColor		m_gradientBgColor2;
	QSettings			m_settings{ "MxDraw", "MxCADViewer" };
};
