/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dCustomCartesianCoord.h"
#include <QString>
#include "Mx2dUtils.h"
#include "Mx2dAnnotationFactory.h"

REGISTER_ANNOTATION("cartesianCoord", Mx2dCustomCartesianCoord);
ACRX_DXF_DEFINE_MEMBERS(Mx2dCustomCartesianCoord, Mx2dCustomAnnotation,
	McDb::kDHL_CURRENT, McDb::kMReleaseCurrent,
	McDbProxyEntity::kAllAllowedBits, CartesianCoordMark,
	CartesianCoord Mark Entity);

Mx2dCustomCartesianCoord::Mx2dCustomCartesianCoord(void)
{
	setType("cartesianCoord");
}

Mx2dCustomCartesianCoord::Mx2dCustomCartesianCoord(const McGePoint3d& startPt, const McGePoint3d& endPt, double textHeight)
	:Mx2dCustomAnnotation(textHeight), m_startPt(startPt), m_endPt(endPt)
{
	setType("cartesianCoord");
}

Mx2dCustomCartesianCoord::~Mx2dCustomCartesianCoord(void)
{
}

Mdesk::Boolean Mx2dCustomCartesianCoord::worldDraw(McGiWorldDraw* wd)
{
	double len = 0;
	McDbVoidPtrArray arr = createTexts(len);
	for (int i = 0; i < arr.length(); ++i) {
		McDbEntity* pEnt = (McDbEntity*)arr[i];
		pEnt->worldDraw(wd);
		delete pEnt;
	}
	arr.clear();
	McDbPolyline* pPoly = createPoly(len);
	pPoly->worldDraw(wd);
	delete pPoly;

	return Mdesk::kTrue;
}

Mcad::ErrorStatus Mx2dCustomCartesianCoord::getGripPoints(McGePoint3dArray& gripPoints, McGeIntArray& osnapModes, McGeIntArray& geomIds) const
{
	assertReadEnabled();
	gripPoints.append(m_startPt);
	gripPoints.append(m_endPt);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomCartesianCoord::moveGripPointsAt(const McGeIntArray& indices, const McGeVector3d& offset)
{
	assertWriteEnabled();
	int iIndex = indices[0];
	switch (iIndex)
	{
	case 0:
		m_startPt = m_startPt + offset;
		break;
	case 1:
		m_endPt = m_endPt + offset;
		break;
	}
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomCartesianCoord::getGeomExtents(McDbExtents& extents, bool roughCalculation) const
{
	assertReadEnabled();

	double len = 0;
	McDbExtents ext;
	McDbVoidPtrArray arr = createTexts(len);
	for (int i = 0; i < arr.length(); ++i) {
		McDbEntity* pEnt = (McDbEntity*)arr[i];
		pEnt->getGeomExtents(ext, roughCalculation);
		extents.addExt(ext);
		delete pEnt;
	}
	arr.clear();
	McDbPolyline* pPoly = createPoly(len);
	pPoly->getGeomExtents(ext, roughCalculation);
	extents.addExt(ext);
	delete pPoly;

	return Mcad::eOk;
}

#define CARTESIANCOORD_VERSION 1

Mcad::ErrorStatus Mx2dCustomCartesianCoord::dwgInFields(McDbDwgFiler* pFiler)
{
	assertWriteEnabled();
	if (pFiler->filerType() != McDb::kCustomEntityFileFiler)
	{
		Mcad::ErrorStatus es;
		if ((es = McDbEntity::dwgInFields(pFiler)) != Mcad::eOk)
		{
			return es;
		}
	}
	Mx2dCustomAnnotation::dwgInFields(pFiler);
	int lVar = 1;
	pFiler->readInt(&lVar);
	pFiler->readPoint3d(&m_startPt);
	pFiler->readPoint3d(&m_endPt);

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomCartesianCoord::dwgOutFields(McDbDwgFiler* pFiler) const
{
	assertReadEnabled();
	if (pFiler->filerType() != McDb::kCustomEntityFileFiler)
	{
		Mcad::ErrorStatus es;
		if ((es = McDbEntity::dwgOutFields(pFiler)) != Mcad::eOk)
		{
			return es;
		}
	}
	Mx2dCustomAnnotation::dwgOutFields(pFiler);
	pFiler->writeInt(CARTESIANCOORD_VERSION);
	pFiler->writePoint3d(m_startPt);
	pFiler->writePoint3d(m_endPt);

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomCartesianCoord::explode(McDbVoidPtrArray& entitySet) const
{
	double len = 0;
	McDbVoidPtrArray arr = createTexts(len);
	McDbPolyline* pPoly = createPoly(len);

	entitySet.append(arr);
	entitySet.append(pPoly);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomCartesianCoord::transformBy(const McGeMatrix3d& xform)
{
	assertWriteEnabled();
	m_startPt.transformBy(xform);
	m_endPt.transformBy(xform);
	return Mcad::eOk;
}

void Mx2dCustomCartesianCoord::fromJson(const QJsonObject& jsonObject)
{
	assertWriteEnabled();
	Mx2dCustomAnnotation::fromJson(jsonObject);
	m_startPt = Mx2d::jsonArray2dToPoint3d(jsonObject["startPoint"].toArray());
	m_endPt = Mx2d::jsonArray2dToPoint3d(jsonObject["endPoint"].toArray());
}

QJsonObject Mx2dCustomCartesianCoord::toJson() const
{
	assertReadEnabled();
	QJsonObject jsonObject = Mx2dCustomAnnotation::toJson();

	jsonObject["startPoint"] = Mx2d::point3dToJsonArray2d(m_startPt);
	jsonObject["endPoint"] = Mx2d::point3dToJsonArray2d(m_endPt);

	return jsonObject;
}

Mx2d::TextInfoList Mx2dCustomCartesianCoord::findText(const QString& text, bool isExactMatch) const
{
	assertReadEnabled();
	Mx2d::TextInfoList res;
	double len = 0;
	McDbVoidPtrArray arr = createTexts(len);
	for (int i = 0; i < arr.length(); ++i)
	{
		McDbExtents ext;
		McDbText* pText = (McDbText*)arr[i];
		pText->getGeomExtents(ext, false);
		QString textStr = QString::fromLocal8Bit(pText->textString());
		McGePoint3d minPt = ext.minPoint();
		McGePoint3d maxPt = ext.maxPoint();
		Mx2d::Extents extents{ minPt.x, minPt.y,maxPt.x, maxPt.y };
		delete pText;
		if ((isExactMatch && (text != textStr)) ||
			(!isExactMatch && !textStr.contains(text, Qt::CaseInsensitive)))
		{
			continue;;
		}
		res.append({ textStr , extents });
	}
	

	return res;
}

void Mx2dCustomCartesianCoord::setStartPt(const McGePoint3d& pt)
{
	assertWriteEnabled();
	m_startPt = pt;
}

McGePoint3d Mx2dCustomCartesianCoord::startPt() const
{
	assertReadEnabled();
	return m_startPt;
}

void Mx2dCustomCartesianCoord::setEndPt(const McGePoint3d& pt)
{
	assertWriteEnabled();
	m_endPt = pt;
}

McGePoint3d Mx2dCustomCartesianCoord::endPt() const
{
	assertReadEnabled();
	return m_endPt;
}



McDbVoidPtrArray Mx2dCustomCartesianCoord::createTexts(double& len) const
{
	double padding = textHeight() / 5;

	McDbObjectId textStyle = Mx::mcdbCurDwg()->textstyle();
	McDbMText* pMText = new McDbMText();

	QString mtext = QString("X: %1\nY: %2").arg(m_startPt.x, 0, 'f', 2).arg(m_startPt.y, 0, 'f', 2);

	pMText->setContents(mtext.toLocal8Bit().constData());
	pMText->setTextHeight(textHeight());
	pMText->setTextStyle(textStyle);
	pMText->setLocation(McGePoint3d(0, 0, 0));
	McDbExtents extents;
	// TODO: use mtext instead of text

	McDbVoidPtrArray arrText;
	pMText->explode(arrText);
	for (int i = 0; i < arrText.length(); i++)
	{
		McDbEntity* pEntity = (McDbEntity*)arrText[i];
		McDbExtents ext;
		pEntity->getGeomExtents(ext, false);
		extents.addExt(ext);

	}
	delete pMText;

	McGePoint3d minPt(extents.minPoint());
	McGePoint3d maxPt(extents.maxPoint());


	McGePoint3d centerPt((minPt.x + maxPt.x) / 2, (minPt.y + maxPt.y) / 2, 0);


	McGePoint3d lbPt = minPt - McGeVector3d(padding, padding, 0);
	McGePoint3d rtPt = maxPt + McGeVector3d(padding, padding, 0);

	McGePoint3d ltPt(lbPt.x, rtPt.y, 0);
	McGePoint3d rbPt(rtPt.x, lbPt.y, 0);


	McGePoint3d frameCenterPt((lbPt.x + rtPt.x) / 2, (lbPt.y + rtPt.y) / 2, 0);


	McGePoint3d leftMidPt(ltPt.x, frameCenterPt.y, 0);
	McGePoint3d rightMidPt(rtPt.x, frameCenterPt.y, 0);


	McGeVector3d vec = m_endPt - m_startPt;
	double angle = vec.angleTo(McGeVector3d::kXAxis); // always 0~PI
	McGeVector3d moveVec;
	if (angle > M_PI / 2)
	{
		moveVec = m_endPt - rightMidPt;
		len = leftMidPt.x - rightMidPt.x;
	}
	else
	{
		moveVec = m_endPt - leftMidPt;
		len = rightMidPt.x - leftMidPt.x;
	}

	for (int i = 0; i < arrText.length(); i++)
	{
		McDbEntity* pEntity = (McDbEntity*)arrText[i];
		pEntity->transformBy(McGeMatrix3d::translation(moveVec));
	}

	McDbVoidPtrArray arr;
	arr.append(arrText);

	return arr;
}

McDbPolyline* Mx2dCustomCartesianCoord::createPoly(double len) const
{

	McGePoint3d hPoint = m_endPt + McGeVector3d(len, 0, 0);

	McDbPolyline* pPoly = new McDbPolyline();
	pPoly->addVertexAt(m_startPt);
	pPoly->addVertexAt(m_endPt);
	pPoly->addVertexAt(hPoint);

	return pPoly;
}


