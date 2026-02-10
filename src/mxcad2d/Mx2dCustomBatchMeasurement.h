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

	virtual Mdesk::Boolean  worldDraw(McGiWorldDraw* wd) override;

	virtual Mcad::ErrorStatus   getGripPoints(McGePoint3dArray& gripPoints, McGeIntArray& osnapModes, McGeIntArray& geomIds) const override;

	virtual Mcad::ErrorStatus moveGripPointsAt(const McGeIntArray& indices, const McGeVector3d& offset) override;

	virtual Mcad::ErrorStatus   getGeomExtents(McDbExtents& extents, bool roughCalculation = true) const override;

	virtual Mcad::ErrorStatus dwgInFields(McDbDwgFiler* pFiler) override;

	virtual Mcad::ErrorStatus dwgOutFields(McDbDwgFiler* pFiler) const override;

	virtual Mcad::ErrorStatus   explode(McDbVoidPtrArray& entitySet) const override;

	virtual Mcad::ErrorStatus   transformBy(const McGeMatrix3d& xform) override;

	virtual void fromJson(const QJsonObject& jsonObject) override;
	virtual QJsonObject toJson() const override;

	virtual Mx2d::TextInfoList findText(const QString& text, bool isExactMatch = false) const override;
public:
	void setCurveShapes(const Mx2d::CurveShapeList& curveShapes);
	Mx2d::CurveShapeList curveShapes() const;

	void setDimPt(const McGePoint3d& dimPt);
	McGePoint3d dimPt() const;

	void setIsDynamicDrawing(bool isDynamicDrawing);
	bool isDynamicDrawing() const;

protected:
	McDbText* createText() const;

private:
	Mx2d::CurveShapeList m_curveShapes;
	McGePoint3d m_dimPt;
	bool m_isDynamicDrawing;
};