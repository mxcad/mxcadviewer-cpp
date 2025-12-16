/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dCustomEllipse.h"
#include "Mx2dUtils.h"
#include "Mx2dAnnotationFactory.h"

REGISTER_ANNOTATION("ellipse", Mx2dCustomEllipse);

ACRX_DXF_DEFINE_MEMBERS(Mx2dCustomEllipse, Mx2dCustomAnnotation,
	McDb::kDHL_CURRENT, McDb::kMReleaseCurrent,
	McDbProxyEntity::kAllAllowedBits, EllipseMark,
	Ellipse Mark Entity);

Mx2dCustomEllipse::Mx2dCustomEllipse(void)
{
	setType("ellipse");
}

Mx2dCustomEllipse::Mx2dCustomEllipse(const McGePoint3d& pt1, const McGePoint3d& pt2, double textHeight)
	:Mx2dCustomAnnotation(textHeight), m_corner1(pt1), m_corner2(pt2)
{
	setType("ellipse");
}

Mx2dCustomEllipse::~Mx2dCustomEllipse(void)
{
}

Mdesk::Boolean Mx2dCustomEllipse::worldDraw(McGiWorldDraw* wd)
{

	McDbEllipse* pEllipse = createEllipse();
	if (pEllipse)
	{
		pEllipse->worldDraw(wd);
		delete pEllipse;
	}

	return Mdesk::kTrue;
}

Mcad::ErrorStatus Mx2dCustomEllipse::getGripPoints(McGePoint3dArray& gripPoints, McGeIntArray& osnapModes, McGeIntArray& geomIds) const
{
	assertReadEnabled();
	gripPoints.append(m_corner1);
	gripPoints.append(m_corner2);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomEllipse::moveGripPointsAt(const McGeIntArray& indices, const McGeVector3d& offset)
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

Mcad::ErrorStatus Mx2dCustomEllipse::getGeomExtents(McDbExtents& extents, bool roughCalculation) const
{
	assertReadEnabled();

	extents.addPoint(m_corner1);
	extents.addPoint(m_corner2);

	return Mcad::eOk;
}

#define ELLIPSE_VERSION 1


Mcad::ErrorStatus Mx2dCustomEllipse::dwgInFields(McDbDwgFiler* pFiler)
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

Mcad::ErrorStatus Mx2dCustomEllipse::dwgOutFields(McDbDwgFiler* pFiler) const
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
	pFiler->writeInt(ELLIPSE_VERSION);
	pFiler->writePoint3d(m_corner1);
	pFiler->writePoint3d(m_corner2);

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomEllipse::explode(McDbVoidPtrArray& entitySet) const
{
	assertReadEnabled();
	McDbEllipse* pEllipse = createEllipse();
	if (pEllipse)
	{
		entitySet.append(pEllipse);
	}
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomEllipse::transformBy(const McGeMatrix3d& xform)
{
	assertWriteEnabled();
	m_corner1.transformBy(xform);
	m_corner2.transformBy(xform);
	return Mcad::eOk;
}

void Mx2dCustomEllipse::fromJson(const QJsonObject& jsonObject)
{
	assertWriteEnabled();
	Mx2dCustomAnnotation::fromJson(jsonObject);
	m_corner1 = Mx2d::jsonArray2dToPoint3d(jsonObject["corner1"].toArray());
	m_corner2 = Mx2d::jsonArray2dToPoint3d(jsonObject["corner2"].toArray());
}

QJsonObject Mx2dCustomEllipse::toJson() const
{
	assertReadEnabled();
	QJsonObject jsonObject = Mx2dCustomAnnotation::toJson();

	jsonObject["corner1"] = Mx2d::point3dToJsonArray2d(m_corner1);
	jsonObject["corner2"] = Mx2d::point3dToJsonArray2d(m_corner2);

	return jsonObject;
}

void Mx2dCustomEllipse::setCorner1(const McGePoint3d& pt)
{
	assertWriteEnabled();
	m_corner1 = pt;
}

McGePoint3d Mx2dCustomEllipse::corner1() const
{
	assertReadEnabled();
	return m_corner1;
}

void Mx2dCustomEllipse::setCorner2(const McGePoint3d& pt)
{
	assertWriteEnabled();
	m_corner2 = pt;
}

McGePoint3d Mx2dCustomEllipse::corner2() const
{
	assertReadEnabled();
	return m_corner2;
}

McDbEllipse* Mx2dCustomEllipse::createEllipse() const
{
	McGePoint3d center((m_corner1.x + m_corner2.x) / 2, (m_corner1.y + m_corner2.y) / 2, 0);
	double majorAxis = fabs(m_corner1.x - m_corner2.x) / 2;
	double minorAxis = fabs(m_corner1.y - m_corner2.y) / 2;

	if (majorAxis == 0. || minorAxis == 0.)
	{
		return nullptr;
	}

	return new McDbEllipse(center, McGeVector3d::kZAxis, McGeVector3d(majorAxis, 0, 0), minorAxis / majorAxis, 0, 2 * M_PI);
}


