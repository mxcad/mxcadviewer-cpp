/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dCustomAnnotation.h"

ACRX_DXF_DEFINE_MEMBERS(Mx2dCustomAnnotation, McDbEntity,
	McDb::kDHL_CURRENT, McDb::kMReleaseCurrent,
	McDbProxyEntity::kAllAllowedBits, AnnotationMark,
	Annotation Mark Entity);

Mx2dCustomAnnotation::Mx2dCustomAnnotation(void)
	:m_textHeight(3.0)
{
}

Mx2dCustomAnnotation::Mx2dCustomAnnotation(double textHeight)
	:m_textHeight(textHeight)
{
}


Mx2dCustomAnnotation::~Mx2dCustomAnnotation(void)
{
}
#define ANNOTATION_VERSION 1
Mcad::ErrorStatus Mx2dCustomAnnotation::dwgInFields(McDbDwgFiler* pFiler)
{
	assertWriteEnabled();
	if (pFiler->filerType() != McDb::kCustomEntityFileFiler)
	{
		Mcad::ErrorStatus es;
		if ((es = McDbEntity::dwgInFields(pFiler)) != Mcad::eOk)
		{
			return es;
		}
	}

	int lVar = 1;
	pFiler->readInt(&lVar);
	pFiler->readDouble(&m_textHeight);
	pFiler->readDouble(&m_dimRatio);
	MxString mxStr;
	pFiler->readString(mxStr);
	m_category = QString::fromLocal8Bit(mxStr.c_str());
	pFiler->readString(mxStr);
	m_type = QString::fromLocal8Bit(mxStr.c_str());
    pFiler->readString(mxStr);
    m_layout = QString::fromLocal8Bit(mxStr.c_str());

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomAnnotation::dwgOutFields(McDbDwgFiler* pFiler) const
{
	assertReadEnabled();
	if (pFiler->filerType() != McDb::kCustomEntityFileFiler)
	{
		Mcad::ErrorStatus es;
		if ((es = McDbEntity::dwgOutFields(pFiler)) != Mcad::eOk)
		{
			return es;
		}
	}


	pFiler->writeInt(ANNOTATION_VERSION);
	pFiler->writeDouble(m_textHeight);
	pFiler->writeDouble(m_dimRatio);
	pFiler->writeString(MxString(m_category.toLocal8Bit().constData()));
	pFiler->writeString(MxString(m_type.toLocal8Bit().constData()));
    pFiler->writeString(MxString(m_layout.toLocal8Bit().constData()));
	return Mcad::eOk;
}

void Mx2dCustomAnnotation::setTextHeight(double textHeight)
{
	assertWriteEnabled();
	m_textHeight = textHeight;
}

double Mx2dCustomAnnotation::textHeight() const
{
	assertReadEnabled();
	return m_textHeight;
}

void Mx2dCustomAnnotation::setDimRatio(double dimRatio)
{
	assertWriteEnabled();
	m_dimRatio = dimRatio;
}

double Mx2dCustomAnnotation::dimRatio() const
{
	assertReadEnabled();
	return m_dimRatio;
}

void Mx2dCustomAnnotation::setCategory(const QString& category)
{
	assertWriteEnabled();
	m_category = category;
}

QString Mx2dCustomAnnotation::category() const
{
	assertReadEnabled();
	return m_category;
}

void Mx2dCustomAnnotation::setType(const QString& type)
{
	assertWriteEnabled();
	m_type = type;
}

QString Mx2dCustomAnnotation::type() const
{
	assertReadEnabled();
	return m_type;
}

void Mx2dCustomAnnotation::setLayout(const QString& layoutName)
{
    assertWriteEnabled();
	m_layout = layoutName;
}

QString Mx2dCustomAnnotation::layout() const
{
    assertReadEnabled();
    return m_layout;
}

void Mx2dCustomAnnotation::fromJson(const QJsonObject& jsonObject)
{
	assertWriteEnabled();
	m_textHeight = jsonObject["textHeight"].toDouble();
	m_dimRatio = jsonObject["dimRatio"].toDouble();
	m_category = jsonObject["category"].toString();
	m_type = jsonObject["type"].toString();
    m_layout = jsonObject["layout"].toString();
}

QJsonObject Mx2dCustomAnnotation::toJson() const
{
	assertReadEnabled();
	QJsonObject jsonObject;
	jsonObject["textHeight"] = m_textHeight;
	jsonObject["dimRatio"] = m_dimRatio;
	jsonObject["category"] = m_category;
	jsonObject["type"] = m_type;
    jsonObject["layout"] = m_layout;
	return jsonObject;
}

Mx2d::TextInfoList Mx2dCustomAnnotation::findText(const QString& text, bool isExactMatch) const
{
	return {};
}
