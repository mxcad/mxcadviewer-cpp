/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "MxSignalTransfer.h"

MxSignalTransfer& MxSignalTransfer::getInstance()
{
	static MxSignalTransfer instance;
	return instance;
}

MxSignalTransfer::MxSignalTransfer(QObject* parent)
	:QObject(parent)
{
}

MxSignalTransfer::~MxSignalTransfer()
{
}
