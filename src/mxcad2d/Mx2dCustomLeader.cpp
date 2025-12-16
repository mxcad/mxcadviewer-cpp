/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dCustomLeader.h"
#include "Mx2dUtils.h"
#include "Mx2dAnnotationFactory.h"

REGISTER_ANNOTATION("leader", Mx2dCustomLeader);
ACRX_DXF_DEFINE_MEMBERS(Mx2dCustomLeader, Mx2dCustomAnnotation,
	McDb::kDHL_CURRENT, McDb::kMReleaseCurrent,
	McDbProxyEntity::kAllAllowedBits, LeaderMark,
	Leader Mark Entity);

Mx2dCustomLeader::Mx2dCustomLeader(void)
{
	setType("leader");
}

Mx2dCustomLeader::Mx2dCustomLeader(const McGePoint3d& startPt, const McGePoint3d& endPt, const QString& text, double textHeight)
	:Mx2dCustomAnnotation(textHeight), m_startPt(startPt), m_endPt(endPt), m_text(text)
{
	setType("leader");
}

Mx2dCustomLeader::~Mx2dCustomLeader(void)
{
}

Mdesk::Boolean Mx2dCustomLeader::worldDraw(McGiWorldDraw* wd)
{
	McDbPolyline* pArrow = createArrow();
	pArrow->worldDraw(wd);
	delete pArrow;


	McDbLine* pLine = createLine();
	pLine->worldDraw(wd);
	delete pLine;

	McDbVoidPtrArray arr = createFrameAndTexts();
	McDbPolyline* pFrame = (McDbPolyline*)arr[0];
	pFrame->worldDraw(wd);
	delete pFrame;

	for (int i = 1; i < arr.length(); ++i) {
		McDbEntity* pEnt = (McDbEntity*)arr[i];
		pEnt->worldDraw(wd);
		delete pEnt;
	}

	arr.clear();

	return Mdesk::kTrue;
}

Mcad::ErrorStatus Mx2dCustomLeader::getGripPoints(McGePoint3dArray& gripPoints, McGeIntArray& osnapModes, McGeIntArray& geomIds) const
{
	assertReadEnabled();
	gripPoints.append(m_startPt);
	gripPoints.append(m_endPt);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomLeader::moveGripPointsAt(const McGeIntArray& indices, const McGeVector3d& offset)
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

Mcad::ErrorStatus Mx2dCustomLeader::getGeomExtents(McDbExtents& extents, bool roughCalculation) const
{
	assertReadEnabled();

	McDbExtents ext;
	McDbPolyline* pArrow = createArrow();
	pArrow->getGeomExtents(ext, roughCalculation);
	extents.addExt(ext);
	delete pArrow;


	McDbLine* pLine = createLine();
	pLine->getGeomExtents(ext, roughCalculation);
	extents.addExt(ext);
	delete pLine;

	McDbVoidPtrArray arr = createFrameAndTexts();
	McDbPolyline* pFrame = (McDbPolyline*)arr[0];
	pFrame->getGeomExtents(ext, roughCalculation);
	extents.addExt(ext);
	delete pFrame;

	for (int i = 1; i < arr.length(); ++i) {
		McDbEntity* pEnt = (McDbEntity*)arr[i];
		pEnt->getGeomExtents(ext, roughCalculation);
		extents.addExt(ext);
		delete pEnt;
	}

	arr.clear();

	return Mcad::eOk;
}

#define LEADER_VERSION 1

Mcad::ErrorStatus Mx2dCustomLeader::dwgInFields(McDbDwgFiler* pFiler)
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
	MxString mxStr;
	pFiler->readString(mxStr);
	m_text = QString::fromLocal8Bit(mxStr.c_str());

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomLeader::dwgOutFields(McDbDwgFiler* pFiler) const
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
	pFiler->writeInt(LEADER_VERSION);
	pFiler->writePoint3d(m_startPt);
	pFiler->writePoint3d(m_endPt);
	pFiler->writeString(MxString(m_text.toLocal8Bit().constData()));

	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomLeader::explode(McDbVoidPtrArray& entitySet) const
{
	McDbPolyline* pArrow = createArrow();
	McDbLine* pLine = createLine();
	McDbVoidPtrArray arr = createFrameAndTexts();

	entitySet.append(pArrow);
	entitySet.append(pLine);
	entitySet.append(arr);
	return Mcad::eOk;
}

Mcad::ErrorStatus Mx2dCustomLeader::transformBy(const McGeMatrix3d& xform)
{
	assertWriteEnabled();
	m_startPt.transformBy(xform);
	m_endPt.transformBy(xform);
	return Mcad::eOk;
}

void Mx2dCustomLeader::fromJson(const QJsonObject& jsonObject)
{
	assertWriteEnabled();
	Mx2dCustomAnnotation::fromJson(jsonObject);
	m_startPt = Mx2d::jsonArray2dToPoint3d(jsonObject["startPoint"].toArray());
	m_endPt = Mx2d::jsonArray2dToPoint3d(jsonObject["endPoint"].toArray());
	m_text = jsonObject["text"].toString();
}

QJsonObject Mx2dCustomLeader::toJson() const
{
	assertReadEnabled();
	QJsonObject jsonObject = Mx2dCustomAnnotation::toJson();

	jsonObject["startPoint"] = Mx2d::point3dToJsonArray2d(m_startPt);
	jsonObject["endPoint"] = Mx2d::point3dToJsonArray2d(m_endPt);
	jsonObject["text"] = m_text;

	return jsonObject;
}

void Mx2dCustomLeader::setStartPt(const McGePoint3d& pt)
{
	assertWriteEnabled();
	m_startPt = pt;
}

McGePoint3d Mx2dCustomLeader::startPt() const
{
	assertReadEnabled();
	return m_startPt;
}

void Mx2dCustomLeader::setEndPt(const McGePoint3d& pt)
{
	assertWriteEnabled();
	m_endPt = pt;
}

McGePoint3d Mx2dCustomLeader::endPt() const
{
	assertReadEnabled();
	return m_endPt;
}

void Mx2dCustomLeader::setText(const QString& text)
{
	assertWriteEnabled();
	m_text = text;
}

QString Mx2dCustomLeader::text() const
{
	assertReadEnabled();
	return m_text;
}


McDbPolyline* Mx2dCustomLeader::createArrow() const
{
	double arrowSize = textHeight() / 2.0;

	McGeVector3d dir = m_endPt - m_startPt;
	dir.normalize();

	McGeVector3d dirVec = dir * arrowSize;

	McGeVector3d vecRight = dirVec;
	vecRight.rotateBy(M_PI / 6, McGeVector3d::kZAxis);
	McGeVector3d vecLeft = dirVec;
	vecLeft.rotateBy(-M_PI / 6, McGeVector3d::kZAxis);

	McGePoint3d midPt = m_startPt;
	McGePoint3d rightPt = midPt + vecRight;
	McGePoint3d leftPt = midPt + vecLeft;

	McDbPolyline* pPoly = new McDbPolyline();
	pPoly->addVertexAt(rightPt);
	pPoly->addVertexAt(midPt);
	pPoly->addVertexAt(leftPt);

	return pPoly;
}

McDbLine* Mx2dCustomLeader::createLine() const
{
	return new McDbLine(m_startPt, m_endPt);
}

McDbVoidPtrArray Mx2dCustomLeader::createFrameAndTexts() const
{
	double padding = textHeight() / 5;

	McDbObjectId textStyle = Mx::mcdbCurDwg()->textstyle();
	McDbMText* pMText = new McDbMText();
	pMText->setContents(m_text.toLocal8Bit().constData());
	pMText->setTextHeight(textHeight());
	pMText->setTextStyle(textStyle);
	pMText->setLocation(McGePoint3d(0, 0, 0));
	McDbExtents extents;


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


	McDbPolyline* pFrame = new McDbPolyline();
	pFrame->addVertexAt(ltPt);
	pFrame->addVertexAt(rtPt);
	pFrame->addVertexAt(rbPt);
	pFrame->addVertexAt(lbPt);
	pFrame->setClosed(true);


	McGePoint3d frameCenterPt((lbPt.x + rtPt.x) / 2, (lbPt.y + rtPt.y) / 2, 0);


	McGePoint3d leftMidPt(ltPt.x, frameCenterPt.y, 0);
	McGePoint3d rightMidPt(rtPt.x, frameCenterPt.y, 0);


	McGeVector3d vec = m_endPt - m_startPt;
	double angle = vec.angleTo(McGeVector3d::kXAxis); // always 0~PI
	McGeVector3d moveVec;
	if (angle > M_PI / 2)
	{
		moveVec = m_endPt - rightMidPt;
	}
	else
	{
		moveVec = m_endPt - leftMidPt;
	}

	for (int i = 0; i < arrText.length(); i++)
	{
		McDbEntity* pEntity = (McDbEntity*)arrText[i];
		pEntity->transformBy(McGeMatrix3d::translation(moveVec));
	}
	pFrame->transformBy(McGeMatrix3d::translation(moveVec));

	McDbVoidPtrArray arr;
	arr.append(pFrame);
	arr.append(arrText);

	return arr;
}




