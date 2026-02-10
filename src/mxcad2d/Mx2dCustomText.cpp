/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dCustomText.h"
#include "Mx2dUtils.h"
#include "Mx2dAnnotationFactory.h"

REGISTER_ANNOTATION("text", Mx2dCustomText);
ACRX_DXF_DEFINE_MEMBERS(Mx2dCustomText, Mx2dCustomAnnotation,
	McDb::kDHL_CURRENT, McDb::kMReleaseCurrent,
	McDbProxyEntity::kAllAllowedBits, TextMark,
	Text Mark Entity);


Mx2dCustomText::Mx2dCustomText(void)
{
	setType("text");
}

Mx2dCustomText::Mx2dCustomText(const QString& textStr, const McGePoint3d& dimPt, double textHeight)
	:Mx2dCustomAnnotation(textHeight), m_text(textStr), m_dimPt(dimPt)
{
	setType("text");
}

Mx2dCustomText::~Mx2dCustomText(void)
{
}


Mdesk::Boolean Mx2dCustomText::worldDraw(McGiWorldDraw* wd)
{
	McDbText* pText = createText();
	pText->worldDraw(wd);
	delete pText;
	return Mdesk::kTrue;
}

Mcad::ErrorStatus Mx2dCustomText::getGripPoints(McGePoint3dArray& gripPoints, McGeIntArray& osnapModes, McGeIntArray& geomIds) const
{
	assertReadEnabled();
	gripPoints.append(m_dimPt);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomText::moveGripPointsAt(const McGeIntArray& indices, const McGeVector3d& offset)
{
	assertWriteEnabled();
	int iIndex = indices[0];
	switch (iIndex)
	{
	case 0:
		m_dimPt = m_dimPt + offset;
		break;
	}
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomText::getGeomExtents(McDbExtents& extents, bool roughCalculation) const
{
	assertReadEnabled();

	McDbText* pText = createText();
	McDbExtents ext;
	pText->getGeomExtents(ext, roughCalculation);
	delete pText;
	extents.addExt(ext);


	return Mcad::eOk;
}

#define TEXT_VERSION 1

Mcad::ErrorStatus Mx2dCustomText::dwgInFields(McDbDwgFiler* pFiler)
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
	Mx2dCustomAnnotation::dwgInFields(pFiler);
	int lVar = 1;
	pFiler->readInt(&lVar);
	MxString mxStr;
	pFiler->readString(mxStr);
	m_text = QString::fromLocal8Bit(mxStr.c_str());
	pFiler->readPoint3d(&m_dimPt);

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomText::dwgOutFields(McDbDwgFiler* pFiler) const
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
	Mx2dCustomAnnotation::dwgOutFields(pFiler);
	pFiler->writeInt(TEXT_VERSION);
	pFiler->writeString(m_text.toLocal8Bit().constData());
	pFiler->writePoint3d(m_dimPt);

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomText::explode(McDbVoidPtrArray& entitySet) const
{
	McDbText* pText = createText();
	entitySet.append(pText);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomText::transformBy(const McGeMatrix3d& xform)
{
	assertWriteEnabled();
	m_dimPt.transformBy(xform);
	return Mcad::eOk;
}

void Mx2dCustomText::fromJson(const QJsonObject& jsonObject)
{
	assertWriteEnabled();
	Mx2dCustomAnnotation::fromJson(jsonObject);
	m_text = jsonObject["text"].toString();
	m_dimPt = Mx2d::jsonArray2dToPoint3d(jsonObject["dimPt"].toArray());
}

QJsonObject Mx2dCustomText::toJson() const
{
	assertReadEnabled();
	QJsonObject jsonObject = Mx2dCustomAnnotation::toJson();
	jsonObject["text"] = m_text;
	jsonObject["dimPt"] = Mx2d::point3dToJsonArray2d(m_dimPt);

	return jsonObject;
}

Mx2d::TextInfoList Mx2dCustomText::findText(const QString& text, bool isExactMatch) const
{
	assertReadEnabled();
	McDbExtents ext;
	McDbText* pText = createText();
	pText->getGeomExtents(ext, false);
	QString textStr = QString::fromLocal8Bit(pText->textString());
	McGePoint3d minPt = ext.minPoint();
	McGePoint3d maxPt = ext.maxPoint();
	Mx2d::Extents extents{ minPt.x, minPt.y,maxPt.x, maxPt.y };
	delete pText;
	if ((isExactMatch && (text != textStr)) ||
		(!isExactMatch && !textStr.contains(text, Qt::CaseInsensitive)))
	{
		return {};
	}

	return { {textStr , extents} };
}

QString Mx2dCustomText::textString() const
{
	assertReadEnabled();
	return m_text;
}

void Mx2dCustomText::setDimPt(const McGePoint3d& dimPt)
{
	assertWriteEnabled();
	m_dimPt = dimPt;
}

McGePoint3d Mx2dCustomText::dimPt() const
{
	assertReadEnabled();
	return m_dimPt;
}

void Mx2dCustomText::setTextString(const QString& textStr)
{
	assertWriteEnabled();
	m_text = textStr;
}

McDbText* Mx2dCustomText::createText() const
{

	McDbText* pText = new McDbText();
	pText->setTextStyle(Mx::mcdbCurDwg()->textstyle());
	pText->setTextString(m_text.toLocal8Bit().constData());
	pText->setHeight(textHeight());
	pText->setAlignmentPoint(m_dimPt);
	return pText;
}
