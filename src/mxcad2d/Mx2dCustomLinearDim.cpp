/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dCustomLinearDim.h"
#include "Mx2dUtils.h"
#include <QString>
#include "Mx2dAnnotationFactory.h"

REGISTER_ANNOTATION("linearDim", Mx2dCustomLinearDim);
ACRX_DXF_DEFINE_MEMBERS(Mx2dCustomLinearDim, Mx2dCustomAnnotation,
	McDb::kDHL_CURRENT, McDb::kMReleaseCurrent,
	McDbProxyEntity::kAllAllowedBits, LinearDimMark,
	LinearDim Mark Entity);


Mx2dCustomLinearDim::Mx2dCustomLinearDim(void)
{
	setType("linearDim");
}

Mx2dCustomLinearDim::Mx2dCustomLinearDim(const McGePoint3d& startPt, const McGePoint3d& endPt, const McGePoint3d& dimPt, double textHeight)
	: Mx2dCustomAnnotation(textHeight), m_startPt(startPt), m_endPt(endPt), m_dimPt(dimPt)
{
	double dx = fabs(m_endPt.x - m_startPt.x);
	double dy = fabs(m_endPt.y - m_startPt.y);
	if (dx > dy)
	{
		m_bHorizontal = true;
	}
	else
	{
		m_bHorizontal = false;
	}
	setType("linearDim");
}

#define LINEARDIM_VERSION 1

Mcad::ErrorStatus Mx2dCustomLinearDim::dwgInFields(McDbDwgFiler* pFiler)
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
	pFiler->readPoint3d(&m_dimPt);
	pFiler->readBool(&m_bHorizontal);

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomLinearDim::dwgOutFields(McDbDwgFiler* pFiler) const
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
	pFiler->writeInt(LINEARDIM_VERSION);
	pFiler->writePoint3d(m_startPt);
	pFiler->writePoint3d(m_endPt);
	pFiler->writePoint3d(m_dimPt);
	pFiler->writeBool(m_bHorizontal);

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomLinearDim::explode(McDbVoidPtrArray& entitySet) const
{

	McDbText* pText = createText();
	entitySet.append(pText);

	McDbLine* pDimLine = createDimLine();
	entitySet.append(pDimLine);

	McDbLine* pExtLine1 = createExtLine1();
	entitySet.append(pExtLine1);

	McDbLine* pExtLine2 = createExtLine2();
	entitySet.append(pExtLine2);

	McDbLine* pObliqueLine1 = createObliqueLine1();
	entitySet.append(pObliqueLine1);

	McDbLine* pObliqueLine2 = createObliqueLine2();
	entitySet.append(pObliqueLine2);

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomLinearDim::transformBy(const McGeMatrix3d& xform)
{
	return Mcad::ErrorStatus();
}

void Mx2dCustomLinearDim::fromJson(const QJsonObject& jsonObject)
{
	assertWriteEnabled();
	Mx2dCustomAnnotation::fromJson(jsonObject);
	m_startPt = Mx2d::jsonArray2dToPoint3d(jsonObject["startPoint"].toArray());
	m_endPt = Mx2d::jsonArray2dToPoint3d(jsonObject["endPoint"].toArray());
	m_dimPt = Mx2d::jsonArray2dToPoint3d(jsonObject["dimPoint"].toArray());
	m_bHorizontal = jsonObject["isHorizontal"].toBool();
}

QJsonObject Mx2dCustomLinearDim::toJson() const
{
	assertReadEnabled();
	QJsonObject jsonObject = Mx2dCustomAnnotation::toJson();

	jsonObject["startPoint"] = Mx2d::point3dToJsonArray2d(m_startPt);
	jsonObject["endPoint"] = Mx2d::point3dToJsonArray2d(m_endPt);
	jsonObject["dimPoint"] = Mx2d::point3dToJsonArray2d(m_dimPt);
	jsonObject["isHorizontal"] = m_bHorizontal;

	return jsonObject;
}

void Mx2dCustomLinearDim::setStartPt(const McGePoint3d& startPt)
{
	assertWriteEnabled();
	m_startPt = startPt;
}

McGePoint3d Mx2dCustomLinearDim::startPt() const
{
	assertReadEnabled();
	return m_startPt;
}

void Mx2dCustomLinearDim::setEndPt(const McGePoint3d& endPt)
{
	assertWriteEnabled();
	m_endPt = endPt;
}

McGePoint3d Mx2dCustomLinearDim::endPt() const
{
	assertReadEnabled();
	return m_endPt;
}

void Mx2dCustomLinearDim::setDimPt(const McGePoint3d& dimPt)
{
	assertWriteEnabled();
	m_dimPt = dimPt;
}

McGePoint3d Mx2dCustomLinearDim::dimPt() const
{
	assertReadEnabled();
	return m_dimPt;
}

void Mx2dCustomLinearDim::setIsHorizontal(bool bHorizontal)
{
	assertWriteEnabled();
	m_bHorizontal = bHorizontal;
}

bool Mx2dCustomLinearDim::isHorizontal() const
{
	assertReadEnabled();
	return m_bHorizontal;
}

McDbLine* Mx2dCustomLinearDim::createDimLine() const
{
	double exLen = textHeight() / 2.0;
	if (m_bHorizontal)
	{
		if (m_startPt.x < m_endPt.x)
		{
			McGePoint3d pt1 = McGePoint3d(m_startPt.x - exLen, m_dimPt.y, 0);
			McGePoint3d pt2 = McGePoint3d(m_endPt.x + exLen, m_dimPt.y, 0);
			McDbLine* pLine = new McDbLine(pt1, pt2);
			return pLine;
		}
		else
		{
			McGePoint3d pt1 = McGePoint3d(m_endPt.x - exLen, m_dimPt.y, 0);
			McGePoint3d pt2 = McGePoint3d(m_startPt.x + exLen, m_dimPt.y, 0);
			McDbLine* pLine = new McDbLine(pt1, pt2);
			return pLine;
		}
	}
	else
	{

		if (m_startPt.y < m_endPt.y)
		{
			McGePoint3d pt1 = McGePoint3d(m_dimPt.x, m_startPt.y - exLen, 0);
			McGePoint3d pt2 = McGePoint3d(m_dimPt.x, m_endPt.y + exLen, 0);
			McDbLine* pLine = new McDbLine(pt1, pt2);
			return pLine;
		}
		else
		{
			McGePoint3d pt1 = McGePoint3d(m_dimPt.x, m_endPt.y - exLen, 0);
			McGePoint3d pt2 = McGePoint3d(m_dimPt.x, m_startPt.y + exLen, 0);
			McDbLine* pLine = new McDbLine(pt1, pt2);
			return pLine;
		}
	}
}

McDbLine* Mx2dCustomLinearDim::createExtLine1() const
{
	double exLen = textHeight() / 2.0;
	if (m_bHorizontal)
	{
		if (m_dimPt.y < m_startPt.y)
		{
			McGePoint3d pt(m_startPt.x, m_dimPt.y - exLen, 0);
			McDbLine* pLine = new McDbLine(m_startPt, pt);
			return pLine;
		}
		else
		{
			McGePoint3d pt(m_startPt.x, m_dimPt.y + exLen, 0);
			McDbLine* pLine = new McDbLine(m_startPt, pt);
			return pLine;
		}
	}
	else
	{
		if (m_dimPt.x < m_startPt.x)
		{
			McGePoint3d pt(m_dimPt.x - exLen, m_startPt.y, 0);
			McDbLine* pLine = new McDbLine(m_startPt, pt);
			return pLine;
		}
		else
		{
			McGePoint3d pt(m_dimPt.x + exLen, m_startPt.y, 0);
			McDbLine* pLine = new McDbLine(m_startPt, pt);
			return pLine;
		}
	}


}

McDbLine* Mx2dCustomLinearDim::createExtLine2() const
{
	double exLen = textHeight() / 2.0;
	if (m_bHorizontal)
	{
		if (m_dimPt.y < m_endPt.y)
		{
			McGePoint3d pt(m_endPt.x, m_dimPt.y - exLen, 0);
			McDbLine* pLine = new McDbLine(m_endPt, pt);
			return pLine;
		}
		else
		{
			McGePoint3d pt(m_endPt.x, m_dimPt.y + exLen, 0);
			McDbLine* pLine = new McDbLine(m_endPt, pt);
			return pLine;
		}
	}
	else
	{
		if (m_dimPt.x < m_endPt.x)
		{
			McGePoint3d pt(m_dimPt.x - exLen, m_endPt.y, 0);
			McDbLine* pLine = new McDbLine(m_endPt, pt);
			return pLine;
		}
		else
		{
			McGePoint3d pt(m_dimPt.x + exLen, m_endPt.y, 0);
			McDbLine* pLine = new McDbLine(m_endPt, pt);
			return pLine;
		}
	}
}

McDbLine* Mx2dCustomLinearDim::createObliqueLine1() const
{
	double exLen = textHeight() / 3.0;
	if (m_bHorizontal)
	{
		McGeVector3d vDir = McGeVector3d(1, 1, 0).normal();
		McGePoint3d pt = McGePoint3d(m_startPt.x, m_dimPt.y, 0);
		McGePoint3d pt1 = pt + vDir * exLen;
		McGePoint3d pt2 = pt - vDir * exLen;
		McDbLine* pLine = new McDbLine(pt1, pt2);
		return pLine;
	}
	else
	{
		McGeVector3d vDir = McGeVector3d(1, -1, 0).normal();
		McGePoint3d pt = McGePoint3d(m_dimPt.x, m_startPt.y, 0);
		McGePoint3d pt1 = pt + vDir * exLen;
		McGePoint3d pt2 = pt - vDir * exLen;
		McDbLine* pLine = new McDbLine(pt1, pt2);
		return pLine;
	}
}

McDbLine* Mx2dCustomLinearDim::createObliqueLine2() const
{
	double exLen = textHeight() / 3.0;
	if (m_bHorizontal)
	{
		McGeVector3d vDir = McGeVector3d(1, 1, 0).normal();
		McGePoint3d pt = McGePoint3d(m_endPt.x, m_dimPt.y, 0);
		McGePoint3d pt1 = pt + vDir * exLen;
		McGePoint3d pt2 = pt - vDir * exLen;
		McDbLine* pLine = new McDbLine(pt1, pt2);
		return pLine;
	}
	else
	{
		McGeVector3d vDir = McGeVector3d(1, -1, 0).normal();
		McGePoint3d pt = McGePoint3d(m_dimPt.x, m_endPt.y, 0);
		McGePoint3d pt1 = pt + vDir * exLen;
		McGePoint3d pt2 = pt - vDir * exLen;
		McDbLine* pLine = new McDbLine(pt1, pt2);
		return pLine;
	}
}

McDbText* Mx2dCustomLinearDim::createText() const
{
	double exLen = textHeight() / 4.0;
	McDbObjectId textStyle = Mx::mcdbCurDwg()->textstyle();
	McDbText* pText = new McDbText();
	pText->setHeight(textHeight());
	pText->setTextStyle(textStyle);
	if (m_bHorizontal)
	{
		double length = fabs(m_endPt.x - m_startPt.x);

		McGePoint3d textPt((m_startPt.x + m_endPt.x) / 2, m_dimPt.y + exLen, 0);

		QString str = QString::number(length * dimRatio(), 'f', 0);
		pText->setTextString(str.toLocal8Bit().constData());

		McDbExtents extents;
		pText->getGeomExtents(extents, false);
		McGePoint3d minPt = extents.minPoint();
		McGePoint3d maxPt = extents.maxPoint();
		McGePoint3d basePt((minPt.x + maxPt.x) / 2, minPt.y, 0);


		McGeMatrix3d matTrans = McGeMatrix3d::translation(textPt - basePt);
		pText->transformBy(matTrans);
		return pText;
	}
	else
	{
		double length = fabs(m_endPt.y - m_startPt.y);

		McGePoint3d textPt(m_dimPt.x - exLen, (m_startPt.y + m_endPt.y) / 2, 0);

		QString str = QString::number(length * dimRatio(), 'f', 0);
		pText->setTextString(str.toLocal8Bit().constData());

		McDbExtents extents;
		pText->getGeomExtents(extents, false);
		McGePoint3d minPt = extents.minPoint();
		McGePoint3d maxPt = extents.maxPoint();
		McGePoint3d basePt((minPt.x + maxPt.x) / 2, minPt.y, 0);

		McGeMatrix3d matRotate = McGeMatrix3d::rotation(M_PI / 2, McGeVector3d::kZAxis, basePt);
		McGeMatrix3d matTrans = McGeMatrix3d::translation(textPt - basePt);
		McGeMatrix3d mat = matTrans * matRotate;
		pText->transformBy(mat);
		return pText;
	}

}

Mx2dCustomLinearDim::~Mx2dCustomLinearDim(void)
{
}

Mdesk::Boolean Mx2dCustomLinearDim::worldDraw(McGiWorldDraw* wd)
{
	double minX = min(m_startPt.x, m_endPt.x);
	double maxX = max(m_startPt.x, m_endPt.x);
	double minY = min(m_startPt.y, m_endPt.y);
	double maxY = max(m_startPt.y, m_endPt.y);

	if (m_dimPt.x >= minX && m_dimPt.x <= maxX && (m_dimPt.y >= maxY || m_dimPt.y <= minY))
	{
		m_bHorizontal = true;
	}
	else if (m_dimPt.y >= minY && m_dimPt.y <= maxY && (m_dimPt.x >= maxX || m_dimPt.x <= minX))
	{
		m_bHorizontal = false;
	}

	McDbText* pText = createText();
	pText->worldDraw(wd);
	delete pText;

	McDbLine* pDimLine = createDimLine();
	pDimLine->worldDraw(wd);
	delete pDimLine;

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

Mcad::ErrorStatus Mx2dCustomLinearDim::getGripPoints(McGePoint3dArray& gripPoints, McGeIntArray& osnapModes, McGeIntArray& geomIds) const
{
	assertReadEnabled();
	gripPoints.append(m_startPt);
	gripPoints.append(m_endPt);
	gripPoints.append(m_dimPt);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomLinearDim::moveGripPointsAt(const McGeIntArray& indices, const McGeVector3d& offset)
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
	case 2:
		m_dimPt = m_dimPt + offset;
		double minX = min(m_startPt.x, m_endPt.x);
		double maxX = max(m_startPt.x, m_endPt.x);
		double minY = min(m_startPt.y, m_endPt.y);
		double maxY = max(m_startPt.y, m_endPt.y);

		if (m_dimPt.x >= minX && m_dimPt.x <= maxX && (m_dimPt.y >= maxY || m_dimPt.y <= minY))
		{
			m_bHorizontal = true;
		}
		else if (m_dimPt.y >= minY && m_dimPt.y <= maxY && (m_dimPt.x >= maxX || m_dimPt.x <= minX))
		{
			m_bHorizontal = false;
		}
		break;
	}
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomLinearDim::getGeomExtents(McDbExtents& extents, bool roughCalculation) const
{
	assertReadEnabled();

	McDbExtents ext;

	McDbText* pText = createText();
	pText->getGeomExtents(ext, roughCalculation);
	extents.addExt(ext);
	delete pText;

	McDbLine* pDimLine = createDimLine();
	pDimLine->getGeomExtents(ext, roughCalculation);
	extents.addExt(ext);
	delete pDimLine;

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
