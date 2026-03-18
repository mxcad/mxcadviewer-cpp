/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include "MxCADInclude.h"
#include "Mx2dUtils.h"
#include "Mx2dCustomAnnotation.h"
class Mx2dCustomContinuousMeasurement : public Mx2dCustomAnnotation
{
	ACRX_DECLARE_MEMBERS(Mx2dCustomContinuousMeasurement);
public:
	Mx2dCustomContinuousMeasurement(void);
	Mx2dCustomContinuousMeasurement(const Mx2d::PLVertexList& pts, const McGePoint3d& dimPt, double textHeight);

	virtual ~Mx2dCustomContinuousMeasurement(void);

	Mdesk::Boolean  worldDraw(McGiWorldDraw* wd) override;

	Mcad::ErrorStatus   getGripPoints(McGePoint3dArray& gripPoints, McGeIntArray& osnapModes, McGeIntArray& geomIds) const override;

	Mcad::ErrorStatus moveGripPointsAt(const McGeIntArray& indices, const McGeVector3d& offset) override;

	Mcad::ErrorStatus   getGeomExtents(McDbExtents& extents, bool roughCalculation = true) const override;

	Mcad::ErrorStatus dwgInFields(McDbDwgFiler* pFiler) override;

	Mcad::ErrorStatus dwgOutFields(McDbDwgFiler* pFiler) const override;

	Mcad::ErrorStatus   explode(McDbVoidPtrArray& entitySet) const override;

	Mcad::ErrorStatus   transformBy(const McGeMatrix3d& xform) override;

	void fromJson(const QJsonObject& jsonObject) override;
	QJsonObject toJson() const override;
	Mx2d::TextInfoList findText(const QString& text, bool isExactMatch = false) const override;

	DimPropertyFlags dimPropertyFlags() const override;
public:
	void setPoints(const Mx2d::PLVertexList& pts);
	Mx2d::PLVertexList points() const;

	Mcad::ErrorStatus explodePoly(McDbVoidPtrArray& entitySet) const;
protected:
	McDbPolyline* createPoly() const;
	McDbLine* createStartEdge(McDbPolyline* poly) const;
	McDbLine* createEndEdge(McDbPolyline* poly) const;
	McDbPolyline* createDimLine(double textLength) const;
	McDbText* createText(McDbPolyline* poly, double& textLength) const;
private:
	Mx2d::PLVertexList m_pts;
};