/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dCustomRect.h"
#include "Mx2dUtils.h"
#include "Mx2dAnnotationFactory.h"

REGISTER_ANNOTATION("rect", Mx2dCustomRect);

ACRX_DXF_DEFINE_MEMBERS(Mx2dCustomRect, Mx2dCustomAnnotation,
	McDb::kDHL_CURRENT, McDb::kMReleaseCurrent,
	McDbProxyEntity::kAllAllowedBits, RectMark,
	Rect Mark Entity);

Mx2dCustomRect::Mx2dCustomRect(void)
{
	setType("rect");
}

Mx2dCustomRect::Mx2dCustomRect(const McGePoint3d& pt1, const McGePoint3d& pt2, double textHeight)
	:Mx2dCustomAnnotation(textHeight), m_corner1(pt1), m_corner2(pt2)
{
	setType("rect");
}

Mx2dCustomRect::~Mx2dCustomRect(void)
{
}

Mdesk::Boolean Mx2dCustomRect::worldDraw(McGiWorldDraw* wd)
{

	McDbPolyline* pPoly = createRect();
	pPoly->worldDraw(wd);
	delete pPoly;

	return Mdesk::kTrue;
}

Mcad::ErrorStatus Mx2dCustomRect::getGripPoints(McGePoint3dArray& gripPoints, McGeIntArray& osnapModes, McGeIntArray& geomIds) const
{
	assertReadEnabled();
	gripPoints.append(m_corner1);
	gripPoints.append(m_corner2);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomRect::moveGripPointsAt(const McGeIntArray& indices, const McGeVector3d& offset)
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

Mcad::ErrorStatus Mx2dCustomRect::getGeomExtents(McDbExtents& extents, bool roughCalculation) const
{
	assertReadEnabled();

	extents.addPoint(m_corner1);
	extents.addPoint(m_corner2);

	return Mcad::eOk;
}

#define RECT_VERSION 1


Mcad::ErrorStatus Mx2dCustomRect::dwgInFields(McDbDwgFiler* pFiler)
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

Mcad::ErrorStatus Mx2dCustomRect::dwgOutFields(McDbDwgFiler* pFiler) const
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
	pFiler->writeInt(RECT_VERSION);
	pFiler->writePoint3d(m_corner1);
	pFiler->writePoint3d(m_corner2);

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomRect::explode(McDbVoidPtrArray& entitySet) const
{
	assertReadEnabled();
	McDbPolyline* pPoly = createRect();
	entitySet.append(pPoly);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomRect::transformBy(const McGeMatrix3d& xform)
{
	assertWriteEnabled();
	m_corner1.transformBy(xform);
	m_corner2.transformBy(xform);
	return Mcad::eOk;
}

void Mx2dCustomRect::fromJson(const QJsonObject& jsonObject)
{
	assertWriteEnabled();
	Mx2dCustomAnnotation::fromJson(jsonObject);
	m_corner1 = Mx2d::jsonArray2dToPoint3d(jsonObject["corner1"].toArray());
	m_corner2 = Mx2d::jsonArray2dToPoint3d(jsonObject["corner2"].toArray());
}

QJsonObject Mx2dCustomRect::toJson() const
{
	assertReadEnabled();
	QJsonObject jsonObject = Mx2dCustomAnnotation::toJson();
	jsonObject["corner1"] = Mx2d::point3dToJsonArray2d(m_corner1);
	jsonObject["corner2"] = Mx2d::point3dToJsonArray2d(m_corner2);

	return jsonObject;
}

McGePoint3dArray Mx2dCustomRect::getRectPoints() const
{
	McGePoint3dArray ret;
	double minX = min(m_corner1.x, m_corner2.x);
	double maxX = max(m_corner1.x, m_corner2.x);
	double minY = min(m_corner1.y, m_corner2.y);
	double maxY = max(m_corner1.y, m_corner2.y);

	McGePoint3d lt(minX, maxY, 0);
	McGePoint3d rt(maxX, maxY, 0);
	McGePoint3d rb(maxX, minY, 0);
	McGePoint3d lb(minX, minY, 0);
	ret.append(lt);
	ret.append(rt);
	ret.append(rb);
	ret.append(lb);

	return ret;
}

void Mx2dCustomRect::setCorner1(const McGePoint3d& pt)
{
	assertWriteEnabled();
	m_corner1 = pt;
}

McGePoint3d Mx2dCustomRect::corner1() const
{
	assertReadEnabled();
	return m_corner1;
}

void Mx2dCustomRect::setCorner2(const McGePoint3d& pt)
{
	assertWriteEnabled();
	m_corner2 = pt;
}

McGePoint3d Mx2dCustomRect::corner2() const
{
	assertReadEnabled();
	return m_corner2;
}

McDbPolyline* Mx2dCustomRect::createRect() const
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


	pPolyline->addVertexAt(lt);
	pPolyline->addVertexAt(rt);
	pPolyline->addVertexAt(rb);
	pPolyline->addVertexAt(lb);
	pPolyline->setClosed(Mdesk::kTrue);

	return pPolyline;
}

double Mx2dCustomRect::getArea() const
{
	double width = fabs(m_corner2.x - m_corner1.x);
	double height = fabs(m_corner1.y - m_corner2.y);
	return width * height;
}

double Mx2dCustomRect::getPerimeter() const
{
	double width = fabs(m_corner2.x - m_corner1.x);
	double height = fabs(m_corner1.y - m_corner2.y);
	return 2 * (width + height);
}


