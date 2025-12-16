/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dCustomHatchArea.h"
#include <QCoreApplication>


ACRX_DXF_DEFINE_MEMBERS(Mx2dCustomHatchArea, Mx2dCustomAnnotation,
	McDb::kDHL_CURRENT, McDb::kMReleaseCurrent,
	McDbProxyEntity::kAllAllowedBits, HatchAreaMark,
	HatchArea Mark Entity);

Mx2dCustomHatchArea::Mx2dCustomHatchArea(void)
{
}

Mx2dCustomHatchArea::Mx2dCustomHatchArea(const Mx2d::HatchRegionList& regions, const McGePoint3d& textPos, double textHeight)
	:Mx2dCustomAnnotation(textHeight), m_regions(regions), m_textPos(textPos)
{
}

Mx2dCustomHatchArea::~Mx2dCustomHatchArea(void)
{
}

Mdesk::Boolean Mx2dCustomHatchArea::worldDraw(McGiWorldDraw* wd)
{

	McDbVoidPtrArray arr = createPolys();
	for (int i = 0; i < arr.length(); ++i) {
		McDbEntity* pEntity = (McDbEntity*)arr[i];
		pEntity->worldDraw(wd);
		delete pEntity;
	}
	double area = getPolyArea();
	double perimeter = getPerimeter();

	McDbText* pText = createText(area, perimeter);
	pText->worldDraw(wd);
	delete pText;

	return Mdesk::kTrue;
}

Mcad::ErrorStatus Mx2dCustomHatchArea::getGripPoints(McGePoint3dArray& gripPoints, McGeIntArray& osnapModes, McGeIntArray& geomIds) const
{
	assertReadEnabled();


	gripPoints.append(m_textPos);

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomHatchArea::moveGripPointsAt(const McGeIntArray& indices, const McGeVector3d& offset)
{
	assertWriteEnabled();
	int iIndex = indices[0];

	switch (iIndex)
	{
	case 0:
		m_textPos = m_textPos + offset;
		break;
	}
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomHatchArea::getGeomExtents(McDbExtents& extents, bool roughCalculation) const
{
	assertReadEnabled();

	McDbExtents polyExtents, textExtents;

	McDbVoidPtrArray arr = createPolys();
	for (int i = 0; i < arr.length(); ++i) {
		McDbEntity* pEntity = (McDbEntity*)arr[i];
		McDbExtents ext;
		pEntity->getGeomExtents(ext, roughCalculation);
		polyExtents.addExt(ext);
		delete pEntity;
	}

	double area = getPolyArea();
	double perimeter = getPerimeter();

	McDbText* pText = createText(area, perimeter);
	pText->getGeomExtents(textExtents, roughCalculation);
	delete pText;

	extents.addExt(polyExtents);
	extents.addExt(textExtents);

	return Mcad::eOk;
}

#define HATCHAREA_VERSION 1


Mcad::ErrorStatus Mx2dCustomHatchArea::dwgInFields(McDbDwgFiler* pFiler)
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

	int regionsSize = 0;
	pFiler->readInt(&regionsSize);


	m_regions.clear();

	for (int i = 0; i < regionsSize; ++i)
	{
		Mx2d::PLVertexListWithMetrics inner;
		Mx2d::PLVertexListWithMetrics outer;

		int innerVerticesSize = 0;
		pFiler->readInt(&innerVerticesSize);
		for (int j = 0; j < innerVerticesSize; j++)
		{
			McGePoint3d pt;
			double bulge;
			pFiler->readPoint3d(&pt);
			pFiler->readDouble(&bulge);
			inner.vertices.append({ pt, bulge });
		}
		double length = 0.0;
		double area = 0.0;
		pFiler->readDouble(&length);
		pFiler->readDouble(&area);
		inner.length = length;
		inner.area = area;

		int outerVerticesSize = 0;
		pFiler->readInt(&outerVerticesSize);
		for (int j = 0; j < outerVerticesSize; j++)
		{
			McGePoint3d pt;
			double bulge;
			pFiler->readPoint3d(&pt);
			pFiler->readDouble(&bulge);
			outer.vertices.append({ pt, bulge });
		}
		pFiler->readDouble(&length);
		pFiler->readDouble(&area);
		outer.length = length;
		outer.area = area;

		m_regions.append({ inner, outer });

	}

	pFiler->readPoint3d(&m_textPos);

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomHatchArea::dwgOutFields(McDbDwgFiler* pFiler) const
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
	pFiler->writeInt(HATCHAREA_VERSION);


	pFiler->writeInt(m_regions.length());



	for (int i = 0; i < m_regions.length(); ++i) {
		Mx2d::HatchRegion region = m_regions[i];
		Mx2d::PLVertexListWithMetrics inner = region.inner;
		Mx2d::PLVertexListWithMetrics outer = region.outer;

		pFiler->writeInt(inner.vertices.length());
		for (int j = 0; j < inner.vertices.length(); j++)
		{
			McGePoint3d pt = inner.vertices[j].pt;
			double bulge = inner.vertices[j].bulge;
			pFiler->writePoint3d(pt);
			pFiler->writeDouble(bulge);
		}
		pFiler->writeDouble(inner.length);
		pFiler->writeDouble(inner.area);

		pFiler->writeInt(outer.vertices.length());
		for (int j = 0; j < outer.vertices.length(); j++)
		{
			McGePoint3d pt = outer.vertices[j].pt;
			double bulge = outer.vertices[j].bulge;
			pFiler->writePoint3d(pt);
			pFiler->writeDouble(bulge);
		}
		pFiler->writeDouble(outer.length);
		pFiler->writeDouble(outer.area);
	}

	pFiler->writePoint3d(m_textPos);


	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomHatchArea::explode(McDbVoidPtrArray& entitySet) const
{
	assertReadEnabled();
	McDbVoidPtrArray arr = createPolys();
	entitySet.append(arr);

	double area = getPolyArea();
	double perimeter = getPerimeter();

	McDbText* pText = createText(area, perimeter);
	entitySet.append(pText);

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomHatchArea::transformBy(const McGeMatrix3d& xform)
{
	assertWriteEnabled();

	for (int i = 0; i < m_regions.length(); ++i) {
		Mx2d::HatchRegion& region = m_regions[i];
		Mx2d::PLVertexListWithMetrics& inner = region.inner;
		Mx2d::PLVertexListWithMetrics& outer = region.outer;
		for (int j = 0; j < inner.vertices.length(); j++)
		{
			McGePoint3d& pt = inner.vertices[j].pt;
			pt.transformBy(xform);
		}
		for (int j = 0; j < outer.vertices.length(); j++)
		{
			McGePoint3d& pt = outer.vertices[j].pt;
			pt.transformBy(xform);
		}
	}
	m_textPos.transformBy(xform);

	return Mcad::eOk;
}

void Mx2dCustomHatchArea::fromJson(const QJsonObject& jsonObject)
{
}

QJsonObject Mx2dCustomHatchArea::toJson() const
{
	return QJsonObject();
}



McDbVoidPtrArray Mx2dCustomHatchArea::createPolys() const
{


	McDbVoidPtrArray arr;
	for (int i = 0; i < m_regions.length(); ++i) {
		Mx2d::HatchRegion region = m_regions[i];
		Mx2d::PLVertexListWithMetrics inner = region.inner;
		Mx2d::PLVertexListWithMetrics outer = region.outer;
		if (inner.vertices.length() > 0)
		{
			McDbPolyline* pPoly = new McDbPolyline();
			pPoly->setClosed(true);
			for (int j = 0; j < inner.vertices.length(); j++)
			{
				McGePoint3d pt = inner.vertices[j].pt;
				double bulge = inner.vertices[j].bulge;
				pPoly->addVertexAt(pt, bulge);
			}
			arr.append(pPoly);
		}

		if (outer.vertices.length() > 0)
		{
			McDbPolyline* pPoly = new McDbPolyline();
			pPoly->setClosed(true);
			for (int j = 0; j < outer.vertices.length(); j++)
			{
				McGePoint3d pt = outer.vertices[j].pt;
				double bulge = outer.vertices[j].bulge;
				pPoly->addVertexAt(pt, bulge);
			}
			arr.append(pPoly);
		}


	}
	return arr;
}

McDbText* Mx2dCustomHatchArea::createText(double area, double perimeter) const
{
	McDbObjectId textStyle = Mx::mcdbCurDwg()->textstyle();

	McDbText* pText = new McDbText();
	pText->setAlignmentPoint(m_textPos);
	pText->setHeight(textHeight());

	QString areaStr = QCoreApplication::translate("Mx2dCustomHatchArea", "Area:%1, Perimeter:%2").arg(QString::number(area * m_dimRatio * m_dimRatio)).arg(QString::number(perimeter * m_dimRatio));

	pText->setTextString(areaStr.toLocal8Bit().constData());
	pText->setTextStyle(textStyle);

	return pText;
}

double Mx2dCustomHatchArea::getPolyArea() const
{
	double areaSum = 0.0;
	for (int i = 0; i < m_regions.length(); ++i) {
		Mx2d::HatchRegion region = m_regions[i];
		Mx2d::PLVertexListWithMetrics inner = region.inner;
		Mx2d::PLVertexListWithMetrics outer = region.outer;
		double area = outer.area - inner.area;
		areaSum += area;

	}
	return areaSum;
}

double Mx2dCustomHatchArea::getPerimeter() const
{
	double perimeterSum = 0.0;
	for (int i = 0; i < m_regions.length(); ++i) {
		Mx2d::HatchRegion region = m_regions[i];
		Mx2d::PLVertexListWithMetrics inner = region.inner;
		Mx2d::PLVertexListWithMetrics outer = region.outer;
		double perimeter = outer.length + inner.length;
		perimeterSum += perimeter;

	}
	return perimeterSum;
}




