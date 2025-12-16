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
#include "Mx2dCustomAnnotation.h"
class Mx2dCustomPolyArea : public Mx2dCustomAnnotation
{
	ACRX_DECLARE_MEMBERS(Mx2dCustomPolyArea);
public:
	Mx2dCustomPolyArea(void);
	Mx2dCustomPolyArea(const McGePoint3dArray& polyPoints, const McGePoint3d& textPos, double textHeight);

	virtual ~Mx2dCustomPolyArea(void);

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
public:
	void setPoints(const McGePoint3dArray& points);
	McGePoint3dArray points() const;

	void setTextPos(const McGePoint3d& pos);
	McGePoint3d textPos() const;

protected:
	McDbPolyline* createPoly() const;
	McDbText* createText(double area, double perimeter) const;

	double getPolyArea(McDbPolyline* pPoly) const;
	double getPerimeter(McDbPolyline* pPoly) const;
private:
	McGePoint3dArray m_polyPoints;
	McGePoint3d m_textPos;
};

