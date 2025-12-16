/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dCustomRectCloud.h"
#include "Mx2dUtils.h"
#include "Mx2dAnnotationFactory.h"

REGISTER_ANNOTATION("rectCloud", Mx2dCustomRectCloud);

ACRX_DXF_DEFINE_MEMBERS(Mx2dCustomRectCloud, Mx2dCustomAnnotation,
	McDb::kDHL_CURRENT, McDb::kMReleaseCurrent,
	McDbProxyEntity::kAllAllowedBits, RectCloudMark,
	RectCloud Mark Entity);

Mx2dCustomRectCloud::Mx2dCustomRectCloud(void)
{
	setType("rectCloud");
}

Mx2dCustomRectCloud::Mx2dCustomRectCloud(const McGePoint3d& pt1, const McGePoint3d& pt2, double textHeight)
	: Mx2dCustomAnnotation(textHeight), m_corner1(pt1), m_corner2(pt2)
{
	setType("rectCloud");
}

Mx2dCustomRectCloud::~Mx2dCustomRectCloud(void)
{
}

Mdesk::Boolean Mx2dCustomRectCloud::worldDraw(McGiWorldDraw* wd)
{

	McDbPolyline* pPoly = createRectCloud();
	pPoly->worldDraw(wd);
	delete pPoly;

	return Mdesk::kTrue;
}

Mcad::ErrorStatus Mx2dCustomRectCloud::getGripPoints(McGePoint3dArray& gripPoints, McGeIntArray& osnapModes, McGeIntArray& geomIds) const
{
	assertReadEnabled();
	gripPoints.append(m_corner1);
	gripPoints.append(m_corner2);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomRectCloud::moveGripPointsAt(const McGeIntArray& indices, const McGeVector3d& offset)
{
	assertWriteEnabled();
	int iIndex = indices[0];
	switch (iIndex)
	{
	case 0:
		m_corner1 = m_corner1 + offset;
		break;
	case 1:
		m_corner2 = m_corner2 + offset;
		break;
	}
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomRectCloud::getGeomExtents(McDbExtents& extents, bool roughCalculation) const
{
	assertReadEnabled();

	McDbExtents ext;
	McDbPolyline* pPoly = createRectCloud();
	pPoly->getGeomExtents(ext, roughCalculation);
	extents.addExt(ext);
	delete pPoly;

	return Mcad::eOk;
}

#define RECTCLOUD_VERSION 1


Mcad::ErrorStatus Mx2dCustomRectCloud::dwgInFields(McDbDwgFiler* pFiler)
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
	pFiler->readPoint3d(&m_corner1);
	pFiler->readPoint3d(&m_corner2);

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomRectCloud::dwgOutFields(McDbDwgFiler* pFiler) const
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
	pFiler->writeInt(RECTCLOUD_VERSION);
	pFiler->writePoint3d(m_corner1);
	pFiler->writePoint3d(m_corner2);

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomRectCloud::explode(McDbVoidPtrArray& entitySet) const
{
	assertReadEnabled();
	McDbPolyline* pPoly = createRectCloud();
	entitySet.append(pPoly);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomRectCloud::transformBy(const McGeMatrix3d& xform)
{
	assertWriteEnabled();
	m_corner1.transformBy(xform);
	m_corner2.transformBy(xform);
	return Mcad::eOk;
}

void Mx2dCustomRectCloud::fromJson(const QJsonObject& jsonObject)
{
	assertWriteEnabled();
	Mx2dCustomAnnotation::fromJson(jsonObject);
	m_corner1 = Mx2d::jsonArray2dToPoint3d(jsonObject["corner1"].toArray());
	m_corner2 = Mx2d::jsonArray2dToPoint3d(jsonObject["corner2"].toArray());
}

QJsonObject Mx2dCustomRectCloud::toJson() const
{
	assertReadEnabled();
	QJsonObject jsonObject = Mx2dCustomAnnotation::toJson();
	jsonObject["corner1"] = Mx2d::point3dToJsonArray2d(m_corner1);
	jsonObject["corner2"] = Mx2d::point3dToJsonArray2d(m_corner2);

	return jsonObject;
}

void Mx2dCustomRectCloud::setCorner1(const McGePoint3d& pt)
{
	assertWriteEnabled();
	m_corner1 = pt;
}

McGePoint3d Mx2dCustomRectCloud::corner1() const
{
	assertReadEnabled();
	return m_corner1;
}

void Mx2dCustomRectCloud::setCorner2(const McGePoint3d& pt)
{
	assertWriteEnabled();
	m_corner2 = pt;
}

McGePoint3d Mx2dCustomRectCloud::corner2() const
{
	assertReadEnabled();
	return m_corner2;
}

McDbPolyline* Mx2dCustomRectCloud::createRectCloud() const
{
	double minX = min(m_corner1.x, m_corner2.x);
	double maxX = max(m_corner1.x, m_corner2.x);
	double minY = min(m_corner1.y, m_corner2.y);
	double maxY = max(m_corner1.y, m_corner2.y);

	McGePoint3d lt(minX, maxY, 0);
	McGePoint3d rt(maxX, maxY, 0);
	McGePoint3d rb(maxX, minY, 0);
	McGePoint3d lb(minX, minY, 0);

	McDbPolyline* pPolyline = new McDbPolyline();


	double width = maxX - minX;
	double segmentLength = textHeight() / 1.5;
	int segmentCountX = max(1, static_cast<int>(round(width / segmentLength)));
	double actualSegmentLengthX = width / segmentCountX;

	double height = maxY - minY;
	int segmentCountY = max(1, static_cast<int>(round(height / segmentLength)));
	double actualSegmentLengthY = height / segmentCountY;

	pPolyline->addVertexAt(lt);

	for (int i = 1; i < segmentCountX; i++)
	{
		McGePoint3d pt(lt.x + i * actualSegmentLengthX, lt.y, 0);
		pPolyline->addVertexAt(pt);
	}

	pPolyline->addVertexAt(rt);

	for (int i = 1; i < segmentCountY; i++)
	{
		McGePoint3d pt(rt.x, rt.y - i * actualSegmentLengthY, 0);
		pPolyline->addVertexAt(pt);
	}

	pPolyline->addVertexAt(rb);
	for (int i = 1; i < segmentCountX; i++)
	{
		McGePoint3d pt(rb.x - i * actualSegmentLengthX, rb.y, 0);
		pPolyline->addVertexAt(pt);
	}

	pPolyline->addVertexAt(lb);

	for (int i = 1; i < segmentCountY; i++)
	{
		McGePoint3d pt(lb.x, lb.y + i * actualSegmentLengthY, 0);
		pPolyline->addVertexAt(pt);
	}
	pPolyline->setClosed(true);
	for (unsigned int i = 0; i < pPolyline->numVerts(); i++)
	{
		pPolyline->setBulgeAt(i, -1.0);
	}
	return pPolyline;
}


