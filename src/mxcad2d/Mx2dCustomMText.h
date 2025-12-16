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
class Mx2dCustomMText : public Mx2dCustomAnnotation
{
	ACRX_DECLARE_MEMBERS(Mx2dCustomMText);
public:
	Mx2dCustomMText(void);
	Mx2dCustomMText(const QString& textStr, const McGePoint3d& dimPt, double textHeight);
	virtual ~Mx2dCustomMText(void);

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
	void setContents(const QString& textStr);
	QString contents() const;

	void setDimPt(const McGePoint3d& dimPt);
	McGePoint3d dimPt() const;

protected:
	McDbVoidPtrArray createTexts() const;
private:
	QString m_text;
	McGePoint3d m_dimPt;
};