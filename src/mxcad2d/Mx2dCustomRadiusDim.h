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
class Mx2dCustomRadiusDim : public Mx2dCustomAnnotation
{
	ACRX_DECLARE_MEMBERS(Mx2dCustomRadiusDim);
public:
	Mx2dCustomRadiusDim(void);
	Mx2dCustomRadiusDim(const McGePoint3d& startPt, const McGePoint3d& midPt, const McGePoint3d& endPt, const McGePoint3d& dimPt, double textHeight);
	Mx2dCustomRadiusDim(const McGePoint3d& centerPt, double radius, const McGePoint3d& dimPt, double textHeight);

	virtual ~Mx2dCustomRadiusDim(void);

	virtual Mdesk::Boolean  worldDraw(McGiWorldDraw* wd) override;

	virtual Mcad::ErrorStatus   getGripPoints(McGePoint3dArray& gripPoints, McGeIntArray& osnapModes, McGeIntArray& geomIds) const override;

	virtual Mcad::ErrorStatus moveGripPointsAt(const McGeIntArray& indices, const McGeVector3d& offset) override;

	virtual Mcad::ErrorStatus   getGeomExtents(McDbExtents& extents, bool roughCalculation = true) const override;

	virtual Mcad::ErrorStatus dwgInFields(McDbDwgFiler* pFiler) override;

	virtual Mcad::ErrorStatus dwgOutFields(McDbDwgFiler* pFiler) const override;

	virtual Mcad::ErrorStatus   explode(McDbVoidPtrArray& entitySet) const override;

	virtual Mcad::ErrorStatus   transformBy(const McGeMatrix3d& xform) override;
	virtual Mcad::ErrorStatus getOsnapPoints(
		McDb::OsnapMode     osnapMode,
		int                 gsSelectionMark,
		const McGePoint3d& pickPoint,
		const McGePoint3d& lastPoint,
		const McGeMatrix3d& viewXform,
		McGePoint3dArray& snapPoints,
		McDbIntArray& geomIds) const override;

	virtual void fromJson(const QJsonObject& jsonObject) override;
	virtual QJsonObject toJson() const override;
public:

	void setStartPt(const McGePoint3d& startPt);
	McGePoint3d startPt() const;

	void setMidPt(const McGePoint3d& midPt);
	McGePoint3d midPt() const;

	void setEndPt(const McGePoint3d& endPt);
	McGePoint3d endPt() const;

	void setCenterPt(const McGePoint3d& centerPt);
	McGePoint3d centerPt() const;

	void setDimPt(const McGePoint3d& dimPt);
	McGePoint3d dimPt() const;

	void setRadius(double radius);
	double radius() const;

	void setIsArc(bool isArc);
	bool isArc() const;


protected:
	McDbPolyline* createArrow(McGePoint3d& arrowPoint) const;

	McDbLine* createLine(const McGePoint3d& arrowPoint) const;

	McDbArc* createArc(const McGePoint3d& arrowPoint) const;

	McDbText* createText() const;

	void calculateArc(McGePoint3d& center, double& radius) const;
private:
	McGePoint3d m_startPt;
	McGePoint3d m_midPt;
	McGePoint3d m_endPt;
	McGePoint3d m_centerPt;
	McGePoint3d m_dimPt;
	double m_radius;
	bool m_isArc;
};