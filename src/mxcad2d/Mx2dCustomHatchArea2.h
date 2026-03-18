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
class Mx2dCustomHatchArea2 : public Mx2dCustomAnnotation
{
	ACRX_DECLARE_MEMBERS(Mx2dCustomHatchArea2);
public:
	Mx2dCustomHatchArea2(void);
	Mx2dCustomHatchArea2(const Mx2d::HatchPLList& polys, const McGePoint3d& dimPt, double textHeight, bool isDynamicDrawing = false);

	virtual ~Mx2dCustomHatchArea2(void);

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
	void setPolys(const Mx2d::HatchPLList& polys);
	Mx2d::HatchPLList polys() const;

	void setIsDynamicDrawing(bool isDynamicDrawing);
	bool isDynamicDrawing() const;
protected:

	McDbVoidPtrArray createPolys() const;

	McDbText* createText(double area, double perimeter) const;

	double getPolyArea() const;
	double getPerimeter() const;
private:
	Mx2d::HatchPLList  m_polys;
	bool m_isDynamicDrawing;
};

