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

class MxSignalTransfer : public QObject
{
	Q_OBJECT
public:

	static MxSignalTransfer& getInstance();

	MxSignalTransfer(const MxSignalTransfer&) = delete;
	MxSignalTransfer& operator=(const MxSignalTransfer&) = delete;

private:
	MxSignalTransfer(QObject* parent = nullptr);
	~MxSignalTransfer();

signals:
	void signalShowPromptMessage(QWidget* target, const QString& message);
	void signalHidePromptMessage(QWidget* target);
	void signalMessageBoxInformation(QWidget* target, const QString& message);
	void signalOpenFile(const QString& filePath);
};
