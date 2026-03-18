/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include "MxCADInclude.h"
#include <QString>
#include "Mx2dUtils.h"
#include "Mx2dCustomAnnotation.h"
class Mx2dCustomArcPolyArea : public Mx2dCustomAnnotation
{
	ACRX_DECLARE_MEMBERS(Mx2dCustomArcPolyArea);
public:
	Mx2dCustomArcPolyArea(void);
	Mx2dCustomArcPolyArea(const Mx2d::PLVertexList& pts, const McGePoint3d& dimPt, double textHeight);

	virtual ~Mx2dCustomArcPolyArea(void);

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

protected:
	McDbPolyline* createPoly() const;
	McDbText* createText(double area, double perimeter) const;

	double getPolyArea(McDbPolyline* pPoly) const;
	double getPerimeter(McDbPolyline* pPoly) const;
private:
	Mx2d::PLVertexList  m_pts;
};

