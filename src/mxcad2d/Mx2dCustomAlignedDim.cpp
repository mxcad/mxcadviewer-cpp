/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dCustomAlignedDim.h"
#include "Mx2dUtils.h"
#include <QString>
#include <QJsonArray>
#include "Mx2dAnnotationFactory.h"

REGISTER_ANNOTATION("alignedDim", Mx2dCustomAlignedDim);

ACRX_DXF_DEFINE_MEMBERS(Mx2dCustomAlignedDim, Mx2dCustomAnnotation,
	McDb::kDHL_CURRENT, McDb::kMReleaseCurrent,
	McDbProxyEntity::kAllAllowedBits, AlignedDimMark,
	AlignedDim Mark Entity);


Mx2dCustomAlignedDim::Mx2dCustomAlignedDim(void)
{
	setType("alignedDim");
}

Mx2dCustomAlignedDim::Mx2dCustomAlignedDim(const McGePoint3d& startPt, const McGePoint3d& endPt, const McGePoint3d& dimPt, double textHeight)
	: Mx2dCustomAnnotation(textHeight), m_startPt(startPt), m_endPt(endPt), m_dimPt(dimPt)
{
	setType("alignedDim");
}

#define ALIGNEDDIM_VERSION 1

Mcad::ErrorStatus Mx2dCustomAlignedDim::dwgInFields(McDbDwgFiler* pFiler)
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

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomAlignedDim::dwgOutFields(McDbDwgFiler* pFiler) const
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
	pFiler->writeInt(ALIGNEDDIM_VERSION);
	pFiler->writePoint3d(m_startPt);
	pFiler->writePoint3d(m_endPt);
	pFiler->writePoint3d(m_dimPt);

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomAlignedDim::explode(McDbVoidPtrArray& entitySet) const
{
	assertReadEnabled();

	McDbLine* pDimLine = createDimLine();
	McDbLine* pExtLine1 = createExtLine1();
	McDbLine* pExtLine2 = createExtLine2();
	McDbLine* pObliqueLine1 = createObliqueLine1();
	McDbLine* pObliqueLine2 = createObliqueLine2();
	McDbText* pText = createText();

	entitySet.append(pDimLine);
	entitySet.append(pExtLine1);
	entitySet.append(pExtLine2);
	entitySet.append(pObliqueLine1);
	entitySet.append(pObliqueLine2);
	entitySet.append(pText);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomAlignedDim::transformBy(const McGeMatrix3d& xform)
{
	assertWriteEnabled();
	m_startPt.transformBy(xform);
	m_endPt.transformBy(xform);
	m_dimPt.transformBy(xform);
	return Mcad::eOk;
}

void Mx2dCustomAlignedDim::fromJson(const QJsonObject& jsonObject)
{
	assertWriteEnabled();
	Mx2dCustomAnnotation::fromJson(jsonObject);
	m_startPt = Mx2d::jsonArray2dToPoint3d(jsonObject["startPoint"].toArray());
	m_endPt = Mx2d::jsonArray2dToPoint3d(jsonObject["endPoint"].toArray());
	m_dimPt = Mx2d::jsonArray2dToPoint3d(jsonObject["dimPoint"].toArray());
}

QJsonObject Mx2dCustomAlignedDim::toJson() const
{
	assertReadEnabled();
	QJsonObject jsonObject = Mx2dCustomAnnotation::toJson();
	jsonObject["startPoint"] = Mx2d::point3dToJsonArray2d(m_startPt);
	jsonObject["endPoint"] = Mx2d::point3dToJsonArray2d(m_endPt);
	jsonObject["dimPoint"] = Mx2d::point3dToJsonArray2d(m_dimPt);

	return jsonObject;
}

Mx2d::TextInfoList Mx2dCustomAlignedDim::findText(const QString& text, bool isExactMatch) const
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


void Mx2dCustomAlignedDim::setStartPt(const McGePoint3d& pt)
{
	assertWriteEnabled();
	m_startPt = pt;
}

McGePoint3d Mx2dCustomAlignedDim::startPt() const
{
	assertReadEnabled();
	return m_startPt;
}

void Mx2dCustomAlignedDim::setEndPt(const McGePoint3d& pt)
{
	assertWriteEnabled();
	m_endPt = pt;
}

McGePoint3d Mx2dCustomAlignedDim::endPt() const
{
	assertReadEnabled();
	return m_endPt;
}

void Mx2dCustomAlignedDim::setDimPt(const McGePoint3d& pt)
{
	assertWriteEnabled();
	m_dimPt = pt;
}

McGePoint3d Mx2dCustomAlignedDim::dimPt() const
{
	assertReadEnabled();
	return m_dimPt;
}

McDbLine* Mx2dCustomAlignedDim::createDimLine() const
{
	McGeVector3d vDir = m_endPt - m_startPt;
	McGeVector3d vDim = m_dimPt - m_startPt;
	// vDim projection to vDir
	Mx2d::Vector2d vDim2d(vDim.x, vDim.y);
	Mx2d::Vector2d vDir2d(vDir.x, vDir.y);
	Mx2d::Vector2d vProj = Mx2d::vectorProjection(vDim2d, vDir2d);
	McGeVector3d vProj3d(vProj.x, vProj.y, 0);

	McGeVector3d vOffset = vDim - vProj3d;
	double exLen = textHeight() / 2.0;
	McGePoint3d pt1 = m_startPt + vOffset;
	McGePoint3d pt2 = m_endPt + vOffset;

	McGePoint3d pt1Offset = pt1 - vDir.normalize() * exLen;
	McGePoint3d pt2Offset = pt2 + vDir.normalize() * exLen;

	McDbLine* pLine = new McDbLine(pt1Offset, pt2Offset);
	return pLine;
}

McDbLine* Mx2dCustomAlignedDim::createExtLine1() const
{
	McGeVector3d vDir = m_endPt - m_startPt;
	McGeVector3d vDim = m_dimPt - m_startPt;
	// vDim projection to vDir
	Mx2d::Vector2d vDim2d(vDim.x, vDim.y);
	Mx2d::Vector2d vDir2d(vDir.x, vDir.y);
	Mx2d::Vector2d vProj = Mx2d::vectorProjection(vDim2d, vDir2d);
	McGeVector3d vProj3d(vProj.x, vProj.y, 0);

	McGeVector3d vOffset = vDim - vProj3d;

	double exLen = textHeight() / 2.0;

	McGePoint3d pt1 = m_startPt + vOffset + vOffset.normalize() * exLen;

	McDbLine* pLine = new McDbLine(m_startPt, pt1);

	return pLine;
}

McDbLine* Mx2dCustomAlignedDim::createExtLine2() const
{
	McGeVector3d vDir = m_endPt - m_startPt;
	McGeVector3d vDim = m_dimPt - m_startPt;
	// vDim projection to vDir
	Mx2d::Vector2d vDim2d(vDim.x, vDim.y);
	Mx2d::Vector2d vDir2d(vDir.x, vDir.y);
	Mx2d::Vector2d vProj = Mx2d::vectorProjection(vDim2d, vDir2d);
	McGeVector3d vProj3d(vProj.x, vProj.y, 0);

	McGeVector3d vOffset = vDim - vProj3d;

	double exLen = textHeight() / 2.0;
	McGePoint3d pt1 = m_endPt + vOffset + vOffset.normalize() * exLen;
	McDbLine* pLine = new McDbLine(m_endPt, pt1);
	return pLine;
}

McDbLine* Mx2dCustomAlignedDim::createObliqueLine1() const
{
	McGeVector3d vDir = m_endPt - m_startPt;
	McGeVector3d vDim = m_dimPt - m_startPt;
	// vDim projection to vDir
	Mx2d::Vector2d vDim2d(vDim.x, vDim.y);
	Mx2d::Vector2d vDir2d(vDir.x, vDir.y);
	Mx2d::Vector2d vProj = Mx2d::vectorProjection(vDim2d, vDir2d);
	McGeVector3d vProj3d(vProj.x, vProj.y, 0);

	McGeVector3d vOffset = vDim - vProj3d;
	McGePoint3d pt = m_startPt + vOffset;
	double exLen = textHeight() / 3.0;
	// vOffset rotate -45 degree
	McGeVector3d vObliqueDir = vOffset.rotateBy(-M_PI / 4, McGeVector3d::kZAxis).normalize();
	McGePoint3d pt1 = pt + vObliqueDir * exLen;
	McGePoint3d pt2 = pt - vObliqueDir * exLen;
	McDbLine* pLine = new McDbLine(pt1, pt2);

	return pLine;
}

McDbLine* Mx2dCustomAlignedDim::createObliqueLine2() const
{
	McGeVector3d vDir = m_endPt - m_startPt;
	McGeVector3d vDim = m_dimPt - m_startPt;
	// vDim projection to vDir
	Mx2d::Vector2d vDim2d(vDim.x, vDim.y);
	Mx2d::Vector2d vDir2d(vDir.x, vDir.y);
	Mx2d::Vector2d vProj = Mx2d::vectorProjection(vDim2d, vDir2d);
	McGeVector3d vProj3d(vProj.x, vProj.y, 0);
	McGeVector3d vOffset = vDim - vProj3d;
	McGePoint3d pt = m_endPt + vOffset;
	double exLen = textHeight() / 3.0;
	// vOffset rotate -45 degree
	McGeVector3d vObliqueDir = vOffset.rotateBy(-M_PI / 4, McGeVector3d::kZAxis).normalize();
	McGePoint3d pt1 = pt + vObliqueDir * exLen;
	McGePoint3d pt2 = pt - vObliqueDir * exLen;
	McDbLine* pLine = new McDbLine(pt1, pt2);
	return pLine;
}

McDbText* Mx2dCustomAlignedDim::createText() const
{
	McGeVector3d vDir = m_endPt - m_startPt;
	McGeVector3d vDim = m_dimPt - m_startPt;
	// vDim projection to vDir
	Mx2d::Vector2d vDim2d(vDim.x, vDim.y);
	Mx2d::Vector2d vDir2d(vDir.x, vDir.y);
	Mx2d::Vector2d vProj = Mx2d::vectorProjection(vDim2d, vDir2d);
	McGeVector3d vProj3d(vProj.x, vProj.y, 0);
	McGeVector3d vOffset = vDim - vProj3d;
	McGePoint3d pt1 = m_startPt + vOffset;
	McGePoint3d pt2 = m_endPt + vOffset;
	McGeVector3d vTextDir;
	if (vOffset.y < 0.0)
	{
		vTextDir = -vOffset;
	}
	else
	{
		vTextDir = vOffset;
	}
	McGePoint3d midPt((pt1.x + pt2.x) / 2, (pt1.y + pt2.y) / 2, 0);
	double exLen = textHeight() / 4.0;
	McGePoint3d textPt = midPt + vTextDir.normalize() * exLen;

	double ang = vTextDir.angleTo(McGeVector3d::kXAxis);


	McDbObjectId textStyle = Mx::mcdbCurDwg()->textstyle();
	McDbText* pText = new McDbText();
	pText->setHeight(textHeight());
	pText->setTextStyle(textStyle);

	QString str = QString::number(vDir.length() * dimRatio(), 'f', 0);
	pText->setTextString(str.toLocal8Bit().constData());

	McDbExtents extents;
	pText->getGeomExtents(extents, false);
	McGePoint3d minPt = extents.minPoint();
	McGePoint3d maxPt = extents.maxPoint();
	McGePoint3d basePt((minPt.x + maxPt.x) / 2, minPt.y, 0);

	// text rotate ang - M_PI / 2 by basePt, then move to textPt
	McGeMatrix3d matRotate = McGeMatrix3d::rotation(ang - M_PI / 2, McGeVector3d::kZAxis, basePt);
	McGeMatrix3d matTrans = McGeMatrix3d::translation(textPt - basePt);
	McGeMatrix3d mat = matTrans * matRotate;
	pText->transformBy(mat);
	return pText;

}

Mx2dCustomAlignedDim::~Mx2dCustomAlignedDim(void)
{
}

Mdesk::Boolean Mx2dCustomAlignedDim::worldDraw(McGiWorldDraw* wd)
{
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

	McDbText* pText = createText();
	pText->worldDraw(wd);
	delete pText;

	return Mdesk::kTrue;
}

Mcad::ErrorStatus Mx2dCustomAlignedDim::getGripPoints(McGePoint3dArray& gripPoints, McGeIntArray& osnapModes, McGeIntArray& geomIds) const
{
	assertReadEnabled();
	gripPoints.append(m_startPt);
	gripPoints.append(m_endPt);
	gripPoints.append(m_dimPt);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomAlignedDim::moveGripPointsAt(const McGeIntArray& indices, const McGeVector3d& offset)
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
		break;
	}
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomAlignedDim::getGeomExtents(McDbExtents& extents, bool roughCalculation) const
{
	assertReadEnabled();
	McDbExtents ext;
	McDbLine* pDimLine = createDimLine();
	pDimLine->getGeomExtents(ext, roughCalculation);
	extents.addExt(ext);
	McDbLine* pExtLine1 = createExtLine1();
	pExtLine1->getGeomExtents(ext, roughCalculation);
	extents.addExt(ext);
	McDbLine* pExtLine2 = createExtLine2();
	pExtLine2->getGeomExtents(ext, roughCalculation);
	extents.addExt(ext);
	McDbLine* pObliqueLine1 = createObliqueLine1();
	pObliqueLine1->getGeomExtents(ext, roughCalculation);
	extents.addExt(ext);
	McDbLine* pObliqueLine2 = createObliqueLine2();
	pObliqueLine2->getGeomExtents(ext, roughCalculation);
	extents.addExt(ext);
	McDbText* pText = createText();
	pText->getGeomExtents(ext, roughCalculation);
	extents.addExt(ext);

	delete pDimLine;
	delete pExtLine1;
	delete pExtLine2;
	delete pObliqueLine1;
	delete pObliqueLine2;
	delete pText;

	return Mcad::eOk;
}
