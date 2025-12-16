/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once
#include <QString>
#include <QHash>
#include <functional>
#include "Mx2dCustomAnnotation.h"

class Mx2dAnnotationFactory {
public:
	using CreatorFunc = std::function<Mx2dCustomAnnotation* ()>;

	static Mx2dAnnotationFactory& instance();

	void registerAnnotation(const QString& type, CreatorFunc creator);

	Mx2dCustomAnnotation* createAnnotation(const QString& type);

private:
	Mx2dAnnotationFactory() = default;
	~Mx2dAnnotationFactory() = default;
	Mx2dAnnotationFactory(const Mx2dAnnotationFactory&) = delete;
	Mx2dAnnotationFactory& operator=(const Mx2dAnnotationFactory&) = delete;

	QHash<QString, CreatorFunc> m_creatorMap;
};

#define REGISTER_ANNOTATION(typeStr, ClassName) \
namespace { \
    struct AnnotationRegistrar_##ClassName { \
        AnnotationRegistrar_##ClassName() { \
            Mx2dAnnotationFactory::instance().registerAnnotation( \
                QString(typeStr), []() { return new ClassName(); } \
            ); \
        } \
    }; \
    static AnnotationRegistrar_##ClassName s_annotationRegistrar_##ClassName; \
}