/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include "MxCADInclude.h"
#include "Mx2dCustomAnnotation.h"
class Mx2dCustomEllipse : public Mx2dCustomAnnotation
{
	ACRX_DECLARE_MEMBERS(Mx2dCustomEllipse);
public:
	Mx2dCustomEllipse(void);
	Mx2dCustomEllipse(const McGePoint3d& pt1, const McGePoint3d& pt2, double textHeight);

	virtual ~Mx2dCustomEllipse(void);

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

	DimPropertyFlags dimPropertyFlags() const override;
public:
	void setCorner1(const McGePoint3d& pt);
	McGePoint3d corner1() const;

	void setCorner2(const McGePoint3d& pt);
	McGePoint3d corner2() const;
protected:
	McDbEllipse* createEllipse() const;
private:
	McGePoint3d m_corner1;
	McGePoint3d m_corner2;
};

