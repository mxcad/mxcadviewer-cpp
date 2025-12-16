/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx3dSignalTransfer.h"

Mx3dSignalTransfer& Mx3dSignalTransfer::getInstance()
{
	static Mx3dSignalTransfer instance;
	return instance;
}

Mx3dSignalTransfer::Mx3dSignalTransfer(QObject* parent)
	:QObject(parent)
{
}

Mx3dSignalTransfer::~Mx3dSignalTransfer()
{
}
