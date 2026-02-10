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
class Mx2dCustomLeader : public Mx2dCustomAnnotation
{
	ACRX_DECLARE_MEMBERS(Mx2dCustomLeader);
public:
	Mx2dCustomLeader(void);
	Mx2dCustomLeader(const McGePoint3d& startPt, const McGePoint3d& endPt, const QString& text, double textHeight);

	virtual ~Mx2dCustomLeader(void);

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
	void setStartPt(const McGePoint3d& pt);
	McGePoint3d startPt() const;

	void setEndPt(const McGePoint3d& pt);
	McGePoint3d endPt() const;

	void setText(const QString& text);
	QString text() const;

protected:

	McDbPolyline* createArrow() const;
	McDbLine* createLine() const;
	McDbVoidPtrArray createFrameAndTexts() const;
private:
	McGePoint3d m_startPt;
	McGePoint3d m_endPt;
	QString m_text;
};