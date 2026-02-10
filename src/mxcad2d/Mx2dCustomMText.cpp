/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dCustomMText.h"
#include "Mx2dUtils.h"
#include "Mx2dAnnotationFactory.h"

REGISTER_ANNOTATION("mtext", Mx2dCustomMText);
ACRX_DXF_DEFINE_MEMBERS(Mx2dCustomMText, Mx2dCustomAnnotation,
	McDb::kDHL_CURRENT, McDb::kMReleaseCurrent,
	McDbProxyEntity::kAllAllowedBits, MTextMark,
	MText Mark Entity);


Mx2dCustomMText::Mx2dCustomMText(void)
{
	setType("mtext");
}

Mx2dCustomMText::Mx2dCustomMText(const QString& textStr, const McGePoint3d& dimPt, double textHeight)
	: Mx2dCustomAnnotation(textHeight), m_text(textStr), m_dimPt(dimPt)
{
	setType("mtext");
}

Mx2dCustomMText::~Mx2dCustomMText(void)
{
}

Mdesk::Boolean Mx2dCustomMText::worldDraw(McGiWorldDraw* wd)
{
	McDbVoidPtrArray texts = createTexts();

	for (int i = 0; i < texts.length(); i++) {
		McDbText* pText = static_cast<McDbText*>(texts[i]);
		pText->worldDraw(wd);
		delete pText;
	}
	return Mdesk::kTrue;
}

Mcad::ErrorStatus Mx2dCustomMText::getGripPoints(McGePoint3dArray& gripPoints, McGeIntArray& osnapModes, McGeIntArray& geomIds) const
{
	assertReadEnabled();
	gripPoints.append(m_dimPt);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomMText::moveGripPointsAt(const McGeIntArray& indices, const McGeVector3d& offset)
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

Mcad::ErrorStatus Mx2dCustomMText::getGeomExtents(McDbExtents& extents, bool roughCalculation) const
{
	assertReadEnabled();

	McDbVoidPtrArray texts = createTexts();

	for (int i = 0; i < texts.length(); i++) {
		McDbText* pText = static_cast<McDbText*>(texts[i]);
		McDbExtents ext;
		pText->getGeomExtents(ext, roughCalculation);
		delete pText;
		extents.addExt(ext);
	}


	return Mcad::eOk;
}

#define MTEXT_VERSION 1

Mcad::ErrorStatus Mx2dCustomMText::dwgInFields(McDbDwgFiler* pFiler)
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

Mcad::ErrorStatus Mx2dCustomMText::dwgOutFields(McDbDwgFiler* pFiler) const
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
	pFiler->writeInt(MTEXT_VERSION);
	pFiler->writeString(m_text.toLocal8Bit().constData());
	pFiler->writePoint3d(m_dimPt);

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomMText::explode(McDbVoidPtrArray& entitySet) const
{
	McDbVoidPtrArray texts = createTexts();
	entitySet.append(texts);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomMText::transformBy(const McGeMatrix3d& xform)
{
	assertWriteEnabled();
	m_dimPt.transformBy(xform);
	return Mcad::eOk;
}

void Mx2dCustomMText::fromJson(const QJsonObject& jsonObject)
{
	assertWriteEnabled();
	Mx2dCustomAnnotation::fromJson(jsonObject);
	m_text = jsonObject["text"].toString();
	m_dimPt = Mx2d::jsonArray2dToPoint3d(jsonObject["dimPoint"].toArray());
}

QJsonObject Mx2dCustomMText::toJson() const
{
	assertReadEnabled();
	QJsonObject jsonObject = Mx2dCustomAnnotation::toJson();

	jsonObject["text"] = m_text;
	jsonObject["dimPoint"] = Mx2d::point3dToJsonArray2d(m_dimPt);

	return jsonObject;
}

Mx2d::TextInfoList Mx2dCustomMText::findText(const QString& text, bool isExactMatch) const
{
	assertReadEnabled();
	Mx2d::TextInfoList res;
	McDbVoidPtrArray arr = createTexts();
	for (int i = 0; i < arr.length(); ++i)
	{
		McDbExtents ext;
		McDbText* pText = (McDbText*)arr[i];
		pText->getGeomExtents(ext, false);
		QString textStr = QString::fromLocal8Bit(pText->textString());
		McGePoint3d minPt = ext.minPoint();
		McGePoint3d maxPt = ext.maxPoint();
		Mx2d::Extents extents{ minPt.x, minPt.y,maxPt.x, maxPt.y };
		delete pText;
		if ((isExactMatch && (text != textStr)) ||
			(!isExactMatch && !textStr.contains(text, Qt::CaseInsensitive)))
		{
			continue;;
		}
		res.append({ textStr , extents });
	}


	return res;
}

void Mx2dCustomMText::setContents(const QString& textStr)
{
	assertWriteEnabled();
	m_text = textStr;
}

QString Mx2dCustomMText::contents() const
{
	assertReadEnabled();
	return m_text;
}

void Mx2dCustomMText::setDimPt(const McGePoint3d& dimPt)
{
	assertWriteEnabled();
	m_dimPt = dimPt;
}

McGePoint3d Mx2dCustomMText::dimPt() const
{
	assertReadEnabled();
	return m_dimPt;
}


McDbVoidPtrArray Mx2dCustomMText::createTexts() const
{
	McDbVoidPtrArray texts;

	McDbMText* pMText = new McDbMText();
	pMText->setTextStyle(Mx::mcdbCurDwg()->textstyle());
	pMText->setContents(m_text.toLocal8Bit().constData());
	pMText->setTextHeight(textHeight());
	pMText->setLocation(m_dimPt);

	pMText->explode(texts);
	delete pMText;
	return texts;
}
