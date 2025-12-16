/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dCustomAngleMeasurement.h"
#include <QString>
#include "Mx2dUtils.h"
#include <QJsonArray>
#include "Mx2dAnnotationFactory.h"

REGISTER_ANNOTATION("angleMeasurement", Mx2dCustomAngleMeasurement);
ACRX_DXF_DEFINE_MEMBERS(Mx2dCustomAngleMeasurement, Mx2dCustomAnnotation,
	McDb::kDHL_CURRENT, McDb::kMReleaseCurrent,
	McDbProxyEntity::kAllAllowedBits, AngleMeasurementMark,
	AngleMeasurement Mark Entity);

Mx2dCustomAngleMeasurement::Mx2dCustomAngleMeasurement(void)
	:m_isDynamicDrawing(false)
{
	setType("angleMeasurement");
}

Mx2dCustomAngleMeasurement::Mx2dCustomAngleMeasurement(const McGePoint3d& line1Start, const McGePoint3d& line1End, const McGePoint3d& line2Start, const McGePoint3d& line2End, const McGePoint3d& dimPt, double textHeight, bool isDynamicDrawing)
	:Mx2dCustomAnnotation(textHeight), m_line1Start(line1Start), m_line1End(line1End), m_line2Start(line2Start), m_line2End(line2End), m_dimPt(dimPt), m_isDynamicDrawing(isDynamicDrawing)
{
	setType("angleMeasurement");
}

Mx2dCustomAngleMeasurement::~Mx2dCustomAngleMeasurement(void)
{
}

Mdesk::Boolean Mx2dCustomAngleMeasurement::worldDraw(McGiWorldDraw* wd)
{
	McDbVoidPtrArray arr = createAllEntities();

	McDbArc* pArc = static_cast<McDbArc*>(arr[0]);
	pArc->worldDraw(wd);
	delete pArc;

	McDbLine* pExtLine1 = static_cast<McDbLine*>(arr[1]);
	pExtLine1->worldDraw(wd);
	delete pExtLine1;

	McDbLine* pExtLine2 = static_cast<McDbLine*>(arr[2]);
	pExtLine2->worldDraw(wd);
	delete pExtLine2;

	McDbText* pText = static_cast<McDbText*>(arr[3]);
	pText->worldDraw(wd);
	delete pText;

	if (m_isDynamicDrawing)
	{
		McDbLine* pDynLine1 = static_cast<McDbLine*>(arr[4]);
		pDynLine1->worldDraw(wd);
		delete pDynLine1;
		McDbLine* pDynLine2 = static_cast<McDbLine*>(arr[5]);
		pDynLine2->worldDraw(wd);
		delete pDynLine2;
	}

	arr.clear();


	return Mdesk::kTrue;
}

Mcad::ErrorStatus Mx2dCustomAngleMeasurement::getGripPoints(McGePoint3dArray& gripPoints, McGeIntArray& osnapModes, McGeIntArray& geomIds) const
{
	assertReadEnabled();
	gripPoints.append(m_dimPt);
	McGeLine2d line1(m_line1Start, m_line1End);
	McGeLine2d line2(m_line2Start, m_line2End);

	McGePoint2d intPt2d;
	line1.intersectWith(&line2, intPt2d);
	McGePoint3d intPt(intPt2d.x, intPt2d.y, 0);
	gripPoints.append(intPt);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomAngleMeasurement::moveGripPointsAt(const McGeIntArray& indices, const McGeVector3d& offset)
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

Mcad::ErrorStatus Mx2dCustomAngleMeasurement::getGeomExtents(McDbExtents& extents, bool roughCalculation) const
{
	assertReadEnabled();

	McDbVoidPtrArray arr = createAllEntities();
	for (int i = 0; i < arr.length(); i++)
	{
		McDbEntity* pEnt = static_cast<McDbEntity*>(arr[i]);
		McDbExtents ext;
		pEnt->getGeomExtents(ext, roughCalculation);
		extents.addExt(ext);
	}

	return Mcad::eOk;
}

#define ANGLEMEASUREMENT_VERSION 1

Mcad::ErrorStatus Mx2dCustomAngleMeasurement::dwgInFields(McDbDwgFiler* pFiler)
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
	pFiler->readPoint3d(&m_line1Start);
	pFiler->readPoint3d(&m_line1End);
	pFiler->readPoint3d(&m_line2Start);
	pFiler->readPoint3d(&m_line2End);
	pFiler->readPoint3d(&m_dimPt);
	pFiler->readBool(&m_isDynamicDrawing);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomAngleMeasurement::dwgOutFields(McDbDwgFiler* pFiler) const
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
	pFiler->writeInt(ANGLEMEASUREMENT_VERSION);
	pFiler->writePoint3d(m_line1Start);
	pFiler->writePoint3d(m_line1End);
	pFiler->writePoint3d(m_line2Start);
	pFiler->writePoint3d(m_line2End);
	pFiler->writePoint3d(m_dimPt);
	pFiler->writeBool(m_isDynamicDrawing);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomAngleMeasurement::explode(McDbVoidPtrArray& entitySet) const
{
	McDbVoidPtrArray arr = createAllEntities();
	entitySet.append(arr);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomAngleMeasurement::transformBy(const McGeMatrix3d& xform)
{
	assertWriteEnabled();
	m_line1Start.transformBy(xform);
	m_line1End.transformBy(xform);
	m_line2Start.transformBy(xform);
	m_line2End.transformBy(xform);
	m_dimPt.transformBy(xform);
	return Mcad::eOk;
}

void Mx2dCustomAngleMeasurement::fromJson(const QJsonObject& jsonObject)
{
	assertWriteEnabled();
	Mx2dCustomAnnotation::fromJson(jsonObject);
	m_line1Start = Mx2d::jsonArray2dToPoint3d(jsonObject["line1StartPoint"].toArray());
	m_line1End = Mx2d::jsonArray2dToPoint3d(jsonObject["line1EndPoint"].toArray());
	m_line2Start = Mx2d::jsonArray2dToPoint3d(jsonObject["line2StartPoint"].toArray());
	m_line2End = Mx2d::jsonArray2dToPoint3d(jsonObject["line2EndPoint"].toArray());
	m_dimPt = Mx2d::jsonArray2dToPoint3d(jsonObject["dimPoint"].toArray());
	m_isDynamicDrawing = jsonObject["isDynamicDrawing"].toBool();
}

QJsonObject Mx2dCustomAngleMeasurement::toJson() const
{
	assertReadEnabled();
	QJsonObject jsonObject = Mx2dCustomAnnotation::toJson();

	jsonObject["line1StartPoint"] = Mx2d::point3dToJsonArray2d(m_line1Start);
	jsonObject["line1EndPoint"] = Mx2d::point3dToJsonArray2d(m_line1End);
	jsonObject["line2StartPoint"] = Mx2d::point3dToJsonArray2d(m_line2Start);
	jsonObject["line2EndPoint"] = Mx2d::point3dToJsonArray2d(m_line2End);
	jsonObject["dimPoint"] = Mx2d::point3dToJsonArray2d(m_dimPt);
	jsonObject["isDynamicDrawing"] = m_isDynamicDrawing;


	return jsonObject;
}

void Mx2dCustomAngleMeasurement::setLine1Start(const McGePoint3d& line1Start)
{
	assertWriteEnabled();
	m_line1Start = line1Start;
}

McGePoint3d Mx2dCustomAngleMeasurement::line1Start() const
{
	assertReadEnabled();
	return m_line1Start;
}

void Mx2dCustomAngleMeasurement::setLine1End(const McGePoint3d& line1End)
{
	assertWriteEnabled();
	m_line1End = line1End;
}

McGePoint3d Mx2dCustomAngleMeasurement::line1End() const
{
	assertReadEnabled();
	return m_line1End;
}

void Mx2dCustomAngleMeasurement::setLine2Start(const McGePoint3d& line2Start)
{
	assertWriteEnabled();
	m_line2Start = line2Start;
}

McGePoint3d Mx2dCustomAngleMeasurement::line2Start() const
{
	assertReadEnabled();
	return m_line2Start;
}

void Mx2dCustomAngleMeasurement::setLine2End(const McGePoint3d& line2End)
{
	assertWriteEnabled();
	m_line2End = line2End;
}

McGePoint3d Mx2dCustomAngleMeasurement::line2End() const
{
	assertReadEnabled();
	return m_line2End;
}

void Mx2dCustomAngleMeasurement::setIsDynamicDrawing(bool isDynamicDrawing)
{
	assertWriteEnabled();
	m_isDynamicDrawing = isDynamicDrawing;
}

bool Mx2dCustomAngleMeasurement::isDynamicDrawing() const
{
	assertReadEnabled();
	return m_isDynamicDrawing;
}

void Mx2dCustomAngleMeasurement::setDimPt(const McGePoint3d& dimPt)
{
	assertWriteEnabled();
	m_dimPt = dimPt;
}

McGePoint3d Mx2dCustomAngleMeasurement::dimPt() const
{
	assertReadEnabled();
	return m_dimPt;
}




McDbVoidPtrArray Mx2dCustomAngleMeasurement::createAllEntities() const
{

	McDbVoidPtrArray arr;
	McGeLine2d line1(m_line1Start, m_line1End);
	McGeLine2d line2(m_line2Start, m_line2End);

	McGePoint2d intPt2d;
	line1.intersectWith(&line2, intPt2d);
	McGePoint3d intPt(intPt2d.x, intPt2d.y, 0);

	McGeVector3d vecLine1(m_line1End - m_line1Start);
	McGeVector3d vecLine2(m_line2End - m_line2Start);
	McGeVector3d vecDim(m_dimPt - intPt);

	McDbArc* pArc = new McDbArc();
	McDbLine* pExtLine1 = new McDbLine();
	McDbLine* pExtLine2 = new McDbLine();
	McDbText* pText = new McDbText();
	McDbObjectId textStyle = Mx::mcdbCurDwg()->textstyle();
	pText->setHeight(textHeight());
	pText->setTextStyle(textStyle);

	double dist = m_dimPt.distanceTo(intPt);
	double ang = 0.0;
	McGeVector3d dir1, dir2;

	if (Mx2d::isBetweenTwoVecs(vecLine1, vecLine2, vecDim))
	{
		ang = vecLine1.angleTo(vecLine2);

		dir1 = vecLine1.normal();
		dir2 = vecLine2.normal();

	}
	else if (Mx2d::isBetweenTwoVecs(-vecLine1, vecLine2, vecDim))
	{
		ang = (-vecLine1).angleTo(vecLine2);

		dir1 = (-vecLine1).normal();
		dir2 = vecLine2.normal();
	}
	else if (Mx2d::isBetweenTwoVecs(vecLine1, -vecLine2, vecDim))
	{
		ang = vecLine1.angleTo(-vecLine2);

		dir1 = vecLine1.normal();
		dir2 = (-vecLine2).normal();
	}
	else
	{
		ang = (-vecLine1).angleTo(-vecLine2);

		dir1 = (-vecLine1).normal();
		dir2 = (-vecLine2).normal();
	}
	McGePoint3d pt1 = intPt + dir1 * dist;
	McGePoint3d pt2 = intPt + dir2 * dist;
	pArc->computeArc(pt1, m_dimPt, pt2);

	// get arc
	arr.append(pArc);

	double extLen = textHeight() / 2.0;
	McGePoint3d pt11 = pt1 + dir1 * extLen;
	McGePoint3d pt12 = pt1 - dir1 * extLen;
	McGePoint3d pt21 = pt2 + dir2 * extLen;
	McGePoint3d pt22 = pt2 - dir2 * extLen;

	pExtLine1->setStartPoint(pt11);
	pExtLine1->setEndPoint(pt12);
	pExtLine2->setStartPoint(pt21);
	pExtLine2->setEndPoint(pt22);
	// get two ext lines
	arr.append(pExtLine1);
	arr.append(pExtLine2);

	QString num = QString::number(ang * 180 / M_PI, 'f', 2);
	QString str = QString(u8"%1°").arg(num);
	pText->setTextString(str.toLocal8Bit().constData());
	McDbExtents extents;
	pText->getGeomExtents(extents, false);
	McGePoint3d minPt = extents.minPoint();
	McGePoint3d maxPt = extents.maxPoint();
	McGePoint3d mbAnchorPt((minPt.x + maxPt.x) / 2.0, minPt.y - textHeight() / 4.0, 0);

	McGeVector3d vecMid = McGeVector3d(McGePoint3d((pt1.x + pt2.x) / 2.0, (pt1.y + pt2.y) / 2.0, 0) - intPt);
	McGePoint3d ptMid = intPt + vecMid.normal() * dist;
	if (vecMid.x >= 0 && vecMid.y >= 0 || vecMid.x <= 0 && vecMid.y <= 0)
	{
		// 1,3
		McGeVector3d rotVec(fabs(vecMid.x), fabs(vecMid.y), 0);
		double angRot = rotVec.angleTo(McGeVector3d::kXAxis);
		pText->transformBy(McGeMatrix3d::rotation(-(M_PI / 2.0 - angRot), McGeVector3d::kZAxis, mbAnchorPt));
	}
	else
	{
		// 2,4
		McGeVector3d rotVec(fabs(vecMid.x), -fabs(vecMid.y), 0);
		double angRot = rotVec.angleTo(McGeVector3d::kXAxis);
		pText->transformBy(McGeMatrix3d::rotation(M_PI / 2.0 - angRot, McGeVector3d::kZAxis, mbAnchorPt));
	}
	pText->transformBy(McGeMatrix3d::translation(ptMid - mbAnchorPt));

	// get text
	arr.append(pText);

	if (m_isDynamicDrawing)
	{
		arr.append(new McDbLine(intPt, pt1));
		arr.append(new McDbLine(intPt, pt2));
	}

	return arr;
}


