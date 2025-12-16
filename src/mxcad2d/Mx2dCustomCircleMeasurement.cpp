/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dCustomCircleMeasurement.h"
#include <QString>
#include "Mx2dUtils.h"
#include <QCoreApplication>
#include "Mx2dAnnotationFactory.h"

REGISTER_ANNOTATION("circleMeasurement", Mx2dCustomCircleMeasurement);
ACRX_DXF_DEFINE_MEMBERS(Mx2dCustomCircleMeasurement, Mx2dCustomAnnotation,
	McDb::kDHL_CURRENT, McDb::kMReleaseCurrent,
	McDbProxyEntity::kAllAllowedBits, CircleMeasurementMark,
	CircleMeasurement Mark Entity);

Mx2dCustomCircleMeasurement::Mx2dCustomCircleMeasurement(void)
{
	setType("circleMeasurement");
}


Mx2dCustomCircleMeasurement::Mx2dCustomCircleMeasurement(const McGePoint3d& centerPt, double radius, const McGePoint3d& dimPt, double textHeight)
	:Mx2dCustomAnnotation(textHeight), m_centerPt(centerPt), m_radius(radius), m_dimPt(dimPt)
{
	setType("circleMeasurement");
}

Mx2dCustomCircleMeasurement::~Mx2dCustomCircleMeasurement(void)
{
}

Mdesk::Boolean Mx2dCustomCircleMeasurement::worldDraw(McGiWorldDraw* wd)
{
	McDbCircle* pCircle = createCircle();
	pCircle->worldDraw(wd);
	delete pCircle;

	McDbVoidPtrArray arr = createTexts();
	for (int i = 0; i < arr.length(); i++)
	{
		McDbText* pText = static_cast<McDbText*>(arr[i]);
		pText->worldDraw(wd);
		delete pText;
	}

	return Mdesk::kTrue;
}

Mcad::ErrorStatus Mx2dCustomCircleMeasurement::getGripPoints(McGePoint3dArray& gripPoints, McGeIntArray& osnapModes, McGeIntArray& geomIds) const
{
	assertReadEnabled();
	gripPoints.append(m_dimPt);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomCircleMeasurement::moveGripPointsAt(const McGeIntArray& indices, const McGeVector3d& offset)
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

Mcad::ErrorStatus Mx2dCustomCircleMeasurement::getGeomExtents(McDbExtents& extents, bool roughCalculation) const
{
	assertReadEnabled();
	McDbExtents ext;
	McDbCircle* pCircle = createCircle();
	pCircle->getGeomExtents(ext, roughCalculation);
	extents.addExt(ext);
	delete pCircle;

	McDbVoidPtrArray arr = createTexts();
	for (int i = 0; i < arr.length(); i++)
	{
		McDbText* pText = static_cast<McDbText*>(arr[i]);
		pText->getGeomExtents(ext, roughCalculation);
		extents.addExt(ext);
		delete pText;
	}
	return Mcad::eOk;
}

#define CIRCLEMEASUREMENT_VERSION 1

Mcad::ErrorStatus Mx2dCustomCircleMeasurement::dwgInFields(McDbDwgFiler* pFiler)
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
	pFiler->readPoint3d(&m_centerPt);
	pFiler->readPoint3d(&m_dimPt);
	pFiler->readDouble(&m_radius);

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomCircleMeasurement::dwgOutFields(McDbDwgFiler* pFiler) const
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
	pFiler->writeInt(CIRCLEMEASUREMENT_VERSION);
	pFiler->writePoint3d(m_centerPt);
	pFiler->writePoint3d(m_dimPt);
	pFiler->writeDouble(m_radius);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomCircleMeasurement::explode(McDbVoidPtrArray& entitySet) const
{
	McDbCircle* pCircle = createCircle();
	McDbVoidPtrArray arr = createTexts();
	entitySet.append(pCircle);
	entitySet.append(arr);

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomCircleMeasurement::transformBy(const McGeMatrix3d& xform)
{
	assertWriteEnabled();

	m_centerPt.transformBy(xform);
	m_dimPt.transformBy(xform);


	return Mcad::eOk;
}

void Mx2dCustomCircleMeasurement::fromJson(const QJsonObject& jsonObject)
{
	assertWriteEnabled();
	Mx2dCustomAnnotation::fromJson(jsonObject);
	m_centerPt = Mx2d::jsonArray2dToPoint3d(jsonObject["centerPoint"].toArray());
	m_dimPt = Mx2d::jsonArray2dToPoint3d(jsonObject["dimPoint"].toArray());
	m_radius = jsonObject["radius"].toDouble();
}

QJsonObject Mx2dCustomCircleMeasurement::toJson() const
{
	assertReadEnabled();
	QJsonObject jsonObject = Mx2dCustomAnnotation::toJson();

	jsonObject["centerPoint"] = Mx2d::point3dToJsonArray2d(m_centerPt);
	jsonObject["dimPoint"] = Mx2d::point3dToJsonArray2d(m_dimPt);
	jsonObject["radius"] = m_radius;

	return jsonObject;
}

void Mx2dCustomCircleMeasurement::setCenterPt(const McGePoint3d& centerPt)
{
	assertWriteEnabled();
	m_centerPt = centerPt;
}

McGePoint3d Mx2dCustomCircleMeasurement::centerPt() const
{
	assertReadEnabled();
	return m_centerPt;
}

void Mx2dCustomCircleMeasurement::setDimPt(const McGePoint3d& dimPt)
{
	assertWriteEnabled();
	m_dimPt = dimPt;
}

McGePoint3d Mx2dCustomCircleMeasurement::dimPt() const
{
	assertReadEnabled();
	return m_dimPt;
}

void Mx2dCustomCircleMeasurement::setRadius(double radius)
{
	assertWriteEnabled();
	m_radius = radius;
}

double Mx2dCustomCircleMeasurement::radius() const
{
	assertReadEnabled();
	return m_radius;
}


McDbCircle* Mx2dCustomCircleMeasurement::createCircle() const
{
	return new McDbCircle(m_centerPt, McGeVector3d::kZAxis, m_radius);
}

McDbVoidPtrArray Mx2dCustomCircleMeasurement::createTexts() const
{
	double offset = textHeight() / 4.0;
	McDbObjectId textStyle = Mx::mcdbCurDwg()->textstyle();

	// radius
	McDbText* pTextRadius = new McDbText();
	pTextRadius->setHeight(textHeight());
	pTextRadius->setTextStyle(textStyle);
	QString num = QString::number(m_radius * dimRatio(), 'f', 2);
	QString str = QCoreApplication::translate("Mx2dCustomCircleMeasurement", "Radius:%1").arg(num);
	pTextRadius->setTextString(str.toLocal8Bit().constData());
	McDbExtents extRadius;
	pTextRadius->getGeomExtents(extRadius, false);
	McGePoint3d mbAnchorPtRadius((extRadius.minPoint().x + extRadius.maxPoint().x) / 2.0, extRadius.minPoint().y - offset, 0.0);

	// circumference
	McDbText* pTextCircumference = new McDbText();
	pTextCircumference->setHeight(textHeight());
	pTextCircumference->setTextStyle(textStyle);
	num = QString::number(2 * M_PI * m_radius * dimRatio(), 'f', 2);
	str = QCoreApplication::translate("Mx2dCustomCircleMeasurement", "Circumference:%1").arg(num);
	pTextCircumference->setTextString(str.toLocal8Bit().constData());
	McDbExtents extCircumference;
	pTextCircumference->getGeomExtents(extCircumference, false);
	double circumferenceMidX = (extCircumference.minPoint().x + extCircumference.maxPoint().x) / 2.0;
	double circumferenceMidY = (extCircumference.minPoint().y + extCircumference.maxPoint().y) / 2.0;
	McGePoint3d mtAnchorPtCircumference(circumferenceMidX, extCircumference.maxPoint().y + offset, 0.0);
	McGePoint3d mmAnchorPtCircumference(circumferenceMidX, circumferenceMidY, 0.0);
	McGePoint3d mbAnchorPtCircumference(circumferenceMidX, extCircumference.minPoint().y - offset, 0.0);

	// area
	McDbText* pTextArea = new McDbText();
	pTextArea->setHeight(textHeight());
	pTextArea->setTextStyle(textStyle);
	num = QString::number(M_PI * m_radius * m_radius * dimRatio() * dimRatio(), 'f', 2);
	str = QCoreApplication::translate("Mx2dCustomCircleMeasurement", "Area:%1").arg(num);
	pTextArea->setTextString(str.toLocal8Bit().constData());
	McDbExtents extArea;
	pTextArea->getGeomExtents(extArea, false);
	McGePoint3d mtAnchorPtArea((extArea.minPoint().x + extArea.maxPoint().x) / 2.0, extArea.maxPoint().y + offset, 0.0);


	McGeMatrix3d circumferenceMoveMat = McGeMatrix3d::translation(m_dimPt - mmAnchorPtCircumference);
	pTextCircumference->transformBy(circumferenceMoveMat);
	mtAnchorPtCircumference.transformBy(circumferenceMoveMat);
	mbAnchorPtCircumference.transformBy(circumferenceMoveMat);


	McGeMatrix3d radiusMoveMat = McGeMatrix3d::translation(mtAnchorPtCircumference - mbAnchorPtRadius);
	pTextRadius->transformBy(radiusMoveMat);

	McGeMatrix3d areaMoveMat = McGeMatrix3d::translation(mbAnchorPtCircumference - mtAnchorPtArea);
	pTextArea->transformBy(areaMoveMat);

	McDbVoidPtrArray arr;
	arr.append(pTextRadius);
	arr.append(pTextCircumference);
	arr.append(pTextArea);

	return arr;
}


