/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dCustomContinuousMeasurement.h"
#include <QString>
#include "Mx2dAnnotationFactory.h"

REGISTER_ANNOTATION("continuousMeasurement", Mx2dCustomContinuousMeasurement);
ACRX_DXF_DEFINE_MEMBERS(Mx2dCustomContinuousMeasurement, Mx2dCustomAnnotation,
	McDb::kDHL_CURRENT, McDb::kMReleaseCurrent,
	McDbProxyEntity::kAllAllowedBits, ContinuousMeasurementMark,
	ContinuousMeasurement Mark Entity);

Mx2dCustomContinuousMeasurement::Mx2dCustomContinuousMeasurement(void)
{
	setType("continuousMeasurement");
}

Mx2dCustomContinuousMeasurement::Mx2dCustomContinuousMeasurement(const Mx2d::PLVertexList& pts, const McGePoint3d& dimPt, double textHeight)
	:Mx2dCustomAnnotation(textHeight), m_pts(pts), m_dimPt(dimPt)
{
	setType("continuousMeasurement");
}

Mx2dCustomContinuousMeasurement::~Mx2dCustomContinuousMeasurement(void)
{
}

Mdesk::Boolean Mx2dCustomContinuousMeasurement::worldDraw(McGiWorldDraw* wd)
{
	McDbPolyline* pPoly = createPoly();
	pPoly->worldDraw(wd);
	McDbLine* pStartEdgeLine = createStartEdge(pPoly);
	if (pStartEdgeLine)
	{
		pStartEdgeLine->worldDraw(wd);
		delete pStartEdgeLine;
	}
	McDbLine* pEndEdgeLine = createEndEdge(pPoly);
	if (pEndEdgeLine)
	{
		pEndEdgeLine->worldDraw(wd);
		delete pEndEdgeLine;
	}
	double textLength = 0.0;
	McDbText* pText = createText(pPoly, textLength);
	if (pText)
	{
		pText->worldDraw(wd);
		delete pText;
	}
	McDbPolyline* pDimLine = createDimLine(textLength);
	pDimLine->worldDraw(wd);
	delete pDimLine;


	delete pPoly;


	return Mdesk::kTrue;
}

Mcad::ErrorStatus Mx2dCustomContinuousMeasurement::getGripPoints(McGePoint3dArray& gripPoints, McGeIntArray& osnapModes, McGeIntArray& geomIds) const
{
	assertReadEnabled();
	for (int i = 0; i < m_pts.length(); ++i) {
		gripPoints.append(m_pts[i].pt);
	}
	gripPoints.append(m_dimPt);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomContinuousMeasurement::moveGripPointsAt(const McGeIntArray& indices, const McGeVector3d& offset)
{
	assertWriteEnabled();
	int iIndex = indices[0];

	if (iIndex == m_pts.length()) {
		m_dimPt = m_dimPt + offset;
	}
	else {
		m_pts[iIndex].pt = m_pts[iIndex].pt + offset;
	}
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomContinuousMeasurement::getGeomExtents(McDbExtents& extents, bool roughCalculation) const
{
	assertReadEnabled();
	McDbExtents ext;
	McDbPolyline* pPoly = createPoly();
	pPoly->getGeomExtents(ext, roughCalculation);
	extents.addExt(ext);
	McDbLine* pStartEdgeLine = createStartEdge(pPoly);
	pStartEdgeLine->getGeomExtents(ext, roughCalculation);
	extents.addExt(ext);
	delete pStartEdgeLine;
	McDbLine* pEndEdgeLine = createEndEdge(pPoly);
	pEndEdgeLine->getGeomExtents(ext, roughCalculation);
	extents.addExt(ext);
	delete pEndEdgeLine;
	double textLength = 0.0;
	McDbText* pText = createText(pPoly, textLength);
	pText->getGeomExtents(ext, roughCalculation);
	extents.addExt(ext);
	delete pText;
	McDbPolyline* pDimLine = createDimLine(textLength);
	pDimLine->getGeomExtents(ext, roughCalculation);
	extents.addExt(ext);
	delete pDimLine;

	delete pPoly;

	return Mcad::eOk;
}

#define CONTINUOUSMEASUREMENT_VERSION 1

Mcad::ErrorStatus Mx2dCustomContinuousMeasurement::dwgInFields(McDbDwgFiler* pFiler)
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
	if (lVar != CONTINUOUSMEASUREMENT_VERSION)
	{
		return Mcad::eInvalidDwgVersion;
	}
	int ptsCount = 0;
	pFiler->readInt(&ptsCount);
	m_pts.clear();
	for (int i = 0; i < ptsCount; i++)
	{
		Mx2d::PLVertex v;
		pFiler->readPoint3d(&v.pt);
		pFiler->readDouble(&v.bulge);
		m_pts.append(v);
	}
	pFiler->readPoint3d(&m_dimPt);

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomContinuousMeasurement::dwgOutFields(McDbDwgFiler* pFiler) const
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
	pFiler->writeInt(CONTINUOUSMEASUREMENT_VERSION);
	pFiler->writeInt(m_pts.size());
	for (int i = 0; i < m_pts.size(); i++)
	{
		const Mx2d::PLVertex& v = m_pts[i];
		pFiler->writePoint3d(v.pt);
		pFiler->writeDouble(v.bulge);
	}
	pFiler->writePoint3d(m_dimPt);

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomContinuousMeasurement::explode(McDbVoidPtrArray& entitySet) const
{
	McDbPolyline* pPoly = createPoly();
	entitySet.append(pPoly);

	McDbLine* pStartEdgeLine = createStartEdge(pPoly);
	entitySet.append(pStartEdgeLine);

	McDbLine* pEndEdgeLine = createEndEdge(pPoly);
	entitySet.append(pEndEdgeLine);

	double textLength = 0.0;
	McDbText* pText = createText(pPoly, textLength);
	entitySet.append(pText);

	McDbPolyline* pDimLine = createDimLine(textLength);
	entitySet.append(pDimLine);

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomContinuousMeasurement::transformBy(const McGeMatrix3d& xform)
{
	assertWriteEnabled();
	for (int i = 0; i < m_pts.length(); ++i) {
		McGePoint3d& pt = m_pts[i].pt;
		pt.transformBy(xform);
	}
	m_dimPt.transformBy(xform);
	return Mcad::eOk;
}

void Mx2dCustomContinuousMeasurement::fromJson(const QJsonObject& jsonObject)
{
	assertWriteEnabled();
	Mx2dCustomAnnotation::fromJson(jsonObject);
	Mx2d::PLVertexList pts;
	QJsonArray pointsArray = jsonObject["points"].toArray();
	for (auto v : pointsArray)
	{
		QJsonArray pointArray = v.toArray();
		Mx2d::PLVertex v;
		v.pt = Mx2d::jsonArray2dToPoint3d(pointArray);
		v.bulge = pointArray[2].toDouble();
		pts.append(v);
	}
	m_pts = pts;
	m_dimPt = Mx2d::jsonArray2dToPoint3d(jsonObject["dimPoint"].toArray());
}

QJsonObject Mx2dCustomContinuousMeasurement::toJson() const
{
	assertReadEnabled();
	QJsonObject jsonObject = Mx2dCustomAnnotation::toJson();
	QJsonArray pointsArray;
	for (auto v : m_pts)
	{
		QJsonArray pointArray;
		pointArray.append(v.pt.x);
		pointArray.append(v.pt.y);
		pointArray.append(v.bulge);
		pointsArray.append(pointArray);
	}
	jsonObject["points"] = pointsArray;
	jsonObject["dimPoint"] = Mx2d::point3dToJsonArray2d(m_dimPt);

	return jsonObject;
}

Mx2d::TextInfoList Mx2dCustomContinuousMeasurement::findText(const QString& text, bool isExactMatch) const
{
	assertReadEnabled();
	McDbExtents ext;
	McDbPolyline* pPoly = createPoly();
	double textLength = 0.0;
	McDbText* pText = createText(pPoly, textLength);
	if (!pText)
	{
        delete pPoly;
        return {};
	}
	pText->getGeomExtents(ext, false);
	QString textStr = QString::fromLocal8Bit(pText->textString());
	McGePoint3d minPt = ext.minPoint();
	McGePoint3d maxPt = ext.maxPoint();
	Mx2d::Extents extents{ minPt.x, minPt.y,maxPt.x, maxPt.y };
	delete pPoly;
	delete pText;
	if ((isExactMatch && (text != textStr)) ||
		(!isExactMatch && !textStr.contains(text, Qt::CaseInsensitive)))
	{
		return {};
	}

	return { {textStr , extents} };
}

void Mx2dCustomContinuousMeasurement::setPoints(const Mx2d::PLVertexList& pts)
{
	assertWriteEnabled();
	m_pts = pts;
}

Mx2d::PLVertexList Mx2dCustomContinuousMeasurement::points() const
{
	assertReadEnabled();
	return m_pts;
}

void Mx2dCustomContinuousMeasurement::setDimPt(const McGePoint3d& pt)
{
	assertWriteEnabled();
	m_dimPt = pt;
}

McGePoint3d Mx2dCustomContinuousMeasurement::dimPt() const
{
	assertReadEnabled();
	return m_dimPt;
}

Mcad::ErrorStatus Mx2dCustomContinuousMeasurement::explodePoly(McDbVoidPtrArray& entitySet) const
{
	McDbPolyline* pPoly = createPoly();
	if (pPoly)
	{
		McDbVoidPtrArray polyEntitySet;
		Mcad::ErrorStatus es = pPoly->explode(polyEntitySet);
		delete pPoly;
		if (es != Mcad::eOk)
		{
			return es;
		}

		entitySet.append(polyEntitySet);
	}



	return Mcad::eOk;
}

McDbPolyline* Mx2dCustomContinuousMeasurement::createPoly() const
{
	McDbPolyline* pPoly = new McDbPolyline();
	for (int i = 0; i < m_pts.size(); i++)
	{
		const Mx2d::PLVertex& v = m_pts[i];
		pPoly->addVertexAt(v.pt, v.bulge);
	}

	return pPoly;
}

McDbLine* Mx2dCustomContinuousMeasurement::createStartEdge(McDbPolyline* poly) const
{
	if (!poly)
	{
		return nullptr;
	}

	// 获取poly的起点的一阶导数方向逆时针旋转45度所在的直线
	McGePoint3d startPt;
	poly->getStartPoint(startPt);
	McGeVector3d dir;
	poly->getFirstDeriv(startPt, dir);
	double extLen = textHeight() / 3.0;
	dir.normalize();
	dir.rotateBy(M_PI / 4, McGeVector3d::kZAxis);
	McGePoint3d pt1 = startPt + dir * extLen;
	McGePoint3d pt2 = startPt - dir * extLen;
	return new McDbLine(pt1, pt2);
}

McDbLine* Mx2dCustomContinuousMeasurement::createEndEdge(McDbPolyline* poly) const
{
	if (!poly)
	{
		return nullptr;
	}

	McGePoint3d endPt;
	poly->getEndPoint(endPt);
	McGeVector3d dir;
	poly->getFirstDeriv(endPt, dir);
	double extLen = textHeight() / 3.0;
	dir.normalize();
	dir.rotateBy(M_PI / 4, McGeVector3d::kZAxis);
	McGePoint3d pt1 = endPt + dir * extLen;
	McGePoint3d pt2 = endPt - dir * extLen;
	return new McDbLine(pt1, pt2);
}

McDbPolyline* Mx2dCustomContinuousMeasurement::createDimLine(double textLength) const
{
	McGePoint3d endPt = m_pts[m_pts.size() - 1].pt;
	McGePoint3d lastPt(m_dimPt.x + textLength, m_dimPt.y, 0);
	McDbPolyline* pPoly = new McDbPolyline();

	pPoly->addVertexAt(endPt);
	pPoly->addVertexAt(m_dimPt);
	pPoly->addVertexAt(lastPt);

	return pPoly;
}

McDbText* Mx2dCustomContinuousMeasurement::createText(McDbPolyline* poly, double& textLength) const
{
	double length = 0.0;
	McGePoint3d endPt;
	poly->getEndPoint(endPt);
	poly->getDistAtPoint(endPt, length);
	McDbObjectId textStyle = Mx::mcdbCurDwg()->textstyle();
	McDbText* pText = new McDbText();
	pText->setHeight(textHeight());
	pText->setTextStyle(textStyle);
	QString str = QString::number(length * dimRatio(), 'f', 2);
	pText->setTextString(str.toLocal8Bit().constData());
	McDbExtents extents;
	pText->getGeomExtents(extents, false);
	McGePoint3d minPt = extents.minPoint();
	McGePoint3d maxPt = extents.maxPoint();
	McGePoint3d lbAnchorPt(minPt.x, minPt.y - textHeight() / 4.0, 0);
	McGePoint3d rbAnchorPt(maxPt.x, minPt.y - textHeight() / 4.0, 0);
	McGeVector3d vec = m_dimPt - endPt;

	double ang = vec.angleTo(McGeVector3d::kXAxis);

	if (ang < M_PI / 2)
	{
		textLength = fabs(maxPt.x - minPt.x);
		McGeVector3d moveVec = m_dimPt - lbAnchorPt;
		pText->transformBy(McGeMatrix3d::translation(moveVec));
	}
	else
	{
		textLength = -fabs(maxPt.x - minPt.x);
		McGeVector3d moveVec = m_dimPt - rbAnchorPt;
		pText->transformBy(McGeMatrix3d::translation(moveVec));
	}


	return pText;
}
