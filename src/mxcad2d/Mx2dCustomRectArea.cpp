/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dCustomRectArea.h"
#include <QCoreApplication>
#include <QString>
#include "Mx2dUtils.h"
#include "Mx2dAnnotationFactory.h"

REGISTER_ANNOTATION("rectArea", Mx2dCustomRectArea);
ACRX_DXF_DEFINE_MEMBERS(Mx2dCustomRectArea, Mx2dCustomRect,
	McDb::kDHL_CURRENT, McDb::kMReleaseCurrent,
	McDbProxyEntity::kAllAllowedBits, RectAreaMark,
	RectArea Mark Entity);

Mx2dCustomRectArea::Mx2dCustomRectArea(void)
{
	setType("rectArea");
}

Mx2dCustomRectArea::Mx2dCustomRectArea(const McGePoint3d& pt1, const McGePoint3d& pt2, const McGePoint3d& textPos, double textHeight)
	: Mx2dCustomRect(pt1, pt2, textHeight), m_textPos(textPos)
{
	setType("rectArea");
}

Mx2dCustomRectArea::~Mx2dCustomRectArea(void)
{
}

Mdesk::Boolean Mx2dCustomRectArea::worldDraw(McGiWorldDraw* wd)
{

	Mx2dCustomRect::worldDraw(wd);

	McDbText* pText = createText();
	pText->worldDraw(wd);
	delete pText;

	return Mdesk::kTrue;
}

Mcad::ErrorStatus Mx2dCustomRectArea::getGripPoints(McGePoint3dArray& gripPoints, McGeIntArray& osnapModes, McGeIntArray& geomIds) const
{
	assertReadEnabled();
	Mx2dCustomRect::getGripPoints(gripPoints, osnapModes, geomIds);
	gripPoints.append(m_textPos);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomRectArea::moveGripPointsAt(const McGeIntArray& indices, const McGeVector3d& offset)
{
	assertWriteEnabled();
	Mx2dCustomRect::moveGripPointsAt(indices, offset);

	int iIndex = indices[0];
	switch (iIndex)
	{
	case 2:
		m_textPos = m_textPos + offset;
		break;
	}
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomRectArea::getGeomExtents(McDbExtents& extents, bool roughCalculation) const
{
	assertReadEnabled();

	Mx2dCustomRect::getGeomExtents(extents, roughCalculation);

	McDbText* pText = createText();
	McDbExtents textExtents;
	pText->getGeomExtents(textExtents, roughCalculation);
	delete pText;

	extents.addExt(textExtents);

	return Mcad::eOk;
}

#define RECTAREA_VERSION 1


Mcad::ErrorStatus Mx2dCustomRectArea::dwgInFields(McDbDwgFiler* pFiler)
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
	Mx2dCustomRect::dwgInFields(pFiler);
	int lVar = 1;
	pFiler->readInt(&lVar);
	pFiler->readPoint3d(&m_textPos);

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomRectArea::dwgOutFields(McDbDwgFiler* pFiler) const
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
	Mx2dCustomRect::dwgOutFields(pFiler);
	pFiler->writeInt(RECTAREA_VERSION);
	pFiler->writePoint3d(m_textPos);

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomRectArea::explode(McDbVoidPtrArray& entitySet) const
{
	assertReadEnabled();
	McDbVoidPtrArray tempSet;
	Mx2dCustomRect::explode(tempSet);
	McDbText* pText = createText();
	entitySet.append(tempSet);
	entitySet.append(pText);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomRectArea::transformBy(const McGeMatrix3d& xform)
{
	assertWriteEnabled();
	Mx2dCustomRect::transformBy(xform);
	m_textPos.transformBy(xform);
	return Mcad::eOk;
}

void Mx2dCustomRectArea::fromJson(const QJsonObject& jsonObject)
{
	assertWriteEnabled();
	Mx2dCustomRect::fromJson(jsonObject);
	m_textPos = Mx2d::jsonArray2dToPoint3d(jsonObject["textPos"].toArray());
}

QJsonObject Mx2dCustomRectArea::toJson() const
{
	assertReadEnabled();
	QJsonObject jsonObject = Mx2dCustomRect::toJson();
	jsonObject["textPos"] = Mx2d::point3dToJsonArray2d(m_textPos);

	return jsonObject;
}

Mx2d::TextInfoList Mx2dCustomRectArea::findText(const QString& text, bool isExactMatch) const
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

void Mx2dCustomRectArea::setTextPos(const McGePoint3d& pos)
{
	assertWriteEnabled();
	m_textPos = pos;
}

McGePoint3d Mx2dCustomRectArea::textPos() const
{
	assertReadEnabled();
	return m_textPos;
}



McDbText* Mx2dCustomRectArea::createText() const
{
	McDbObjectId textStyle = Mx::mcdbCurDwg()->textstyle();
	McDbText* pText = new McDbText();

	pText->setAlignmentPoint(m_textPos);
	pText->setHeight(textHeight());

	double area = getArea();
	double perimeter = getPerimeter();

	QString areaStr = QCoreApplication::translate("Mx2dCustomRectArea", "Area:%1, Perimeter:%2").arg(QString::number(area * m_dimRatio * m_dimRatio)).arg(QString::number(perimeter * m_dimRatio));

	pText->setTextString(areaStr.toLocal8Bit().constData());
	pText->setTextStyle(textStyle);

	return pText;
}


