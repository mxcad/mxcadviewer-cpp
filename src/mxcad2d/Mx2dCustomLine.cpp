/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dCustomLine.h"
#include "Mx2dUtils.h"
#include "Mx2dAnnotationFactory.h"

REGISTER_ANNOTATION("line", Mx2dCustomLine);
ACRX_DXF_DEFINE_MEMBERS(Mx2dCustomLine, Mx2dCustomAnnotation,
	McDb::kDHL_CURRENT, McDb::kMReleaseCurrent,
	McDbProxyEntity::kAllAllowedBits, LineMark,
	Line Mark Entity);

Mx2dCustomLine::Mx2dCustomLine(void)
{
	setType("line");
}

Mx2dCustomLine::Mx2dCustomLine(const McGePoint3d& startPt, const McGePoint3d& endPt, double textHeight)
	:Mx2dCustomAnnotation(textHeight), m_startPt(startPt), m_endPt(endPt)
{
	setType("line");
}

Mx2dCustomLine::~Mx2dCustomLine(void)
{
}

Mdesk::Boolean Mx2dCustomLine::worldDraw(McGiWorldDraw* wd)
{
	McDbLine* pLine = createLine(m_startPt, m_endPt);
	pLine->worldDraw(wd);
	delete pLine;
	return Mdesk::kTrue;
}

Mcad::ErrorStatus Mx2dCustomLine::getGripPoints(McGePoint3dArray& gripPoints, McGeIntArray& osnapModes, McGeIntArray& geomIds) const
{
	assertReadEnabled();
	gripPoints.append(m_startPt);
	gripPoints.append(m_endPt);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomLine::moveGripPointsAt(const McGeIntArray& indices, const McGeVector3d& offset)
{
	assertWriteEnabled();
	int iIndex = indices[0];
	switch (iIndex)
	{
	case 0:
		m_startPt = m_startPt + offset;
		break;
	case 1:
		m_endPt = m_endPt + offset;
		break;
	}
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomLine::getGeomExtents(McDbExtents& extents, bool roughCalculation) const
{
	assertReadEnabled();
	extents.addPoint(m_startPt);
	extents.addPoint(m_endPt);

	return Mcad::eOk;
}

#define LINE_VERSION 1

Mcad::ErrorStatus Mx2dCustomLine::dwgInFields(McDbDwgFiler* pFiler)
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
	pFiler->readPoint3d(&m_startPt);
	pFiler->readPoint3d(&m_endPt);

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomLine::dwgOutFields(McDbDwgFiler* pFiler) const
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
	pFiler->writeInt(LINE_VERSION);
	pFiler->writePoint3d(m_startPt);
	pFiler->writePoint3d(m_endPt);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomLine::explode(McDbVoidPtrArray& entitySet) const
{
	McDbLine* pLine = createLine(m_startPt, m_endPt);
	entitySet.append(pLine);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomLine::transformBy(const McGeMatrix3d& xform)
{
	assertWriteEnabled();
	m_startPt.transformBy(xform);
	m_endPt.transformBy(xform);
	return Mcad::eOk;
}

void Mx2dCustomLine::fromJson(const QJsonObject& jsonObject)
{
	assertWriteEnabled();
	Mx2dCustomAnnotation::fromJson(jsonObject);
	m_startPt = Mx2d::jsonArray2dToPoint3d(jsonObject["startPoint"].toArray());
	m_endPt = Mx2d::jsonArray2dToPoint3d(jsonObject["endPoint"].toArray());
}

QJsonObject Mx2dCustomLine::toJson() const
{
	assertReadEnabled();
	QJsonObject jsonObject = Mx2dCustomAnnotation::toJson();

	jsonObject["startPoint"] = Mx2d::point3dToJsonArray2d(m_startPt);
	jsonObject["endPoint"] = Mx2d::point3dToJsonArray2d(m_endPt);

	return jsonObject;
}

void Mx2dCustomLine::setStartPt(const McGePoint3d& startPt)
{
	assertWriteEnabled();
	m_startPt = startPt;
}

McGePoint3d Mx2dCustomLine::startPt() const
{
	assertReadEnabled();
	return m_startPt;
}

void Mx2dCustomLine::setEndPt(const McGePoint3d& endPt)
{
	assertWriteEnabled();
	m_endPt = endPt;
}

McGePoint3d Mx2dCustomLine::endPt() const
{
	assertReadEnabled();
	return m_endPt;
}

McDbLine* Mx2dCustomLine::createLine(const McGePoint3d& startPt, const McGePoint3d& endPt) const
{
	return new McDbLine(startPt, endPt);
}
