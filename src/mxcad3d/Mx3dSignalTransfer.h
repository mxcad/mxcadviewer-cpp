/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <memory>	
#include <vector>
#include <QObject>

class Mx3dSignalTransfer : public QObject
{
	Q_OBJECT
public:

	static Mx3dSignalTransfer& getInstance();

	Mx3dSignalTransfer(const Mx3dSignalTransfer&) = delete;
	Mx3dSignalTransfer& operator=(const Mx3dSignalTransfer&) = delete;

private:
	Mx3dSignalTransfer(QObject* parent = nullptr);
	~Mx3dSignalTransfer();

signals:
	void signalSetBgColor(const QColor& bgColor); 
	void signalSetGradientBgColor1(const QColor& gradientBgColor1); 
	void signalSetGradientBgColor2(const QColor& gradientBgColor2); 
	void signalSetGradientMode(int gradientMode); 
	void signalViewSetBgColor();
};
