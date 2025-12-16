/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dCustomArcLengthDim.h"
#include <QString>
#include <QJsonArray>
#include "Mx2dUtils.h"
#include "Mx2dAnnotationFactory.h"

REGISTER_ANNOTATION("arcLengthDim", Mx2dCustomArcLengthDim);
ACRX_DXF_DEFINE_MEMBERS(Mx2dCustomArcLengthDim, Mx2dCustomAnnotation,
	McDb::kDHL_CURRENT, McDb::kMReleaseCurrent,
	McDbProxyEntity::kAllAllowedBits, ArcLengthDimMark,
	ArcLengthDim Mark Entity);

Mx2dCustomArcLengthDim::Mx2dCustomArcLengthDim(void)
{
	setType("arcLengthDim");
}

Mx2dCustomArcLengthDim::Mx2dCustomArcLengthDim(const McGePoint3d& startPt, const McGePoint3d& midPt, const McGePoint3d& endPt, const McGePoint3d& dimPt, double textHeight)
	:Mx2dCustomAnnotation(textHeight), m_startPt(startPt), m_midPt(midPt), m_endPt(endPt), m_dimPt(dimPt)
{
	setType("arcLengthDim");
}


Mx2dCustomArcLengthDim::~Mx2dCustomArcLengthDim(void)
{
}

Mdesk::Boolean Mx2dCustomArcLengthDim::worldDraw(McGiWorldDraw* wd)
{
	McDbArc* pArc = createArc();
	pArc->worldDraw(wd);
	delete pArc;

	McDbText* pText = createText();
	pText->worldDraw(wd);
	delete pText;

	McDbLine* pExtLine1 = createExtLine1();
	pExtLine1->worldDraw(wd);
	delete pExtLine1;
	McDbLine* pExtLine2 = createExtLine2();
	pExtLine2->worldDraw(wd);
	delete pExtLine2;

	McDbLine* pObliqueLine1 = createObliqueLine1();
	pObliqueLine1->worldDraw(wd);
	delete pObliqueLine1;
	McDbLine* pObliqueLine2 = createObliqueLine2();
	pObliqueLine2->worldDraw(wd);
	delete pObliqueLine2;

	return Mdesk::kTrue;
}

Mcad::ErrorStatus Mx2dCustomArcLengthDim::getGripPoints(McGePoint3dArray& gripPoints, McGeIntArray& osnapModes, McGeIntArray& geomIds) const
{
	assertReadEnabled();
	gripPoints.append(m_dimPt);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomArcLengthDim::moveGripPointsAt(const McGeIntArray& indices, const McGeVector3d& offset)
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

Mcad::ErrorStatus Mx2dCustomArcLengthDim::getGeomExtents(McDbExtents& extents, bool roughCalculation) const
{
	assertReadEnabled();
	McDbExtents ext;
	McDbArc* pArc = createArc();
	pArc->getGeomExtents(ext, roughCalculation);
	extents.addExt(ext);
	delete pArc;

	McDbText* pText = createText();
	pText->getGeomExtents(ext, roughCalculation);
	extents.addExt(ext);
	delete pText;

	McDbLine* pExtLine1 = createExtLine1();
	pExtLine1->getGeomExtents(ext, roughCalculation);
	extents.addExt(ext);
	delete pExtLine1;

	McDbLine* pExtLine2 = createExtLine2();
	pExtLine2->getGeomExtents(ext, roughCalculation);
	extents.addExt(ext);
	delete pExtLine2;

	McDbLine* pObliqueLine1 = createObliqueLine1();
	pObliqueLine1->getGeomExtents(ext, roughCalculation);
	extents.addExt(ext);
	delete pObliqueLine1;

	McDbLine* pObliqueLine2 = createObliqueLine2();
	pObliqueLine2->getGeomExtents(ext, roughCalculation);
	extents.addExt(ext);
	delete pObliqueLine2;

	return Mcad::eOk;
}

#define ARCLENGTHDIM_VERSION 1

Mcad::ErrorStatus Mx2dCustomArcLengthDim::dwgInFields(McDbDwgFiler* pFiler)
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
	pFiler->readPoint3d(&m_dimPt);

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomArcLengthDim::dwgOutFields(McDbDwgFiler* pFiler) const
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
	pFiler->writeInt(ARCLENGTHDIM_VERSION);
	pFiler->writePoint3d(m_startPt);
	pFiler->writePoint3d(m_midPt);
	pFiler->writePoint3d(m_endPt);
	pFiler->writePoint3d(m_dimPt);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomArcLengthDim::explode(McDbVoidPtrArray& entitySet) const
{
	McDbArc* pArc = createArc();
	McDbText* pText = createText();
	McDbLine* pExtLine1 = createExtLine1();
	McDbLine* pExtLine2 = createExtLine2();
	McDbLine* pObliqueLine1 = createObliqueLine1();
	McDbLine* pObliqueLine2 = createObliqueLine2();
	entitySet.append(pArc);
	entitySet.append(pText);
	entitySet.append(pExtLine1);
	entitySet.append(pExtLine2);
	entitySet.append(pObliqueLine1);
	entitySet.append(pObliqueLine2);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomArcLengthDim::transformBy(const McGeMatrix3d& xform)
{
	assertWriteEnabled();
	m_startPt.transformBy(xform);
	m_midPt.transformBy(xform);
	m_endPt.transformBy(xform);
	m_dimPt.transformBy(xform);
	return Mcad::eOk;
}

void Mx2dCustomArcLengthDim::fromJson(const QJsonObject& jsonObject)
{
	assertWriteEnabled();
	Mx2dCustomAnnotation::fromJson(jsonObject);
	m_startPt = Mx2d::jsonArray2dToPoint3d(jsonObject["startPoint"].toArray());
	m_midPt = Mx2d::jsonArray2dToPoint3d(jsonObject["midPoint"].toArray());
	m_endPt = Mx2d::jsonArray2dToPoint3d(jsonObject["endPoint"].toArray());
	m_dimPt = Mx2d::jsonArray2dToPoint3d(jsonObject["dimPoint"].toArray());

}

QJsonObject Mx2dCustomArcLengthDim::toJson() const
{
	assertReadEnabled();
	QJsonObject jsonObject = Mx2dCustomAnnotation::toJson();

	jsonObject["startPoint"] = Mx2d::point3dToJsonArray2d(m_startPt);
	jsonObject["midPoint"] = Mx2d::point3dToJsonArray2d(m_midPt);
	jsonObject["endPoint"] = Mx2d::point3dToJsonArray2d(m_endPt);
	jsonObject["dimPoint"] = Mx2d::point3dToJsonArray2d(m_dimPt);

	return jsonObject;
}

void Mx2dCustomArcLengthDim::setStartPt(const McGePoint3d& pt)
{
	assertWriteEnabled();
	m_startPt = pt;
}

McGePoint3d Mx2dCustomArcLengthDim::startPt() const
{
	assertReadEnabled();
	return m_startPt;
}

void Mx2dCustomArcLengthDim::setMidPt(const McGePoint3d& pt)
{
	assertWriteEnabled();
	m_midPt = pt;
}

McGePoint3d Mx2dCustomArcLengthDim::midPt() const
{
	assertReadEnabled();
	return m_midPt;
}

void Mx2dCustomArcLengthDim::setEndPt(const McGePoint3d& pt)
{
	assertWriteEnabled();
	m_endPt = pt;
}

McGePoint3d Mx2dCustomArcLengthDim::endPt() const
{
	assertReadEnabled();
	return m_endPt;
}

void Mx2dCustomArcLengthDim::setDimPt(const McGePoint3d& pt)
{
	assertWriteEnabled();
	m_dimPt = pt;
}

McGePoint3d Mx2dCustomArcLengthDim::dimPt() const
{
	assertReadEnabled();
	return m_dimPt;
}



McDbArc* Mx2dCustomArcLengthDim::createArc() const
{
	McGePoint3d center;
	double radius = 0.0, length = 0.0;
	calculateArc(center, radius, length);
	McGeVector3d dirStart(m_startPt - center);
	McGeVector3d dirMid(m_midPt - center);
	McGeVector3d dirEnd(m_endPt - center);
	dirStart.normalize();
	dirMid.normalize();
	dirEnd.normalize();
	double offset = m_dimPt.distanceTo(center) - radius;
	McGeVector3d offsetVecStart(dirStart * offset);
	McGeVector3d offsetVecMid(dirMid * offset);
	McGeVector3d offsetVecEnd(dirEnd * offset);
	McGePoint3d ptStart = m_startPt + offsetVecStart;
	McGePoint3d ptMid = m_midPt + offsetVecMid;
	McGePoint3d ptEnd = m_endPt + offsetVecEnd;
	McDbArc* pArc = new McDbArc();
	pArc->computeArc(ptStart, ptMid, ptEnd);
	return pArc;
}

McDbText* Mx2dCustomArcLengthDim::createText() const
{
	McGePoint3d center;
	double radius = 0.0, length = 0.0;
	calculateArc(center, radius, length);
	McDbObjectId textStyle = Mx::mcdbCurDwg()->textstyle();
	McDbText* pText = new McDbText();
	pText->setHeight(textHeight());
	pText->setTextStyle(textStyle);
	QString num = QString::number(length * dimRatio(), 'f', 2);
	QString str = QString(u8"⌒%1").arg(num);
	pText->setTextString(str.toLocal8Bit().constData());
	McDbExtents extents;
	pText->getGeomExtents(extents, false);
	McGePoint3d minPt = extents.minPoint();
	McGePoint3d maxPt = extents.maxPoint();
	McGePoint3d mbAnchorPt((minPt.x + maxPt.x) / 2.0, minPt.y - textHeight() / 4.0, 0);
	McGeVector3d vec = m_midPt - center;
	McGeVector3d dirMid = vec;
	dirMid.normalize();
	double offset = m_dimPt.distanceTo(center) - radius;
	McGeVector3d offsetVecMid(dirMid * offset);
	McGePoint3d ptMid = m_midPt + offsetVecMid;
	if (vec.x >= 0 && vec.y >= 0 || vec.x <= 0 && vec.y <= 0)
	{
		// 1,3
		McGeVector3d rotVec(fabs(vec.x), fabs(vec.y), 0);
		double ang = rotVec.angleTo(McGeVector3d::kXAxis);
		pText->transformBy(McGeMatrix3d::rotation(-(M_PI / 2.0 - ang), McGeVector3d::kZAxis, mbAnchorPt));
	}
	else
	{
		// 2,4
		McGeVector3d rotVec(fabs(vec.x), -fabs(vec.y), 0);
		double ang = rotVec.angleTo(McGeVector3d::kXAxis);
		pText->transformBy(McGeMatrix3d::rotation(M_PI / 2.0 - ang, McGeVector3d::kZAxis, mbAnchorPt));
	}
	pText->transformBy(McGeMatrix3d::translation(ptMid - mbAnchorPt));
	return pText;

}

McDbLine* Mx2dCustomArcLengthDim::createExtLine1() const
{
	McGePoint3d center;
	double radius = 0.0, length = 0.0;
	calculateArc(center, radius, length);

	double extLen = textHeight() / 2.0;
	McGeVector3d dir(m_startPt - center);
	dir.normalize();
	double offset = m_dimPt.distanceTo(center) - radius;
	McGeVector3d offsetVec(dir * offset);
	McGePoint3d pt = m_startPt + offsetVec + offsetVec.normal() * extLen;
	return new McDbLine(m_startPt, pt);
}

McDbLine* Mx2dCustomArcLengthDim::createExtLine2() const
{
	McGePoint3d center;
	double radius = 0.0, length = 0.0;
	calculateArc(center, radius, length);

	double extLen = textHeight() / 2.0;
	McGeVector3d dir(m_endPt - center);
	dir.normalize();
	double offset = m_dimPt.distanceTo(center) - radius;
	McGeVector3d offsetVec(dir * offset);
	McGePoint3d pt = m_endPt + offsetVec + offsetVec.normal() * extLen;
	return new McDbLine(m_endPt, pt);
}

McDbLine* Mx2dCustomArcLengthDim::createObliqueLine1() const
{
	McGePoint3d center;
	double radius = 0.0, length = 0.0;
	calculateArc(center, radius, length);

	double extLen = textHeight() / 3.0;
	McGeVector3d dir(m_startPt - center);
	dir.normalize();
	double offset = m_dimPt.distanceTo(center) - radius;
	McGeVector3d offsetVec(dir * offset);
	McGePoint3d pt = m_startPt + offsetVec;
	McGeVector3d oDir = dir;
	oDir.rotateBy(-M_PI / 4, McGeVector3d::kZAxis);
	McGePoint3d pt1 = pt + oDir * extLen;
	McGePoint3d pt2 = pt - oDir * extLen;
	return new McDbLine(pt1, pt2);
}

McDbLine* Mx2dCustomArcLengthDim::createObliqueLine2() const
{
	McGePoint3d center;
	double radius = 0.0, length = 0.0;
	calculateArc(center, radius, length);

	double extLen = textHeight() / 3.0;
	McGeVector3d dir(m_endPt - center);
	dir.normalize();
	double offset = m_dimPt.distanceTo(center) - radius;
	McGeVector3d offsetVec(dir * offset);
	McGePoint3d pt = m_endPt + offsetVec;
	McGeVector3d oDir = dir;
	oDir.rotateBy(-M_PI / 4, McGeVector3d::kZAxis);
	McGePoint3d pt1 = pt + oDir * extLen;
	McGePoint3d pt2 = pt - oDir * extLen;
	return new McDbLine(pt1, pt2);
}

void Mx2dCustomArcLengthDim::calculateArc(McGePoint3d& center, double& radius, double& length) const
{
	McGeCircArc2d arc({ m_startPt.x, m_startPt.y }, { m_midPt.x, m_midPt.y }, { m_endPt.x, m_endPt.y });
	center = arc.center();
	radius = arc.radius();
	length = arc.distanceOfParam(arc.paramOf(arc.endPoint()));
}


