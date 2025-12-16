/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dAnnotationFactory.h"
#include <QDebug>

Mx2dAnnotationFactory& Mx2dAnnotationFactory::instance()
{
	static Mx2dAnnotationFactory factory;
	return factory;
}

void Mx2dAnnotationFactory::registerAnnotation(const QString& type, CreatorFunc creator)
{
	m_creatorMap[type] = creator;
}

Mx2dCustomAnnotation* Mx2dAnnotationFactory::createAnnotation(const QString& type)
{
	auto it = m_creatorMap.find(type);
	if (it != m_creatorMap.end()) {
		return it.value()();
	}
	qDebug() << "Unsupported annotation type: " << type;
	return nullptr;
}
