/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QObject>
#include <QString>
#include <QJsonObject>
#include "MxCADInclude.h"

namespace Mx2d {
	class CurveShape : public QObject
	{
		Q_OBJECT
	public:
		CurveShape() = default;
		virtual ~CurveShape() = default;

		void setTypeName(const QString& typeName);
		QString typeName() const;
		virtual double getLength() const = 0;
		virtual Mcad::ErrorStatus dwgInFields(McDbDwgFiler* pFiler) = 0;
		virtual Mcad::ErrorStatus dwgOutFields(McDbDwgFiler* pFiler) const = 0;
		virtual Mcad::ErrorStatus transformBy(const McGeMatrix3d& xform) = 0;
		virtual McDbEntity* createEntity() const = 0;
		virtual QJsonObject toJson() const = 0;
		virtual void fromJson(const QJsonObject& json) = 0;
	private:
		QString m_typeName = "CurveShape";
	};



	class LineShape : public CurveShape
	{
		Q_OBJECT
	public:
		LineShape();
		LineShape(const McGePoint3d& startPt, const McGePoint3d& endPt);
		virtual ~LineShape() = default;

		virtual double getLength() const override;
		virtual Mcad::ErrorStatus dwgInFields(McDbDwgFiler* pFiler) override;
		virtual Mcad::ErrorStatus dwgOutFields(McDbDwgFiler* pFiler) const override;
		virtual Mcad::ErrorStatus transformBy(const McGeMatrix3d& xform) override;
		virtual McDbEntity* createEntity() const override;
		virtual QJsonObject toJson() const override;
		virtual void fromJson(const QJsonObject& json) override;
	private:
		McGePoint3d m_startPt;
		McGePoint3d m_endPt;
	};


	class CircleShape : public CurveShape
	{
		Q_OBJECT
	public:
		CircleShape();
		CircleShape(const McGePoint3d& center, double radius);
		virtual ~CircleShape() = default;

		virtual double getLength() const override;
		virtual Mcad::ErrorStatus dwgInFields(McDbDwgFiler* pFiler) override;
		virtual Mcad::ErrorStatus dwgOutFields(McDbDwgFiler* pFiler) const override;
		virtual Mcad::ErrorStatus transformBy(const McGeMatrix3d& xform) override;
		virtual McDbEntity* createEntity() const override;
		virtual QJsonObject toJson() const override;
		virtual void fromJson(const QJsonObject& json) override;
	private:
		McGePoint3d m_center;
		double m_radius;
	};

	class ArcShape : public CurveShape
	{
		Q_OBJECT
	public:
		ArcShape();
		ArcShape(const McGePoint3d& startPt, const McGePoint3d& midPt, const McGePoint3d& endPt);
		virtual ~ArcShape() = default;

		virtual double getLength() const override;
		virtual Mcad::ErrorStatus dwgInFields(McDbDwgFiler* pFiler) override;
		virtual Mcad::ErrorStatus dwgOutFields(McDbDwgFiler* pFiler) const override;
		virtual Mcad::ErrorStatus transformBy(const McGeMatrix3d& xform) override;
		virtual McDbEntity* createEntity() const override;
		virtual QJsonObject toJson() const override;
		virtual void fromJson(const QJsonObject& json) override;
	private:
		McGePoint3d m_startPt;
		McGePoint3d m_midPt;
		McGePoint3d m_endPt;
	};



	class EllipseShape : public CurveShape
	{
		Q_OBJECT
	public:
		EllipseShape();
		EllipseShape(const McGePoint3d& center, const McGeVector3d& majorAxis, double radiusRatio, double startAngle, double endAngle);
		virtual ~EllipseShape() = default;

		virtual double getLength() const override;
		virtual Mcad::ErrorStatus dwgInFields(McDbDwgFiler* pFiler) override;
		virtual Mcad::ErrorStatus dwgOutFields(McDbDwgFiler* pFiler) const override;
		virtual Mcad::ErrorStatus transformBy(const McGeMatrix3d& xform) override;
		virtual McDbEntity* createEntity() const override;
		virtual QJsonObject toJson() const override;
		virtual void fromJson(const QJsonObject& json) override;
	private:
		McGePoint3d m_center;
		McGeVector3d m_majorAxis;
		double m_radiusRatio;
		double m_startAngle;
		double m_endAngle;
	};

}// namespace Mx2d