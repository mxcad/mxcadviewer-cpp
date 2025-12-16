/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dCustomHatchArea2.h"
#include <QCoreApplication>
#include "Mx2dAnnotationFactory.h"

REGISTER_ANNOTATION("hatchArea2", Mx2dCustomHatchArea2);

ACRX_DXF_DEFINE_MEMBERS(Mx2dCustomHatchArea2, Mx2dCustomAnnotation,
	McDb::kDHL_CURRENT, McDb::kMReleaseCurrent,
	McDbProxyEntity::kAllAllowedBits, HatchArea2Mark,
	HatchArea2 Mark Entity);

Mx2dCustomHatchArea2::Mx2dCustomHatchArea2(void)
	:m_isDynamicDrawing(false)
{
	setType("hatchArea2");
}

Mx2dCustomHatchArea2::Mx2dCustomHatchArea2(const Mx2d::HatchPLList& polys, const McGePoint3d& textPos, double textHeight, bool isDynamicDrawing)
	:Mx2dCustomAnnotation(textHeight), m_polys(polys), m_textPos(textPos), m_isDynamicDrawing(isDynamicDrawing)
{
	setType("hatchArea2");
}

Mx2dCustomHatchArea2::~Mx2dCustomHatchArea2(void)
{
}

Mdesk::Boolean Mx2dCustomHatchArea2::worldDraw(McGiWorldDraw* wd)
{

	McDbVoidPtrArray arr = createPolys();
	for (int i = 0; i < arr.length(); ++i) {
		McDbEntity* pEntity = (McDbEntity*)arr[i];
		pEntity->worldDraw(wd);
		delete pEntity;
	}


	if (!m_isDynamicDrawing)
	{
		double area = getPolyArea();
		double perimeter = getPerimeter();
		McDbText* pText = createText(area, perimeter);
		pText->worldDraw(wd);
		delete pText;
	}

	return Mdesk::kTrue;
}

Mcad::ErrorStatus Mx2dCustomHatchArea2::getGripPoints(McGePoint3dArray& gripPoints, McGeIntArray& osnapModes, McGeIntArray& geomIds) const
{
	assertReadEnabled();


	gripPoints.append(m_textPos);

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomHatchArea2::moveGripPointsAt(const McGeIntArray& indices, const McGeVector3d& offset)
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

Mcad::ErrorStatus Mx2dCustomHatchArea2::getGeomExtents(McDbExtents& extents, bool roughCalculation) const
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

#define HATCHAREA2_VERSION 1


Mcad::ErrorStatus Mx2dCustomHatchArea2::dwgInFields(McDbDwgFiler* pFiler)
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
	// read polys length
	int polysSize = 0;
	pFiler->readInt(&polysSize);


	m_polys.clear();
	// read polys
	for (int i = 0; i < polysSize; ++i)
	{
		Mx2d::PLVertexList vertices;
		int polyVerticesSize = 0;
		pFiler->readInt(&polyVerticesSize);
		for (int j = 0; j < polyVerticesSize; j++)
		{
			McGePoint3d pt;
			double bulge;
			pFiler->readPoint3d(&pt);
			pFiler->readDouble(&bulge);
			vertices.append({ pt, bulge });
		}
		double length = 0.0;
		double area = 0.0;
		bool isExternal = false;
		pFiler->readDouble(&length);
		pFiler->readDouble(&area);
		pFiler->readBool(&isExternal);

		m_polys.append({ vertices, length, area, isExternal });

	}
	// read text position
	pFiler->readPoint3d(&m_textPos);
	pFiler->readBool(&m_isDynamicDrawing);


	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomHatchArea2::dwgOutFields(McDbDwgFiler* pFiler) const
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
	pFiler->writeInt(HATCHAREA2_VERSION);

	// write polys size
	pFiler->writeInt(m_polys.length());

	// write polys

	for (int i = 0; i < m_polys.length(); ++i) {
		Mx2d::PLVertexListWithMetricsAndExtFlag poly = m_polys[i];
		pFiler->writeInt(poly.vertices.length());
		for (int j = 0; j < poly.vertices.length(); j++)
		{
			McGePoint3d pt = poly.vertices[j].pt;
			double bulge = poly.vertices[j].bulge;
			pFiler->writePoint3d(pt);
			pFiler->writeDouble(bulge);
		}
		pFiler->writeDouble(poly.length);
		pFiler->writeDouble(poly.area);
		pFiler->writeBool(poly.isExternal);
	}
	// write text position
	pFiler->writePoint3d(m_textPos);
	pFiler->writeBool(m_isDynamicDrawing);


	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomHatchArea2::explode(McDbVoidPtrArray& entitySet) const
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

Mcad::ErrorStatus Mx2dCustomHatchArea2::transformBy(const McGeMatrix3d& xform)
{
	assertWriteEnabled();

	for (int i = 0; i < m_polys.length(); ++i) {
		Mx2d::PLVertexListWithMetricsAndExtFlag& poly = m_polys[i];
		for (int j = 0; j < poly.vertices.length(); j++)
		{
			McGePoint3d& pt = poly.vertices[j].pt;
			pt.transformBy(xform);
		}
	}
	m_textPos.transformBy(xform);

	return Mcad::eOk;
}

void Mx2dCustomHatchArea2::fromJson(const QJsonObject& jsonObject)
{
	assertWriteEnabled();
	Mx2dCustomAnnotation::fromJson(jsonObject);
	Mx2d::HatchPLList polys;
	QJsonArray polysArray = jsonObject["polys"].toArray();
	for (auto poly_ : polysArray)
	{
		QJsonObject polyObj = poly_.toObject();
		Mx2d::PLVertexListWithMetricsAndExtFlag poly;
		poly.length = polyObj["length"].toDouble();
		poly.area = polyObj["area"].toDouble();
		poly.isExternal = polyObj["isExternal"].toBool();
		QJsonArray verticesArray = polyObj["vertices"].toArray();
		for (auto vertex : verticesArray)
		{
			QJsonArray vertexArray = vertex.toArray();
			McGePoint3d pt = Mx2d::jsonArray2dToPoint3d(vertexArray);
			poly.vertices.append({ pt, vertexArray[2].toDouble() });
		}
		polys.append(poly);
	}
	m_polys = polys;
	m_textPos = Mx2d::jsonArray2dToPoint3d(jsonObject["textPos"].toArray());
	m_isDynamicDrawing = jsonObject["isDynamicDrawing"].toBool();
}

QJsonObject Mx2dCustomHatchArea2::toJson() const
{
	assertReadEnabled();
	QJsonObject jsonObject = Mx2dCustomAnnotation::toJson();
	QJsonArray polysArray;
	for (auto poly : m_polys)
	{
		QJsonObject polyObj;
		polyObj["length"] = poly.length;
		polyObj["area"] = poly.area;
		polyObj["isExternal"] = poly.isExternal;
		QJsonArray verticesArray;
		for (auto vertex : poly.vertices)
		{
			QJsonArray vertexArray;
			vertexArray.append(vertex.pt.x);
			vertexArray.append(vertex.pt.y);
			vertexArray.append(vertex.bulge);
			verticesArray.append(vertexArray);
		}
		polyObj["vertices"] = verticesArray;
		polysArray.append(polyObj);
	}
	jsonObject["polys"] = polysArray;
	jsonObject["textPos"] = Mx2d::point3dToJsonArray2d(m_textPos);
	jsonObject["isDynamicDrawing"] = m_isDynamicDrawing;

	return jsonObject;
}

void Mx2dCustomHatchArea2::setPolys(const Mx2d::HatchPLList& polys)
{
	assertWriteEnabled();
	m_polys = polys;
}

Mx2d::HatchPLList Mx2dCustomHatchArea2::polys() const
{
	assertReadEnabled();
	return m_polys;
}

void Mx2dCustomHatchArea2::setTextPos(const McGePoint3d& textPos)
{
	assertWriteEnabled();
	m_textPos = textPos;
}

McGePoint3d Mx2dCustomHatchArea2::textPos() const
{
	assertReadEnabled();
	return m_textPos;
}

void Mx2dCustomHatchArea2::setIsDynamicDrawing(bool isDynamicDrawing)
{
	assertWriteEnabled();
	m_isDynamicDrawing = isDynamicDrawing;
}

bool Mx2dCustomHatchArea2::isDynamicDrawing() const
{
	assertReadEnabled();
	return m_isDynamicDrawing;
}



McDbVoidPtrArray Mx2dCustomHatchArea2::createPolys() const
{
	McDbVoidPtrArray arr;
	for (int i = 0; i < m_polys.length(); ++i) {
		Mx2d::PLVertexListWithMetricsAndExtFlag poly = m_polys[i];
		McDbPolyline* pPoly = new McDbPolyline();
		pPoly->setClosed(true);
		for (int j = 0; j < poly.vertices.length(); j++)
		{
			McGePoint3d pt = poly.vertices[j].pt;
			double bulge = poly.vertices[j].bulge;
			pPoly->addVertexAt(pt, bulge);
		}
		arr.append(pPoly);

	}
	return arr;
}

McDbText* Mx2dCustomHatchArea2::createText(double area, double perimeter) const
{
	// 当前文字样式
	McDbObjectId textStyle = Mx::mcdbCurDwg()->textstyle();

	McDbText* pText = new McDbText();
	pText->setAlignmentPoint(m_textPos);
	pText->setHeight(textHeight());

	QString areaStr = QCoreApplication::translate("Mx2dCustomHatchArea2", "Area:%1, Perimeter:%2").arg(QString::number(area * dimRatio() * dimRatio())).arg(QString::number(perimeter * dimRatio()));

	pText->setTextString(areaStr.toLocal8Bit().constData());
	pText->setTextStyle(textStyle);

	return pText;
}

double Mx2dCustomHatchArea2::getPolyArea() const
{
	double areaSum = 0.0;
	for (int i = 0; i < m_polys.length(); ++i) {
		Mx2d::PLVertexListWithMetricsAndExtFlag poly = m_polys[i];
		if (poly.isExternal)
		{
			areaSum += poly.area;
		}
		else
		{
			areaSum -= poly.area;
		}
	}
	return areaSum;
}

double Mx2dCustomHatchArea2::getPerimeter() const
{
	double perimeterSum = 0.0;
	for (int i = 0; i < m_polys.length(); ++i) {
		Mx2d::PLVertexListWithMetricsAndExtFlag poly = m_polys[i];
		perimeterSum += poly.length;

	}
	return perimeterSum;
}




