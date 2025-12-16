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
class Mx2dCustomAngleMeasurement : public Mx2dCustomAnnotation
{
	ACRX_DECLARE_MEMBERS(Mx2dCustomAngleMeasurement);
public:
	Mx2dCustomAngleMeasurement(void);
	Mx2dCustomAngleMeasurement(const McGePoint3d& line1Start, const McGePoint3d& line1End, const McGePoint3d& line2Start, const McGePoint3d& line2End, const McGePoint3d& dimPt, double textHeight, bool isDynamicDrawing = false);

	virtual ~Mx2dCustomAngleMeasurement(void);

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

	void setLine1Start(const McGePoint3d& line1Start);
	McGePoint3d line1Start() const;
	void setLine1End(const McGePoint3d& line1End);
	McGePoint3d line1End() const;
	void setLine2Start(const McGePoint3d& line2Start);
	McGePoint3d line2Start() const;
	void setLine2End(const McGePoint3d& line2End);
	McGePoint3d line2End() const;
	void setIsDynamicDrawing(bool isDynamicDrawing);
	bool isDynamicDrawing() const;

	void setDimPt(const McGePoint3d& dimPt);
	McGePoint3d dimPt() const;

protected:

	McDbVoidPtrArray createAllEntities() const;


private:
	McGePoint3d m_line1Start;
	McGePoint3d m_line1End;
	McGePoint3d m_line2Start;
	McGePoint3d m_line2End;
	McGePoint3d m_dimPt;
	bool m_isDynamicDrawing;
};