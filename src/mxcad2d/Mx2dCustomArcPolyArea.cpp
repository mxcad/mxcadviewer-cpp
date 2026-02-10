/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dCustomArcPolyArea.h"
#include <QCoreApplication>
#include "Mx2dAnnotationFactory.h"

REGISTER_ANNOTATION("arcPolyArea", Mx2dCustomArcPolyArea);

ACRX_DXF_DEFINE_MEMBERS(Mx2dCustomArcPolyArea, Mx2dCustomAnnotation,
	McDb::kDHL_CURRENT, McDb::kMReleaseCurrent,
	McDbProxyEntity::kAllAllowedBits, ArcPolyAreaMark,
	ArcPolyArea Mark Entity);

Mx2dCustomArcPolyArea::Mx2dCustomArcPolyArea(void)
{
	setType("arcPolyArea");
}

Mx2dCustomArcPolyArea::Mx2dCustomArcPolyArea(const Mx2d::PLVertexList& pts, const McGePoint3d& textPos, double textHeight)
	:Mx2dCustomAnnotation(textHeight), m_pts(pts), m_textPos(textPos)
{
	setType("arcPolyArea");
}

Mx2dCustomArcPolyArea::~Mx2dCustomArcPolyArea(void)
{
}

Mdesk::Boolean Mx2dCustomArcPolyArea::worldDraw(McGiWorldDraw* wd)
{

	McDbPolyline* pPoly = createPoly();
	pPoly->worldDraw(wd);
	double area = getPolyArea(pPoly);
	double perimeter = getPerimeter(pPoly);
	delete pPoly;

	McDbText* pText = createText(area, perimeter);
	pText->worldDraw(wd);
	delete pText;

	return Mdesk::kTrue;
}

Mcad::ErrorStatus Mx2dCustomArcPolyArea::getGripPoints(McGePoint3dArray& gripPoints, McGeIntArray& osnapModes, McGeIntArray& geomIds) const
{
	assertReadEnabled();

	for (int i = 0; i < m_pts.length(); ++i) {
		gripPoints.append(m_pts[i].pt);
	}
	gripPoints.append(m_textPos);

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomArcPolyArea::moveGripPointsAt(const McGeIntArray& indices, const McGeVector3d& offset)
{
	assertWriteEnabled();
	int iIndex = indices[0];

	if (iIndex == m_pts.length()) {
		m_textPos = m_textPos + offset;
	}
	else {
		m_pts[iIndex].pt = m_pts[iIndex].pt + offset;
	}

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomArcPolyArea::getGeomExtents(McDbExtents& extents, bool roughCalculation) const
{
	assertReadEnabled();

	McDbExtents ext;
	McDbPolyline* pPoly = createPoly();
	pPoly->getGeomExtents(ext, false);
	double area = getPolyArea(pPoly);
	double perimeter = getPerimeter(pPoly);
	extents.addExt(ext);
	delete pPoly;

	McDbText* pText = createText(area, perimeter);
	pText->getGeomExtents(ext, false);
	extents.addExt(ext);
	delete pText;


	return Mcad::eOk;
}

#define ARCPOLYAREA_VERSION 1


Mcad::ErrorStatus Mx2dCustomArcPolyArea::dwgInFields(McDbDwgFiler* pFiler)
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
	// read number of vertex
	int ptCount = 0;
	pFiler->readInt(&ptCount);

	// read polygon
	m_pts.clear();
	for (int i = 0; i < ptCount; ++i) {
		McGePoint3d pt;
		double bulge = 0.0;
		pFiler->readPoint3d(&pt);
		pFiler->readDouble(&bulge);
		m_pts.append({ pt,bulge });
	}
	// read text position
	pFiler->readPoint3d(&m_textPos);


	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomArcPolyArea::dwgOutFields(McDbDwgFiler* pFiler) const
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
	pFiler->writeInt(ARCPOLYAREA_VERSION);

	// write number of vertex
	pFiler->writeInt(m_pts.length());

	// write polygon
	for (int i = 0; i < m_pts.length(); ++i) {
		pFiler->writePoint3d(m_pts[i].pt);
		pFiler->writeDouble(m_pts[i].bulge);
	}
	// write text position
	pFiler->writePoint3d(m_textPos);


	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomArcPolyArea::explode(McDbVoidPtrArray& entitySet) const
{
	assertReadEnabled();
	McDbPolyline* pPoly = createPoly();
	entitySet.append(pPoly);

	double area = getPolyArea(pPoly);
	double perimeter = getPerimeter(pPoly);

	McDbText* pText = createText(area, perimeter);
	entitySet.append(pText);

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomArcPolyArea::transformBy(const McGeMatrix3d& xform)
{
	assertWriteEnabled();

	for (int i = 0; i < m_pts.length(); ++i) {
		McGePoint3d& pt = m_pts[i].pt;
		pt.transformBy(xform);
	}
	m_textPos.transformBy(xform);

	return Mcad::eOk;
}

void Mx2dCustomArcPolyArea::fromJson(const QJsonObject& jsonObject)
{
	assertWriteEnabled();
	Mx2dCustomAnnotation::fromJson(jsonObject);
	m_pts = Mx2d::jsonArrayToPLVertexList(jsonObject["points"].toArray());
	m_textPos = Mx2d::jsonArray2dToPoint3d(jsonObject["textPosition"].toArray());
}

QJsonObject Mx2dCustomArcPolyArea::toJson() const
{
	assertReadEnabled();
	QJsonObject jsonObject = Mx2dCustomAnnotation::toJson();
	jsonObject["points"] = Mx2d::plVertexListToJsonArray(m_pts);
	jsonObject["textPosition"] = Mx2d::point3dToJsonArray2d(m_textPos);
	return jsonObject;
}

Mx2d::TextInfoList Mx2dCustomArcPolyArea::findText(const QString& text, bool isExactMatch) const
{
	assertReadEnabled();

	McDbPolyline* pPoly = createPoly();
	double area = getPolyArea(pPoly);
	double perimeter = getPerimeter(pPoly);
	delete pPoly;

	McDbExtents ext;
	McDbText* pText = createText(area, perimeter);
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

Mx2d::PLVertexList Mx2dCustomArcPolyArea::points() const
{
	assertReadEnabled();
	return m_pts;
}

void Mx2dCustomArcPolyArea::setTextPos(const McGePoint3d& pos)
{
	assertWriteEnabled();
	m_textPos = pos;
}

McGePoint3d Mx2dCustomArcPolyArea::textPos() const
{
	assertReadEnabled();
	return m_textPos;
}

void Mx2dCustomArcPolyArea::setPoints(const Mx2d::PLVertexList& pts)
{
	assertWriteEnabled();
	m_pts = pts;
}

McDbPolyline* Mx2dCustomArcPolyArea::createPoly() const
{
	McDbPolyline* pPoly = new McDbPolyline();
	for (int i = 0; i < m_pts.length(); i++)
	{
		pPoly->addVertexAt(m_pts[i].pt, m_pts[i].bulge);
	}
	pPoly->setClosed(Mdesk::kTrue);
	return pPoly;
}

McDbText* Mx2dCustomArcPolyArea::createText(double area, double perimeter) const
{
	McDbObjectId textStyle = Mx::mcdbCurDwg()->textstyle();

	McDbText* pText = new McDbText();
	pText->setAlignmentPoint(m_textPos);
	pText->setHeight(textHeight());
	QString areaStr = QCoreApplication::translate("Mx2dCustomArcPolyArea", "Area:%1, Perimeter:%2").arg(QString::number(area * dimRatio() * dimRatio())).arg(QString::number(perimeter * dimRatio()));

	pText->setTextString(areaStr.toLocal8Bit().constData());
	pText->setTextStyle(textStyle);

	return pText;
}

double Mx2dCustomArcPolyArea::getPolyArea(McDbPolyline* pPoly) const
{
	if (pPoly != nullptr) {
		double area = 0.0;
		pPoly->getArea(area);
		return area;
	}
	return 0.0;
}

double Mx2dCustomArcPolyArea::getPerimeter(McDbPolyline* pPoly) const
{
	if (pPoly != nullptr) {
		double perimeter = 0.0;

		McGePoint3d lastPt = m_pts[m_pts.length() - 1].pt;
		double lastBulge = m_pts[m_pts.length() - 1].bulge;

		double lastParameter = 0.0;
		pPoly->getEndParam(lastParameter);
		pPoly->getDistAtParam(lastParameter, perimeter);

		McGePoint3d firstPt = m_pts[0].pt;
		if (Mx2d::IsEqual(lastBulge, 0.0))
		{
			perimeter += firstPt.distanceTo(lastPt);
		}
		else
		{
			McGePoint3d center;
			double radius, angle;
			Mx2d::calcArc(lastPt, firstPt, lastBulge, center, radius, angle);
			perimeter += radius * angle;
		}


		return perimeter;
	}
	return 0.0;
}



