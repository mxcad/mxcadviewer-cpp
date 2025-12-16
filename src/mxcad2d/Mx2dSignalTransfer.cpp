/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dSignalTransfer.h"

Mx2dSignalTransfer& Mx2dSignalTransfer::getInstance()
{
	static Mx2dSignalTransfer instance;
	return instance;
}

Mx2dSignalTransfer::Mx2dSignalTransfer(QObject* parent)
	:QObject(parent)
{
}

Mx2dSignalTransfer::~Mx2dSignalTransfer()
{
}
