/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include "MxCADInclude.h"
#include <QJsonObject>
#include <QString>
#include "Mx2dUtils.h"

class Mx2dCustomAnnotation : public McDbEntity
{
	ACRX_DECLARE_MEMBERS(Mx2dCustomAnnotation);
public:
	Mx2dCustomAnnotation(void);
	Mx2dCustomAnnotation(double textHeight);
	virtual ~Mx2dCustomAnnotation(void);

	virtual Mcad::ErrorStatus dwgInFields(McDbDwgFiler* pFiler) override;

	virtual Mcad::ErrorStatus dwgOutFields(McDbDwgFiler* pFiler) const override;

public:
	void setTextHeight(double textHeight);
	double textHeight() const;

	void setDimRatio(double dimRatio);
	double dimRatio() const;

	void setCategory(const QString& category);
	QString category() const;

	void setType(const QString& type);
	QString type() const;

	void setLayout(const QString& layoutName);
    QString layout() const;

	virtual void fromJson(const QJsonObject& jsonObject);
	virtual QJsonObject toJson() const;

	virtual Mx2d::TextInfoList findText(const QString& text, bool isExactMatch = false) const;

protected:
	double m_textHeight;
	double m_dimRatio = 1.0;
	QString m_category = "default";
	QString m_type = "annotation";
	QString m_layout = "Model";
};