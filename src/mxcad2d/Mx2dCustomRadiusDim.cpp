/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dCustomRadiusDim.h"
#include <QString>
#include "Mx2dUtils.h"
#include "Mx2dAnnotationFactory.h"

REGISTER_ANNOTATION("radiusDim", Mx2dCustomRadiusDim);
ACRX_DXF_DEFINE_MEMBERS(Mx2dCustomRadiusDim, Mx2dCustomAnnotation,
	McDb::kDHL_CURRENT, McDb::kMReleaseCurrent,
	McDbProxyEntity::kAllAllowedBits, RadiusDimMark,
	RadiusDim Mark Entity);

Mx2dCustomRadiusDim::Mx2dCustomRadiusDim(void)
{
	setType("radiusDim");
}

Mx2dCustomRadiusDim::Mx2dCustomRadiusDim(const McGePoint3d& startPt, const McGePoint3d& midPt, const McGePoint3d& endPt, const McGePoint3d& dimPt, double textHeight)
	:Mx2dCustomAnnotation(textHeight), m_startPt(startPt), m_midPt(midPt), m_endPt(endPt), m_dimPt(dimPt), m_isArc(true)
{
	setType("radiusDim");
	calculateArc(m_centerPt, m_radius);
}

Mx2dCustomRadiusDim::Mx2dCustomRadiusDim(const McGePoint3d& centerPt, double radius, const McGePoint3d& dimPt, double textHeight)
	:Mx2dCustomAnnotation(textHeight), m_centerPt(centerPt), m_radius(radius), m_dimPt(dimPt), m_isArc(false)
{
	setType("radiusDim");
}

Mx2dCustomRadiusDim::~Mx2dCustomRadiusDim(void)
{
}

Mdesk::Boolean Mx2dCustomRadiusDim::worldDraw(McGiWorldDraw* wd)
{
	McGePoint3d arrowPoint;
	McDbPolyline* pArrow = createArrow(arrowPoint);
	pArrow->worldDraw(wd);
	delete pArrow;
	McDbLine* pLine = createLine(arrowPoint);
	pLine->worldDraw(wd);
	delete pLine;

	if (m_isArc)
	{
		McDbArc* pArc = createArc(arrowPoint);
		if (pArc)
		{
			pArc->worldDraw(wd);
			delete pArc;
		}
	}

	McDbText* pText = createText();
	pText->worldDraw(wd);
	delete pText;

	return Mdesk::kTrue;
}

Mcad::ErrorStatus Mx2dCustomRadiusDim::getGripPoints(McGePoint3dArray& gripPoints, McGeIntArray& osnapModes, McGeIntArray& geomIds) const
{
	assertReadEnabled();
	gripPoints.append(m_dimPt);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomRadiusDim::moveGripPointsAt(const McGeIntArray& indices, const McGeVector3d& offset)
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

Mcad::ErrorStatus Mx2dCustomRadiusDim::getGeomExtents(McDbExtents& extents, bool roughCalculation) const
{
	assertReadEnabled();

	McDbVoidPtrArray entitySet;
	explode(entitySet);

	for (int i = 0; i < entitySet.length(); i++)
	{
		McDbEntity* pEntity = (McDbEntity*)entitySet[i];
		if (pEntity)
		{
			McDbExtents entityExtents;
			if (pEntity->getGeomExtents(entityExtents, roughCalculation) == Mcad::eOk)
			{
				extents.addExt(entityExtents);
			}
			delete pEntity;
		}
	}
	entitySet.clear();
	return Mcad::eOk;
}

#define RADIUSDIM_VERSION 1

Mcad::ErrorStatus Mx2dCustomRadiusDim::dwgInFields(McDbDwgFiler* pFiler)
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
	pFiler->readPoint3d(&m_midPt);
	pFiler->readPoint3d(&m_endPt);
	pFiler->readPoint3d(&m_centerPt);
	pFiler->readPoint3d(&m_dimPt);
	pFiler->readDouble(&m_radius);
	pFiler->readBool(&m_isArc);

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomRadiusDim::dwgOutFields(McDbDwgFiler* pFiler) const
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
	pFiler->writeInt(RADIUSDIM_VERSION);
	pFiler->writePoint3d(m_startPt);
	pFiler->writePoint3d(m_midPt);
	pFiler->writePoint3d(m_endPt);
	pFiler->writePoint3d(m_centerPt);
	pFiler->writePoint3d(m_dimPt);
	pFiler->writeDouble(m_radius);
	pFiler->writeBool(m_isArc);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomRadiusDim::explode(McDbVoidPtrArray& entitySet) const
{
	McGePoint3d arrowPoint;
	McDbPolyline* pArrow = createArrow(arrowPoint);
	entitySet.append(pArrow);

	McDbLine* pLine = createLine(arrowPoint);
	entitySet.append(pLine);

	if (m_isArc)
	{
		McDbArc* pArc = createArc(arrowPoint);
		if (pArc)
		{
			entitySet.append(pArc);
		}
	}

	McDbText* pText = createText();
	entitySet.append(pText);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomRadiusDim::transformBy(const McGeMatrix3d& xform)
{
	assertWriteEnabled();
	m_startPt.transformBy(xform);
	m_midPt.transformBy(xform);
	m_endPt.transformBy(xform);
	m_centerPt.transformBy(xform);
	m_dimPt.transformBy(xform);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomRadiusDim::getOsnapPoints(McDb::OsnapMode osnapMode, int gsSelectionMark, const McGePoint3d& pickPoint, const McGePoint3d& lastPoint, const McGeMatrix3d& viewXform, McGePoint3dArray& snapPoints, McDbIntArray& geomIds) const
{
	assertReadEnabled();

	if (osnapMode == McDb::kOsModeEnd)
	{
		McGeVector3d dir(m_dimPt - m_centerPt);
		dir.normalize();
		McGePoint3d arrowPt = m_centerPt + dir * m_radius;
		snapPoints.append(m_dimPt);
		snapPoints.append(arrowPt);
	}
	else if (osnapMode == McDb::kOsModeIns)
	{

		snapPoints.append(m_centerPt);
	}
	return Mcad::eOk;
}

void Mx2dCustomRadiusDim::fromJson(const QJsonObject& jsonObject)
{
	assertWriteEnabled();
	Mx2dCustomAnnotation::fromJson(jsonObject);
	m_startPt = Mx2d::jsonArray2dToPoint3d(jsonObject["startPoint"].toArray());
	m_midPt = Mx2d::jsonArray2dToPoint3d(jsonObject["midPoint"].toArray());
	m_endPt = Mx2d::jsonArray2dToPoint3d(jsonObject["endPoint"].toArray());
	m_centerPt = Mx2d::jsonArray2dToPoint3d(jsonObject["centerPoint"].toArray());
	m_dimPt = Mx2d::jsonArray2dToPoint3d(jsonObject["dimPoint"].toArray());
	m_radius = jsonObject["radius"].toDouble();
	m_isArc = jsonObject["isArc"].toBool();
}

QJsonObject Mx2dCustomRadiusDim::toJson() const
{
	assertReadEnabled();
	QJsonObject jsonObject = Mx2dCustomAnnotation::toJson();
	jsonObject["startPoint"] = Mx2d::point3dToJsonArray2d(m_startPt);
	jsonObject["midPoint"] = Mx2d::point3dToJsonArray2d(m_midPt);
	jsonObject["endPoint"] = Mx2d::point3dToJsonArray2d(m_endPt);
	jsonObject["centerPoint"] = Mx2d::point3dToJsonArray2d(m_centerPt);
	jsonObject["dimPoint"] = Mx2d::point3dToJsonArray2d(m_dimPt);
	jsonObject["radius"] = m_radius;
	jsonObject["isArc"] = m_isArc;

	return jsonObject;
}

Mx2d::TextInfoList Mx2dCustomRadiusDim::findText(const QString& text, bool isExactMatch) const
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

void Mx2dCustomRadiusDim::setStartPt(const McGePoint3d& startPt)
{
	assertWriteEnabled();
	m_startPt = startPt;
}

McGePoint3d Mx2dCustomRadiusDim::startPt() const
{
	assertReadEnabled();
	return m_startPt;
}

void Mx2dCustomRadiusDim::setMidPt(const McGePoint3d& midPt)
{
	assertWriteEnabled();
	m_midPt = midPt;
}

McGePoint3d Mx2dCustomRadiusDim::midPt() const
{
	assertReadEnabled();
	return m_midPt;
}

void Mx2dCustomRadiusDim::setEndPt(const McGePoint3d& endPt)
{
	assertWriteEnabled();
	m_endPt = endPt;
}

McGePoint3d Mx2dCustomRadiusDim::endPt() const
{
	assertReadEnabled();
	return m_endPt;
}

void Mx2dCustomRadiusDim::setCenterPt(const McGePoint3d& centerPt)
{
	assertWriteEnabled();
	m_centerPt = centerPt;
}

McGePoint3d Mx2dCustomRadiusDim::centerPt() const
{
	assertReadEnabled();
	return m_centerPt;
}

void Mx2dCustomRadiusDim::setDimPt(const McGePoint3d& dimPt)
{
	assertWriteEnabled();
	m_dimPt = dimPt;
}

McGePoint3d Mx2dCustomRadiusDim::dimPt() const
{
	assertReadEnabled();
	return m_dimPt;
}

void Mx2dCustomRadiusDim::setRadius(double radius)
{
	assertWriteEnabled();
	m_radius = radius;
}

double Mx2dCustomRadiusDim::radius() const
{
	assertReadEnabled();
	return m_radius;
}

void Mx2dCustomRadiusDim::setIsArc(bool isArc)
{
	assertWriteEnabled();
	m_isArc = isArc;
}

bool Mx2dCustomRadiusDim::isArc() const
{
	assertReadEnabled();
	return m_isArc;
}


McDbPolyline* Mx2dCustomRadiusDim::createArrow(McGePoint3d& arrowPoint) const
{
	double arrowSize = textHeight() / 2.0;
	McGeVector3d dir(m_dimPt - m_centerPt);
	dir.normalize();
	McGePoint3d arrowPt = m_centerPt + dir * m_radius;
	arrowPoint = arrowPt;

	McGeVector3d dirVec;
	if (m_dimPt.distanceTo(m_centerPt) < m_radius)
	{
		dirVec = -dir * arrowSize;
	}
	else
	{
		dirVec = dir * arrowSize;
	}

	McGeVector3d vecRight = dirVec;
	vecRight.rotateBy(M_PI / 6, McGeVector3d::kZAxis);
	McGeVector3d vecLeft = dirVec;
	vecLeft.rotateBy(-M_PI / 6, McGeVector3d::kZAxis);

	McGePoint3d midPt = arrowPt;
	McGePoint3d rightPt = midPt + vecRight;
	McGePoint3d leftPt = midPt + vecLeft;

	McDbPolyline* pPoly = new McDbPolyline();
	pPoly->addVertexAt(rightPt);
	pPoly->addVertexAt(midPt);
	pPoly->addVertexAt(leftPt);

	return pPoly;

}

McDbLine* Mx2dCustomRadiusDim::createLine(const McGePoint3d& arrowPoint) const
{
	return new McDbLine(arrowPoint, m_dimPt);
}

McDbArc* Mx2dCustomRadiusDim::createArc(const McGePoint3d& arrowPoint) const
{
	bool isNearClosedStartPt = false;
	if (arrowPoint.distanceTo(m_startPt) < arrowPoint.distanceTo(m_endPt))
	{
		isNearClosedStartPt = true;
	}



	McGeVector3d startVec(m_startPt - m_centerPt);
	McGeVector3d midVec(m_midPt - m_centerPt);
	McGeVector3d endVec(m_endPt - m_centerPt);

	McGeVector3d crossVec = startVec.crossProduct(midVec);
	if (crossVec.z > 0.0)
	{
		if (isNearClosedStartPt)
		{
			McGeVector3d arrowVec(arrowPoint - m_centerPt);
			if (arrowVec.crossProduct(startVec).z > 0.0)
			{
				McGePoint3d pt1 = arrowPoint;
				McGePoint3d pt2 = pt1;
				McGePoint3d pt3 = m_startPt;
				double ang = arrowVec.angleTo(startVec);
				pt2.rotateBy(ang / 2.0, McGeVector3d::kZAxis, m_centerPt);
				McDbArc* pArc = new McDbArc();
				pArc->computeArc(pt1, pt2, pt3);
				return pArc;
			}
			else
			{
				return nullptr;
			}
		}
		else
		{
			McGeVector3d arrowVec(arrowPoint - m_centerPt);
			if (arrowVec.crossProduct(endVec).z < 0.0)
			{
				McGePoint3d pt1 = m_endPt;
				McGePoint3d pt2 = pt1;
				McGePoint3d pt3 = arrowPoint;
				double ang = endVec.angleTo(arrowVec);
				pt2.rotateBy(ang / 2.0, McGeVector3d::kZAxis, m_centerPt);
				McDbArc* pArc = new McDbArc();
				pArc->computeArc(pt1, pt2, pt3);
				return pArc;
			}
			else
			{
				return nullptr;
			}
		}

	}
	else // clockwise
	{
		if (isNearClosedStartPt)
		{
			McGeVector3d arrowVec(arrowPoint - m_centerPt);
			if (arrowVec.crossProduct(startVec).z < 0.0)
			{
				McGePoint3d pt1 = arrowPoint;
				McGePoint3d pt2 = pt1;
				McGePoint3d pt3 = m_startPt;
				double ang = arrowVec.angleTo(startVec);
				pt2.rotateBy(-ang / 2.0, McGeVector3d::kZAxis, m_centerPt);
				McDbArc* pArc = new McDbArc();
				pArc->computeArc(pt1, pt2, pt3);
				return pArc;
			}
			else
			{
				return nullptr;
			}
		}
		else
		{
			McGeVector3d arrowVec(arrowPoint - m_centerPt);
			if (arrowVec.crossProduct(endVec).z > 0.0)
			{
				McGePoint3d pt1 = m_endPt;
				McGePoint3d pt2 = pt1;
				McGePoint3d pt3 = arrowPoint;
				double ang = endVec.angleTo(arrowVec);
				pt2.rotateBy(-ang / 2.0, McGeVector3d::kZAxis, m_centerPt);
				McDbArc* pArc = new McDbArc();
				pArc->computeArc(pt1, pt2, pt3);
				return pArc;
			}
			else
			{
				return nullptr;
			}
		}
	}

	return nullptr;
}

McDbText* Mx2dCustomRadiusDim::createText() const
{
	McDbObjectId textStyle = Mx::mcdbCurDwg()->textstyle();
	McDbText* pText = new McDbText();
	pText->setHeight(textHeight());
	pText->setTextStyle(textStyle);
	QString num = QString::number(m_radius * dimRatio(), 'f', 3);
	QString str = QString("R%1").arg(num);
	pText->setTextString(str.toLocal8Bit().constData());
	McDbExtents extents;
	pText->getGeomExtents(extents, false);
	McGePoint3d minPt = extents.minPoint();
	McGePoint3d maxPt = extents.maxPoint();
	McGePoint3d lbAnchorPt(minPt.x, minPt.y - textHeight() / 4.0, 0);
	McGePoint3d rbAnchorPt(maxPt.x, minPt.y - textHeight() / 4.0, 0);
	McGeVector3d vec = m_dimPt - m_centerPt;

	if (vec.x >= 0 && vec.y >= 0 || vec.x <= 0 && vec.y <= 0)
	{
		// 1,3
		McGeVector3d rotVec(fabs(vec.x), fabs(vec.y), 0);
		double ang = rotVec.angleTo(McGeVector3d::kXAxis);
		pText->transformBy(McGeMatrix3d::rotation(ang, McGeVector3d::kZAxis, lbAnchorPt));
		rbAnchorPt.rotateBy(ang, McGeVector3d::kZAxis, lbAnchorPt);
		if (m_dimPt.distanceTo(m_centerPt) <= m_radius) // inner
		{
			if (vec.x >= 0 && vec.y >= 0) // 1
			{
				pText->transformBy(McGeMatrix3d::translation(m_dimPt - lbAnchorPt));
			}
			else // 3
			{
				pText->transformBy(McGeMatrix3d::translation(m_dimPt - rbAnchorPt));
			}
		}
		else
		{
			if (vec.x >= 0 && vec.y >= 0) // 1
			{
				pText->transformBy(McGeMatrix3d::translation(m_dimPt - rbAnchorPt));
			}
			else // 3
			{
				pText->transformBy(McGeMatrix3d::translation(m_dimPt - lbAnchorPt));
			}
		}
	}
	else
	{
		// 2,4
		McGeVector3d rotVec(fabs(vec.x), -fabs(vec.y), 0);
		double ang = rotVec.angleTo(McGeVector3d::kXAxis);
		pText->transformBy(McGeMatrix3d::rotation(-ang, McGeVector3d::kZAxis, lbAnchorPt));
		rbAnchorPt.rotateBy(-ang, McGeVector3d::kZAxis, lbAnchorPt);
		if (m_dimPt.distanceTo(m_centerPt) <= m_radius) // inner
		{
			if (vec.x <= 0 && vec.y >= 0) // 2
			{
				pText->transformBy(McGeMatrix3d::translation(m_dimPt - rbAnchorPt));
			}
			else // 4
			{
				pText->transformBy(McGeMatrix3d::translation(m_dimPt - lbAnchorPt));
			}
		}
		else
		{
			if (vec.x <= 0 && vec.y >= 0) // 2
			{
				pText->transformBy(McGeMatrix3d::translation(m_dimPt - lbAnchorPt));
			}
			else // 4
			{
				pText->transformBy(McGeMatrix3d::translation(m_dimPt - rbAnchorPt));
			}
		}
	}

	return pText;

}

void Mx2dCustomRadiusDim::calculateArc(McGePoint3d& center, double& radius) const
{
	McGeCircArc2d arc({ m_startPt.x, m_startPt.y }, { m_midPt.x, m_midPt.y }, { m_endPt.x, m_endPt.y });
	center = arc.center();
	radius = arc.radius();
}


