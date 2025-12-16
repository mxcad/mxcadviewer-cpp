/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dCustomBatchMeasurement.h"
#include "Mx2dAnnotationFactory.h"

REGISTER_ANNOTATION("batchMeasurement", Mx2dCustomBatchMeasurement);
ACRX_DXF_DEFINE_MEMBERS(Mx2dCustomBatchMeasurement, Mx2dCustomAnnotation,
	McDb::kDHL_CURRENT, McDb::kMReleaseCurrent,
	McDbProxyEntity::kAllAllowedBits, BatchMeasurementMark,
	BatchMeasurement Mark Entity);

Mx2dCustomBatchMeasurement::Mx2dCustomBatchMeasurement(void)
	:m_isDynamicDrawing(false)
{
	setType("batchMeasurement");
}

Mx2dCustomBatchMeasurement::Mx2dCustomBatchMeasurement(const Mx2d::CurveShapeList& curveShapes, const McGePoint3d& dimPt, double textHeight, bool isDynamicDrawing)
	:Mx2dCustomAnnotation(textHeight), m_curveShapes(curveShapes), m_dimPt(dimPt), m_isDynamicDrawing(isDynamicDrawing)
{
	setType("batchMeasurement");
}

Mx2dCustomBatchMeasurement::~Mx2dCustomBatchMeasurement(void)
{
}

Mdesk::Boolean Mx2dCustomBatchMeasurement::worldDraw(McGiWorldDraw* wd)
{
	for (int i = 0; i < m_curveShapes.length(); i++)
	{
		McDbEntity* pEntity = m_curveShapes[i]->createEntity();
		pEntity->worldDraw(wd);
		delete pEntity;
	}
	if (!m_isDynamicDrawing)
	{
		McDbText* pText = createText();
		pText->worldDraw(wd);
		delete pText;
	}
	return Mdesk::kTrue;
}

Mcad::ErrorStatus Mx2dCustomBatchMeasurement::getGripPoints(McGePoint3dArray& gripPoints, McGeIntArray& osnapModes, McGeIntArray& geomIds) const
{
	assertReadEnabled();
	gripPoints.append(m_dimPt);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomBatchMeasurement::moveGripPointsAt(const McGeIntArray& indices, const McGeVector3d& offset)
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

Mcad::ErrorStatus Mx2dCustomBatchMeasurement::getGeomExtents(McDbExtents& extents, bool roughCalculation) const
{
	assertReadEnabled();
	McDbExtents ext;
	for (int i = 0; i < m_curveShapes.length(); i++)
	{
		McDbEntity* pEntity = m_curveShapes[i]->createEntity();
		pEntity->getGeomExtents(ext, roughCalculation);
		extents.addExt(ext);
		delete pEntity;
	}
	if (!m_isDynamicDrawing)
	{
		McDbText* pText = createText();
		pText->getGeomExtents(ext, roughCalculation);
		extents.addExt(ext);
		delete pText;
	}

	return Mcad::eOk;
}

#define BATCHMEASUREMENT_VERSION 1

Mcad::ErrorStatus Mx2dCustomBatchMeasurement::dwgInFields(McDbDwgFiler* pFiler)
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
	pFiler->readPoint3d(&m_dimPt);
	pFiler->readBool(&m_isDynamicDrawing);
	int numShapes = 0;
	pFiler->readInt(&numShapes);
	for (int i = 0; i < numShapes; i++)
	{
		QString typeName;
		MxString mxStr;
		pFiler->readString(mxStr);
		typeName = QString::fromLocal8Bit(mxStr.c_str());
		std::shared_ptr<Mx2d::CurveShape> shape;
		if (typeName == "LineShape")
		{
			shape = std::make_shared<Mx2d::LineShape>();
		}
		else if (typeName == "ArcShape")
		{
			shape = std::make_shared<Mx2d::ArcShape>();
		}
		else if (typeName == "CircleShape")
		{
			shape = std::make_shared<Mx2d::CircleShape>();
		}
		else if (typeName == "EllipseShape")
		{
			shape = std::make_shared<Mx2d::EllipseShape>();
		}
		if (shape) {
			shape->dwgInFields(pFiler);
			m_curveShapes.append(shape);
		}
	}

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomBatchMeasurement::dwgOutFields(McDbDwgFiler* pFiler) const
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
	pFiler->writeInt(BATCHMEASUREMENT_VERSION);
	pFiler->writePoint3d(m_dimPt);
	pFiler->writeBool(m_isDynamicDrawing);
	// write number of shapes
	int numShapes = m_curveShapes.length();
	pFiler->writeInt(numShapes);
	for (int i = 0; i < numShapes; i++)
	{
		QString typeName = m_curveShapes[i]->typeName();
		pFiler->writeString(typeName.toLocal8Bit().constData());
		m_curveShapes[i]->dwgOutFields(pFiler);
	}

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomBatchMeasurement::explode(McDbVoidPtrArray& entitySet) const
{
	for (int i = 0; i < m_curveShapes.length(); i++)
	{
		McDbEntity* pEntity = m_curveShapes[i]->createEntity();
		entitySet.append(pEntity);
	}
	McDbText* pText = createText();
	entitySet.append(pText);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomBatchMeasurement::transformBy(const McGeMatrix3d& xform)
{
	assertWriteEnabled();
	m_dimPt.transformBy(xform);
	for (int i = 0; i < m_curveShapes.length(); i++)
	{
		m_curveShapes[i]->transformBy(xform);
	}
	return Mcad::eOk;
}

void Mx2dCustomBatchMeasurement::fromJson(const QJsonObject& jsonObject)
{
	assertWriteEnabled();
	Mx2dCustomAnnotation::fromJson(jsonObject);
	Mx2d::CurveShapeList curveShapes;
	QJsonArray shapesArray = jsonObject["shapes"].toArray();
	for (int i = 0; i < shapesArray.size(); i++)
	{
		QJsonObject shapeObject = shapesArray[i].toObject();
		QString typeName = shapeObject["typeName"].toString();
		std::shared_ptr<Mx2d::CurveShape> shape;
		if (typeName == "LineShape")
		{
			shape = std::make_shared<Mx2d::LineShape>();
		}
		else if (typeName == "ArcShape")
		{
			shape = std::make_shared<Mx2d::ArcShape>();
		}
		else if (typeName == "CircleShape")
		{
			shape = std::make_shared<Mx2d::CircleShape>();
		}
		else if (typeName == "EllipseShape")
		{
			shape = std::make_shared<Mx2d::EllipseShape>();
		}
		if (shape) {
			shape->fromJson(shapeObject);
			curveShapes.append(shape);
		}
	}
	m_curveShapes = curveShapes;
	m_dimPt = Mx2d::jsonArray2dToPoint3d(jsonObject["dimPoint"].toArray());
	m_isDynamicDrawing = jsonObject["isDynamicDrawing"].toBool();
}

QJsonObject Mx2dCustomBatchMeasurement::toJson() const
{
	assertReadEnabled();
	QJsonObject jsonObject = Mx2dCustomAnnotation::toJson();
	QJsonArray shapesArray;
	for (auto shape : m_curveShapes)
	{
		shapesArray.append(shape->toJson());
	}
	jsonObject["shapes"] = shapesArray;
	jsonObject["dimPoint"] = Mx2d::point3dToJsonArray2d(m_dimPt);
	jsonObject["isDynamicDrawing"] = m_isDynamicDrawing;
	return jsonObject;
}

void Mx2dCustomBatchMeasurement::setCurveShapes(const Mx2d::CurveShapeList& curveShapes)
{
	assertWriteEnabled();
	m_curveShapes = curveShapes;
}

Mx2d::CurveShapeList Mx2dCustomBatchMeasurement::curveShapes() const
{
	assertReadEnabled();
	return m_curveShapes;
}

void Mx2dCustomBatchMeasurement::setDimPt(const McGePoint3d& dimPt)
{
	assertWriteEnabled();
	m_dimPt = dimPt;
}

McGePoint3d Mx2dCustomBatchMeasurement::dimPt() const
{
	assertReadEnabled();
	return m_dimPt;
}

void Mx2dCustomBatchMeasurement::setIsDynamicDrawing(bool isDynamicDrawing)
{
	assertWriteEnabled();
	m_isDynamicDrawing = isDynamicDrawing;
}

bool Mx2dCustomBatchMeasurement::isDynamicDrawing() const
{
	assertReadEnabled();
	return m_isDynamicDrawing;
}

McDbText* Mx2dCustomBatchMeasurement::createText() const
{
	double sum = 0.0;
	for (int i = 0; i < m_curveShapes.length(); i++)
	{
		double length = m_curveShapes[i]->getLength();
		sum += length;
	}
	McDbObjectId textStyle = Mx::mcdbCurDwg()->textstyle();
	McDbText* pText = new McDbText();
	pText->setHeight(textHeight());
	pText->setTextStyle(textStyle);

	QString str = QString::number(sum * dimRatio(), 'f', 0);
	pText->setTextString(str.toLocal8Bit().constData());

	McDbExtents extents;
	pText->getGeomExtents(extents, false);
	McGePoint3d minPt = extents.minPoint();
	McGePoint3d maxPt = extents.maxPoint();
	McGePoint3d basePt((minPt.x + maxPt.x) / 2, (minPt.y + maxPt.y) / 2, 0);
	pText->transformBy(McGeMatrix3d::translation(m_dimPt - basePt));

	return pText;
}

