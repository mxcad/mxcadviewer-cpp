/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "CurveShape.h"
#include <QJsonArray>
namespace Mx2d {

	void CurveShape::setTypeName(const QString& typeName)
	{
		m_typeName = typeName;
	}

	QString CurveShape::typeName() const
	{
		return m_typeName;
	}

	// ===============LineShape===============

	LineShape::LineShape()
	{
		setTypeName("LineShape");
	}

	LineShape::LineShape(const McGePoint3d& startPt, const McGePoint3d& endPt)
		:m_startPt(startPt), m_endPt(endPt)
	{
		setTypeName("LineShape");
	}

	double LineShape::getLength() const
	{
		return m_startPt.distanceTo(m_endPt);
	}
	Mcad::ErrorStatus LineShape::dwgInFields(McDbDwgFiler* pFiler)
	{
		pFiler->readPoint3d(&m_startPt);
		pFiler->readPoint3d(&m_endPt);

		return Mcad::eOk;
	}
	Mcad::ErrorStatus LineShape::dwgOutFields(McDbDwgFiler* pFiler) const
	{
		pFiler->writePoint3d(m_startPt);
		pFiler->writePoint3d(m_endPt);
		return Mcad::eOk;
	}
	Mcad::ErrorStatus LineShape::transformBy(const McGeMatrix3d& xform)
	{
		m_startPt.transformBy(xform);
		m_endPt.transformBy(xform);
		return Mcad::eOk;
	}
	McDbEntity* LineShape::createEntity() const
	{
		McDbLine* pLine = new McDbLine(m_startPt, m_endPt);

		return pLine;
	}

	QJsonObject LineShape::toJson() const
	{
		QJsonObject json;
		json["typeName"] = typeName();
		json["startPoint"] = QJsonArray{ m_startPt.x, m_startPt.y };
		json["endPoint"] = QJsonArray{ m_endPt.x, m_endPt.y };
		return json;
	}

	void LineShape::fromJson(const QJsonObject& json)
	{
		m_startPt = McGePoint3d(json["startPoint"][0].toDouble(), json["startPoint"][1].toDouble(), 0);
		m_endPt = McGePoint3d(json["endPoint"][0].toDouble(), json["endPoint"][1].toDouble(), 0);
	}


	// ===============CircleShape===============
	CircleShape::CircleShape()
	{
		setTypeName("CircleShape");
	}

	CircleShape::CircleShape(const McGePoint3d& center, double radius)
		: m_center(center), m_radius(radius)
	{
		setTypeName("CircleShape");
	}

	double CircleShape::getLength() const
	{
		return M_PI * m_radius * m_radius;
	}
	Mcad::ErrorStatus CircleShape::dwgInFields(McDbDwgFiler* pFiler)
	{
		pFiler->readPoint3d(&m_center);
		pFiler->readDouble(&m_radius);
		return Mcad::eOk;
	}
	Mcad::ErrorStatus CircleShape::dwgOutFields(McDbDwgFiler* pFiler) const
	{
		pFiler->writePoint3d(m_center);
		pFiler->writeDouble(m_radius);
		return Mcad::eOk;
	}
	Mcad::ErrorStatus CircleShape::transformBy(const McGeMatrix3d& xform)
	{
		m_center.transformBy(xform);
		return Mcad::eOk;
	}
	McDbEntity* CircleShape::createEntity() const
	{
		McDbCircle* pCircle = new McDbCircle(m_center, McGeVector3d::kZAxis, m_radius);
		return pCircle;
	}

	QJsonObject CircleShape::toJson() const
	{
		QJsonObject json;
		json["typeName"] = typeName();
		json["center"] = QJsonArray{ m_center.x, m_center.y };
		json["radius"] = m_radius;
		return json;
	}

	void CircleShape::fromJson(const QJsonObject& json)
	{
		m_center = McGePoint3d(json["center"][0].toDouble(), json["center"][1].toDouble(), 0);
		m_radius = json["radius"].toDouble();
	}

	// ===============ArcShape===============
	ArcShape::ArcShape()
	{
		setTypeName("ArcShape");
	}

	ArcShape::ArcShape(const McGePoint3d& startPt, const McGePoint3d& midPt, const McGePoint3d& endPt)
		: m_startPt(startPt), m_midPt(midPt), m_endPt(endPt)
	{
		setTypeName("ArcShape");
	}
	double ArcShape::getLength() const
	{
		McDbArc* pArc = McDbArc::cast(createEntity());
		double startAngle = pArc->startAngle();
		double endAngle = pArc->endAngle();
		double sweepAngle = endAngle - startAngle;
		if (sweepAngle < 0.0)
			sweepAngle += 2.0 * M_PI;
		double length = sweepAngle * pArc->radius();
		delete pArc;
		return length;
	}
	Mcad::ErrorStatus ArcShape::dwgInFields(McDbDwgFiler* pFiler)
	{
		pFiler->readPoint3d(&m_startPt);
		pFiler->readPoint3d(&m_midPt);
		pFiler->readPoint3d(&m_endPt);
		return Mcad::eOk;
	}
	Mcad::ErrorStatus ArcShape::dwgOutFields(McDbDwgFiler* pFiler) const
	{
		pFiler->writePoint3d(m_startPt);
		pFiler->writePoint3d(m_midPt);
		pFiler->writePoint3d(m_endPt);
		return Mcad::eOk;
	}
	Mcad::ErrorStatus ArcShape::transformBy(const McGeMatrix3d& xform)
	{
		m_startPt.transformBy(xform);
		m_midPt.transformBy(xform);
		m_endPt.transformBy(xform);
		return Mcad::eOk;
	}
	McDbEntity* ArcShape::createEntity() const
	{
		McDbArc* pArc = new McDbArc();
		pArc->computeArc(m_startPt, m_midPt, m_endPt);
		return pArc;
	}

	QJsonObject ArcShape::toJson() const
	{
		QJsonObject json;
		json["typeName"] = typeName();
		json["startPoint"] = QJsonArray{ m_startPt.x, m_startPt.y };
		json["midPoint"] = QJsonArray{ m_midPt.x, m_midPt.y };
		json["endPoint"] = QJsonArray{ m_endPt.x, m_endPt.y };
		return json;
	}

	void ArcShape::fromJson(const QJsonObject& json)
	{
		m_startPt = McGePoint3d(json["startPoint"][0].toDouble(), json["startPoint"][1].toDouble(), 0);
		m_midPt = McGePoint3d(json["midPoint"][0].toDouble(), json["midPoint"][1].toDouble(), 0);
		m_endPt = McGePoint3d(json["endPoint"][0].toDouble(), json["endPoint"][1].toDouble(), 0);
	}

	// ===============EllipseShape===============
	EllipseShape::EllipseShape()
	{
		setTypeName("EllipseShape");
	}

	EllipseShape::EllipseShape(const McGePoint3d& center, const McGeVector3d& majorAxis, double radiusRatio, double startAngle, double endAngle)
		: m_center(center), m_majorAxis(majorAxis), m_radiusRatio(radiusRatio), m_startAngle(startAngle), m_endAngle(endAngle)
	{
		setTypeName("EllipseShape");
	}
	double EllipseShape::getLength() const
	{
		McDbEllipse* pEllipse = McDbEllipse::cast(createEntity());
		double endParam = 0.0;  pEllipse->getEndParam(endParam);
		double length = 0.0; pEllipse->getDistAtParam(endParam, length);

		return length;
	}
	Mcad::ErrorStatus EllipseShape::dwgInFields(McDbDwgFiler* pFiler)
	{
		pFiler->readPoint3d(&m_center);
		pFiler->readVector3d(&m_majorAxis);
		pFiler->readDouble(&m_radiusRatio);
		pFiler->readDouble(&m_startAngle);
		pFiler->readDouble(&m_endAngle);
		return Mcad::eOk;
	}
	Mcad::ErrorStatus EllipseShape::dwgOutFields(McDbDwgFiler* pFiler) const
	{
		pFiler->writePoint3d(m_center);
		pFiler->writeVector3d(m_majorAxis);
		pFiler->writeDouble(m_radiusRatio);
		pFiler->writeDouble(m_startAngle);
		pFiler->writeDouble(m_endAngle);
		return Mcad::eOk;
	}
	Mcad::ErrorStatus EllipseShape::transformBy(const McGeMatrix3d& xform)
	{
		m_center.transformBy(xform);
		return Mcad::eOk;
	}
	McDbEntity* EllipseShape::createEntity() const
	{
		McDbEllipse* pEllipse = new McDbEllipse(m_center, McGeVector3d::kZAxis, m_majorAxis, m_radiusRatio, m_startAngle, m_endAngle);
		return pEllipse;
	}
	QJsonObject EllipseShape::toJson() const
	{
		QJsonObject json;
		json["typeName"] = typeName();
		json["center"] = QJsonArray{ m_center.x, m_center.y };
		json["majorAxis"] = QJsonArray{ m_majorAxis.x, m_majorAxis.y };
		json["radiusRatio"] = m_radiusRatio;
		json["startAngle"] = m_startAngle;
		json["endAngle"] = m_endAngle;
		return json;
	}
	void EllipseShape::fromJson(const QJsonObject& json)
	{
		m_center = McGePoint3d(json["center"][0].toDouble(), json["center"][1].toDouble(), 0);
		m_majorAxis = McGeVector3d(json["majorAxis"][0].toDouble(), json["majorAxis"][1].toDouble(), 0);
		m_radiusRatio = json["radiusRatio"].toDouble();
		m_startAngle = json["startAngle"].toDouble();
		m_endAngle = json["endAngle"].toDouble();
	}
} // namespace Mx2d