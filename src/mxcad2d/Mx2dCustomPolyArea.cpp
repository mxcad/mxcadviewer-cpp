/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dCustomPolyArea.h"
#include <QJsonArray>
#include "Mx2dUtils.h"
#include <QCoreApplication>
#include "Mx2dAnnotationFactory.h"

REGISTER_ANNOTATION("polyArea", Mx2dCustomPolyArea);
ACRX_DXF_DEFINE_MEMBERS(Mx2dCustomPolyArea, Mx2dCustomAnnotation,
	McDb::kDHL_CURRENT, McDb::kMReleaseCurrent,
	McDbProxyEntity::kAllAllowedBits, PolyAreaMark,
	PolyArea Mark Entity);

Mx2dCustomPolyArea::Mx2dCustomPolyArea(void)
{
	setType("polyArea");
}

Mx2dCustomPolyArea::Mx2dCustomPolyArea(const McGePoint3dArray& polyPoints, const McGePoint3d& textPos, double textHeight)
	:Mx2dCustomAnnotation(textHeight), m_polyPoints(polyPoints), m_textPos(textPos)
{
	setType("polyArea");
}

Mx2dCustomPolyArea::~Mx2dCustomPolyArea(void)
{
}

Mdesk::Boolean Mx2dCustomPolyArea::worldDraw(McGiWorldDraw* wd)
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

Mcad::ErrorStatus Mx2dCustomPolyArea::getGripPoints(McGePoint3dArray& gripPoints, McGeIntArray& osnapModes, McGeIntArray& geomIds) const
{
	assertReadEnabled();

	for (int i = 0; i < m_polyPoints.length(); ++i) {
		gripPoints.append(m_polyPoints[i]);
	}
	gripPoints.append(m_textPos);

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomPolyArea::moveGripPointsAt(const McGeIntArray& indices, const McGeVector3d& offset)
{
	assertWriteEnabled();
	int iIndex = indices[0];

	if (iIndex == m_polyPoints.length()) {
		m_textPos = m_textPos + offset;
	}
	else {
		m_polyPoints[iIndex] = m_polyPoints[iIndex] + offset;
	}

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomPolyArea::getGeomExtents(McDbExtents& extents, bool roughCalculation) const
{
	assertReadEnabled();

	McDbExtents ext;
	McDbPolyline* pPoly = createPoly();
	pPoly->getGeomExtents(ext, roughCalculation);
	double area = getPolyArea(pPoly);
	double perimeter = getPerimeter(pPoly);
	extents.addExt(ext);
	delete pPoly;

	McDbText* pText = createText(area, perimeter);
	pText->getGeomExtents(ext, roughCalculation);
	extents.addExt(ext);
	delete pText;

	return Mcad::eOk;
}

#define POLYAREA_VERSION 1


Mcad::ErrorStatus Mx2dCustomPolyArea::dwgInFields(McDbDwgFiler* pFiler)
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
	// read number of points
	int ptCount = 0;
	pFiler->readInt(&ptCount);

	// read points
	m_polyPoints.clear();
	for (int i = 0; i < ptCount; ++i) {
		McGePoint3d pt;
		pFiler->readPoint3d(&pt);
		m_polyPoints.append(pt);
	}
	// read text position
	pFiler->readPoint3d(&m_textPos);


	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomPolyArea::dwgOutFields(McDbDwgFiler* pFiler) const
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
	pFiler->writeInt(POLYAREA_VERSION);

	// write number of points
	pFiler->writeInt(m_polyPoints.length());

	// write points
	for (int i = 0; i < m_polyPoints.length(); ++i) {
		pFiler->writePoint3d(m_polyPoints[i]);
	}
	// write text position
	pFiler->writePoint3d(m_textPos);

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomPolyArea::explode(McDbVoidPtrArray& entitySet) const
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

Mcad::ErrorStatus Mx2dCustomPolyArea::transformBy(const McGeMatrix3d& xform)
{
	assertWriteEnabled();

	for (int i = 0; i < m_polyPoints.length(); ++i) {
		McGePoint3d& pt = m_polyPoints[i];
		pt.transformBy(xform);
	}
	m_textPos.transformBy(xform);

	return Mcad::eOk;
}

void Mx2dCustomPolyArea::fromJson(const QJsonObject& jsonObject)
{
	assertWriteEnabled();
	Mx2dCustomAnnotation::fromJson(jsonObject);
	McGePoint3dArray points;
	QJsonArray pointsArray = jsonObject["points"].toArray();
	for (auto point : pointsArray)
	{
		QJsonArray pointArray = point.toArray();
		points.append(Mx2d::jsonArray2dToPoint3d(pointArray));
	}
	m_polyPoints = points;
	m_textPos = Mx2d::jsonArray2dToPoint3d(jsonObject["textPos"].toArray());
}

QJsonObject Mx2dCustomPolyArea::toJson() const
{
	assertReadEnabled();
	QJsonObject jsonObject = Mx2dCustomAnnotation::toJson();
	QJsonArray pointsArray;
	for (int i = 0; i < m_polyPoints.length(); i++)
	{
		QJsonArray pointArray;
		pointArray.append(m_polyPoints[i].x);
		pointArray.append(m_polyPoints[i].y);
		pointsArray.append(pointArray);
	}
	jsonObject["points"] = pointsArray;
	jsonObject["textPos"] = Mx2d::point3dToJsonArray2d(m_textPos);

	return jsonObject;
}

Mx2d::TextInfoList Mx2dCustomPolyArea::findText(const QString& text, bool isExactMatch) const
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

void Mx2dCustomPolyArea::setPoints(const McGePoint3dArray& points)
{
	assertWriteEnabled();
	m_polyPoints = points;
}
McGePoint3dArray Mx2dCustomPolyArea::points() const
{
	assertReadEnabled();
	return m_polyPoints;
}

void Mx2dCustomPolyArea::setTextPos(const McGePoint3d& pos)
{
	assertWriteEnabled();
	m_textPos = pos;
}

McGePoint3d Mx2dCustomPolyArea::textPos() const
{
	assertReadEnabled();
	return m_textPos;
}


McDbPolyline* Mx2dCustomPolyArea::createPoly() const
{
	McDbPolyline* pPoly = new McDbPolyline();
	for (int i = 0; i < m_polyPoints.length(); i++)
	{
		pPoly->addVertexAt(m_polyPoints[i]);
	}
	pPoly->setClosed(Mdesk::kTrue);
	return pPoly;
}

McDbText* Mx2dCustomPolyArea::createText(double area, double perimeter) const
{
	McDbObjectId textStyle = Mx::mcdbCurDwg()->textstyle();

	McDbText* pText = new McDbText();
	pText->setAlignmentPoint(m_textPos);
	pText->setHeight(textHeight());

	QString areaStr = QCoreApplication::translate("Mx2dCustomPolyArea", "Area:%1, Perimeter:%2").arg(QString::number(area * dimRatio() * dimRatio())).arg(QString::number(perimeter * dimRatio()));

	pText->setTextString(areaStr.toLocal8Bit().constData());
	pText->setTextStyle(textStyle);

	return pText;
}

double Mx2dCustomPolyArea::getPolyArea(McDbPolyline* pPoly) const
{
	if (pPoly != nullptr) {
		double area = 0.0;
		pPoly->getArea(area);
		return area;
	}
	return 0.0;
}

double Mx2dCustomPolyArea::getPerimeter(McDbPolyline* pPoly) const
{
	if (pPoly != nullptr) {
		double perimeter = 0.0;
		McGePoint3d lastPt = m_polyPoints[m_polyPoints.length() - 1];
		double lastParameter = 0.0;
		pPoly->getEndParam(lastParameter);
		pPoly->getDistAtParam(lastParameter, perimeter);

		McGePoint3d firstPt = m_polyPoints[0];
		perimeter += firstPt.distanceTo(lastPt);


		return perimeter;
	}
	return 0.0;
}



