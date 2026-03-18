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
class Mx2dCustomBatchMeasurement : public Mx2dCustomAnnotation
{
	ACRX_DECLARE_MEMBERS(Mx2dCustomBatchMeasurement);
public:
	Mx2dCustomBatchMeasurement(void);
	Mx2dCustomBatchMeasurement(const Mx2d::CurveShapeList& curveShapes, const McGePoint3d& dimPt, double textHeight, bool isDynamicDrawing = false);

	virtual ~Mx2dCustomBatchMeasurement(void);

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
	void setCurveShapes(const Mx2d::CurveShapeList& curveShapes);
	Mx2d::CurveShapeList curveShapes() const;


	void setIsDynamicDrawing(bool isDynamicDrawing);
	bool isDynamicDrawing() const;

protected:
	McDbText* createText() const;

private:
	Mx2d::CurveShapeList m_curveShapes;
	bool m_isDynamicDrawing;
};