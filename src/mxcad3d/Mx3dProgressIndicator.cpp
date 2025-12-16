/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx3dProgressIndicator.h"
#include <QDebug>

Mx3dProgressIndicator::Mx3dProgressIndicator(QObject* parent)
	: QObject(parent)
{
}

Mx3dProgressIndicator::~Mx3dProgressIndicator()
{
}

bool Mx3dProgressIndicator::UserBreak()
{
	return m_isAborted;
}

void Mx3dProgressIndicator::Reset()
{
	m_isAborted = false;
}

void Mx3dProgressIndicator::Show(const Message_ProgressScope& theScope, const bool isForce)
{
	if (theScope.Name() && (theScope.Name() != m_sLastStepName || isForce))
	{
		QString qstr("Transfering:");
		qstr += theScope.Name();
		emit progressDetailChanged(qstr);
		m_sLastStepName = theScope.Name();
	}
	const double pc = this->GetPosition(); // range [0,1]
	int val = static_cast<int>(pc * 100);
	if (val > 100)
		val = 100;
	else if (val < 0)
		val = 0;
	if (m_iLastProgress != val || isForce)
	{
		emit progressPercentChanged(val);
		m_iLastProgress = val;
	}
}

void Mx3dProgressIndicator::onAborted()
{
	m_isAborted = true;
}
