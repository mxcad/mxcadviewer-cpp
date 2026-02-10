/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "MxCADCommand.h"
#include <QMessageBox>
#include <QDebug>
#include <qmath.h>
#include "Mx2dUtils.h"
#include "xlsxwriter.h"
#include <QStandardPaths>
#include <QFileDialog>
#include "Mx2dCustomRectCloud.h"
#include "Mx2dCustomRect.h"
#include "Mx2dCustomLine.h"
#include "Mx2dCustomEllipse.h"
#include "Mx2dCustomLeader.h"
#include "Mx2dCustomText.h"
#include "Mx2dCustomMText.h"
#include "Mx2dCustomPolyArea.h"
#include "Mx2dCustomRectArea.h"
#include "Mx2dCustomCartesianCoord.h"
#include "Mx2dCustomAlignedDim.h"
#include "Mx2dCustomLinearDim.h"
#include "Mx2dCustomContinuousMeasurement.h"
#include "Mx2dCustomRadiusDim.h"
#include "Mx2dCustomArcLengthDim.h"
#include "Mx2dCustomCircleMeasurement.h"
#include "Mx2dCustomAngleMeasurement.h"
#include "Mx2dCustomArcPolyArea.h"
#include "Mx2dCustomHatchArea.h"
#include "Mx2dCustomHatchArea2.h"
#include "Mx2dCustomBatchMeasurement.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCoreApplication>
#include "Mx2dSignalTransfer.h"
#include "MxSignalTransfer.h"
#include "MxUtils.h"
#include "MxLogger.h"


#define HIDE_PROMPT_MESSAGE \
	MXAPP.CallMain([=]() {	\
		emit MxSignalTransfer::getInstance().signalHidePromptMessage(MxUtils::gCurrentTab);	\
	})


#define SHOW_PROMPT_MESSAGE(msg) \
        MXAPP.CallMain([=]() { \
        emit MxSignalTransfer::getInstance().signalShowPromptMessage(MxUtils::gCurrentTab, msg + QCoreApplication::translate("MxCADCommand", ",Right-click to break current step, Esc to stop current command.")); \
    })
void MxCADCommand::RegisterCommand()
{
	Mx_AddThreadCommand(Mx_TestThread);
	Mx_AddCommand(Mx_Test);
	Mx_AddCommand(Mx_TextSearch);
	Mx_AddThreadCommand(Mx_TextSearchRect);
	Mx_AddThreadCommand(Mx_TextSearchPolygon);
	Mx_AddThreadCommand(Mx_GetRectArea);
	Mx_AddThreadCommand(Mx_GetPolyArea);
	Mx_AddThreadCommand(Mx_DrawLine);
	Mx_AddCommand(Mx_MoveViewCenterTo);
	Mx_AddCommand(Mx_ZoomAll);
	Mx_AddThreadCommand(Mx_ZoomWin);
	Mx_AddThreadCommand(Mx_GetAllLayer);
	Mx_AddCommand(Mx_SetSomeLayersOffStatus);
	Mx_AddThreadCommand(Mx_SetSelectedLayersOff);
	Mx_AddThreadCommand(Mx_SetNotSelectedLayersOff);
	Mx_AddThreadCommand(Mx_ExtractTextRect);
	Mx_AddThreadCommand(Mx_ExtractTextPoly);
	Mx_AddThreadCommand(Mx_ExtractTable);
	Mx_AddThreadCommand(Mx_DrawNumberedText);
	Mx_AddThreadCommand(Mx_DrawText);
	Mx_AddThreadCommand(Mx_DrawMText);
	Mx_AddThreadCommand(Mx_DrawRectMark);
	Mx_AddThreadCommand(Mx_DrawEllipseMark);
	Mx_AddThreadCommand(Mx_DrawRectCloudMark);
	Mx_AddThreadCommand(Mx_DrawLeaderMark);
	Mx_AddThreadCommand(Mx_DrawPolyAreaMark);
	Mx_AddThreadCommand(Mx_DrawArcPolyAreaMark);
	Mx_AddThreadCommand(Mx_DrawRectAreaMark);
	Mx_AddThreadCommand(Mx_DrawCartesianCoordMark);
	Mx_AddThreadCommand(Mx_DrawAlignedDimMark);
	Mx_AddThreadCommand(Mx_DrawLinearDimMark);
	Mx_AddThreadCommand(Mx_DrawContinuousMeasurementMark);
	Mx_AddThreadCommand(Mx_ShowSegmentLengths);
	Mx_AddThreadCommand(Mx_DrawRadiusDimMark);
	Mx_AddThreadCommand(Mx_DrawArcLengthDimMark);
	Mx_AddThreadCommand(Mx_DrawCircleMeasurementMark);
	Mx_AddThreadCommand(Mx_DrawDistPointToLineMark);
	Mx_AddThreadCommand(Mx_DrawAngleMeasurementMark);
	Mx_AddThreadCommand(Mx_CalculateSiderArea);
	Mx_AddThreadCommand(Mx_DrawHatchArea2Mark);
	Mx_AddThreadCommand(Mx_MoveText);
	Mx_AddThreadCommand(Mx_CopyText);
	Mx_AddThreadCommand(Mx_ModifyText);
	Mx_AddThreadCommand(Mx_SelectTextToModify);
	Mx_AddThreadCommand(Mx_SelectAreaToOffset);
	Mx_AddThreadCommand(Mx_DoOffsetArea);
	Mx_AddThreadCommand(Mx_BatchMeasure);
	Mx_AddThreadCommand(Mx_MoveAnnotation);
	Mx_AddThreadCommand(Mx_CopyAnnotation);
	Mx_AddThreadCommand(Mx_EraseAnnotation);
	Mx_AddThreadCommand(Mx_GetDrawingLength);
	Mx_AddCommand(Mx_UpdateDisplay);
}


void MxCADCommand::textFilter(Mx2d::TextInfoList& result, const McDbObjectIdArray& ids, const QString& containStr, bool isFull)
{
	for (int i = 0; i < ids.length(); i++)
	{
		McDbObjectId curId = ids[i];
		McDbObjectPointer<McDbEntity> spEntity(curId, McDb::kForRead);
		if (spEntity.openStatus() != Mcad::eOk)
			continue;

		if (spEntity->isA() == McDbText::desc())
		{
			McDbText* pText = McDbText::cast(spEntity.object());
			QString str = QString::fromLocal8Bit(pText->textString());
			if (isFull)
			{
				if (str == containStr)
				{
					McDbExtents ext;
					pText->getGeomExtents(ext, false);
					McGePoint3d minPt = ext.minPoint();
					McGePoint3d maxPt = ext.maxPoint();
					double minX, minY, maxX, maxY;
					minX = minPt.x; minY = minPt.y; maxX = maxPt.x; maxY = maxPt.y;
					Mx2d::Extents ext2d(minX, minY, maxX, maxY);

					result.append({ str, ext2d });
				}
			}
			else
			{
				if (str.contains(containStr, Qt::CaseInsensitive))
				{
					McDbExtents ext;
					pText->getGeomExtents(ext, false);
					McGePoint3d minPt = ext.minPoint();
					McGePoint3d maxPt = ext.maxPoint();
					double minX, minY, maxX, maxY;
					minX = minPt.x; minY = minPt.y; maxX = maxPt.x; maxY = maxPt.y;
					Mx2d::Extents ext2d(minX, minY, maxX, maxY);

					result.append({ str, ext2d });
				}
			}

		}
		else if (spEntity->isA() == McDbMText::desc())
		{
			McDbMText* pMText = McDbMText::cast(spEntity.object());
			McDbVoidPtrArray resultExplode;
			if (pMText->explode(resultExplode) != Mcad::eOk)
				continue;
			for (int j = 0; j < resultExplode.length(); j++)
			{
				McDbText* pText = static_cast<McDbText*>(resultExplode[j]);
				QString str = QString::fromLocal8Bit(pText->textString());
				if (isFull)
				{
					if (str == containStr)
					{
						McDbExtents ext;
						pText->getGeomExtents(ext, false);
						McGePoint3d minPt = ext.minPoint();
						McGePoint3d maxPt = ext.maxPoint();
						double minX, minY, maxX, maxY;
						minX = minPt.x; minY = minPt.y; maxX = maxPt.x; maxY = maxPt.y;
						Mx2d::Extents ext2d(minX, minY, maxX, maxY);

						result.append({ str, ext2d });
					}
				}
				else
				{
					if (str.contains(containStr, Qt::CaseInsensitive))
					{
						McDbExtents ext;
						pText->getGeomExtents(ext, false);
						McGePoint3d minPt = ext.minPoint();
						McGePoint3d maxPt = ext.maxPoint();
						double minX, minY, maxX, maxY;
						minX = minPt.x; minY = minPt.y; maxX = maxPt.x; maxY = maxPt.y;
						Mx2d::Extents ext2d(minX, minY, maxX, maxY);

						result.append({ str, ext2d });
					}
				}
				delete pText;
			}
			resultExplode.clear();
		}
		else if (spEntity->isA() == McDbBlockReference::desc())
		{
			McDbBlockReference* pBlkRef = McDbBlockReference::cast(spEntity.object());
			deepTextInBlockRef(result, *pBlkRef, curId, containStr, isFull);
		}
	}
}

void MxCADCommand::worldDrawLine(const McGePoint3d& startPoint, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw)
{
	if (!pData->isValidCurPoint) return;
	McGePoint3d curPoint = pData->curPoint;
	McCmColor color;
	color.setRGB(230, 81, 0);
	pWorldDraw->subEntityTraits().setTrueColor(color);
	auto spLine = std::make_unique<Mx2dCustomLine>(startPoint, curPoint, 1.0);
	spLine->worldDraw(pWorldDraw);
}

void MxCADCommand::worldDrawRect(const McGePoint3d& corner1, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw)
{
	if (!pData->isValidCurPoint) return;
	McGePoint3d curPoint = pData->curPoint;
	McCmColor color;
	color.setRGB(230, 81, 0);
	pWorldDraw->subEntityTraits().setTrueColor(color);
	auto spRect = std::make_unique<Mx2dCustomRect>(corner1, curPoint, 1.0);
	spRect->worldDraw(pWorldDraw);
}

void MxCADCommand::worldDrawPolygon(const McGePoint3dArray& pts, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw)
{
	if (!pData->isValidCurPoint) return;
	McGePoint3d curPoint = pData->curPoint;

	if (pts.length() > 0)
	{
		McCmColor color;
		color.setRGB(230, 81, 0);
		pWorldDraw->subEntityTraits().setTrueColor(color);
		auto spPoly = std::make_unique<McDbPolyline>();
		for (int i = 0; i < pts.length(); i++)
		{
			spPoly->addVertexAt(pts[i]);
		}
		spPoly->addVertexAt(curPoint);
		spPoly->setClosed(true);
		spPoly->worldDraw(pWorldDraw);
	}
}

void MxCADCommand::worldDrawPolygon(const Mx2d::PLVertexList& pts, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw)
{
	if (!pData->isValidCurPoint) return;
	McGePoint3d curPoint = pData->curPoint;

	if (pts.length() > 0)
	{
		auto spPoly = std::make_unique<McDbPolyline>();
		for (int i = 0; i < pts.length(); i++)
		{
			spPoly->addVertexAt(pts[i].pt, pts[i].bulge);
		}
		spPoly->addVertexAt(curPoint);
#if 0
		spPoly->worldDraw(pWorldDraw);
#elif 1
		// Save original drawing context
		McCmColor oldColor = pWorldDraw->subEntityTraits().trueColor();

		// Set new color for drawing context
		McCmColor newColor; newColor.setRGB(230, 81, 0);
		pWorldDraw->subEntityTraits().setTrueColor(newColor);

		// Use drawing context to draw the shape
		spPoly->worldDraw(pWorldDraw);

		// Call setupForEntity to apply drawing context attributes to the drawn shape
		pWorldDraw->subEntityTraits().setupForEntity(spPoly.get());

		// Restore original drawing context
		pWorldDraw->subEntityTraits().setTrueColor(oldColor);
#endif // 0

	}
}


void MxCADCommand::worldDrawEllipse(const McGePoint3d& corner1, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw)
{
	if (!pData->isValidCurPoint) return;
	McGePoint3d corner2 = pData->curPoint;
	McCmColor color;
	color.setRGB(230, 81, 0);
	pWorldDraw->subEntityTraits().setTrueColor(color);
	auto spEllipse = std::make_unique<Mx2dCustomEllipse>(corner1, corner2, 1.0);
	spEllipse->worldDraw(pWorldDraw);
}

void MxCADCommand::worldDrawRectCloud(const McGePoint3d& corner1, double textHeight, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw)
{
	if (!pData->isValidCurPoint) return;
	McGePoint3d corner2 = pData->curPoint;
	McCmColor color;
	color.setRGB(230, 81, 0);
	pWorldDraw->subEntityTraits().setTrueColor(color);
	auto spRectCloud = std::make_unique<Mx2dCustomRectCloud>(corner1, corner2, textHeight);
	spRectCloud->worldDraw(pWorldDraw);
}

void MxCADCommand::worldDrawText(const QString& str, double textHeight, McDbObjectId textStyle, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw)
{
	if (!pData->isValidCurPoint) return;
	McGePoint3d curPoint = pData->curPoint;
	McCmColor color;
	color.setRGB(230, 81, 0);
	pWorldDraw->subEntityTraits().setTrueColor(color);
	auto spText = std::make_unique<Mx2dCustomText>(str, curPoint, textHeight);
	spText->worldDraw(pWorldDraw);
}

void MxCADCommand::worldDrawMText(const QString& str, double textHeight, McDbObjectId textStyle, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw)
{
	if (!pData->isValidCurPoint) return;
	McGePoint3d curPoint = pData->curPoint;
	McCmColor color;
	color.setRGB(230, 81, 0);
	pWorldDraw->subEntityTraits().setTrueColor(color);
	auto spMText = std::make_unique<Mx2dCustomMText>(str, curPoint, textHeight);
	spMText->worldDraw(pWorldDraw);
}

void MxCADCommand::worldDrawLeader(const McGePoint3d& startPt, const QString& text, double textHeight, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw)
{
	if (!pData->isValidCurPoint) return;
	McGePoint3d curPoint = pData->curPoint;
	McCmColor color;
	color.setRGB(230, 81, 0);
	pWorldDraw->subEntityTraits().setTrueColor(color);
	auto spLeader = std::make_unique<Mx2dCustomLeader>(startPt, curPoint, text, textHeight);
	spLeader->worldDraw(pWorldDraw);
}

void MxCADCommand::worldDrawPolyArea(const McGePoint3dArray& pts, double textHeight, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw)
{
	if (!pData->isValidCurPoint) return;
	McGePoint3d curPoint = pData->curPoint;
	if (pts.length() >= 3)
	{
		McCmColor color;
		color.setRGB(230, 81, 0);
		pWorldDraw->subEntityTraits().setTrueColor(color);
		auto spPolyArea = std::make_unique<Mx2dCustomPolyArea>(pts, curPoint, textHeight);
		spPolyArea->worldDraw(pWorldDraw);
	}
}

void MxCADCommand::worldDrawRectArea(const McGePoint3d& corner1, const McGePoint3d& corner2, double textHeight, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw)
{
	if (!pData->isValidCurPoint) return;
	McGePoint3d curPoint = pData->curPoint;
	McCmColor color;
	color.setRGB(230, 81, 0);
	pWorldDraw->subEntityTraits().setTrueColor(color);
	auto spRectArea = std::make_unique<Mx2dCustomRectArea>(corner1, corner2, curPoint, textHeight);
	spRectArea->worldDraw(pWorldDraw);
}

void MxCADCommand::worldDrawCartesianCoord(const McGePoint3d& insertPt, double textHeight, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw)
{
	if (!pData->isValidCurPoint) return;
	McGePoint3d curPoint = pData->curPoint;
	McCmColor color;
	color.setRGB(230, 81, 0);
	pWorldDraw->subEntityTraits().setTrueColor(color);
	auto spCoord = std::make_unique<Mx2dCustomCartesianCoord>(insertPt, curPoint, textHeight);
	spCoord->worldDraw(pWorldDraw);
}

void MxCADCommand::worldDrawAlignedDim(const McGePoint3d& p1, const McGePoint3d& p2, double textHeight, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw)
{
	if (!pData->isValidCurPoint) return;
	McGePoint3d curPoint = pData->curPoint;
	McCmColor color;
	color.setRGB(230, 81, 0);
	pWorldDraw->subEntityTraits().setTrueColor(color);
	auto spDim = std::make_unique<Mx2dCustomAlignedDim>(p1, p2, curPoint, textHeight);
	spDim->worldDraw(pWorldDraw);
}

void MxCADCommand::worldDrawLinearDim(Mx2dCustomLinearDim* pLinearDim, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw)
{
	if (!pData->isValidCurPoint) return;
	McGePoint3d curPoint = pData->curPoint;
	McCmColor color;
	color.setRGB(230, 81, 0);
	pWorldDraw->subEntityTraits().setTrueColor(color);
	pLinearDim->setDimPt(curPoint);
	pLinearDim->worldDraw(pWorldDraw);
}

void MxCADCommand::worldDrawContinuousMeasurement(const Mx2d::PLVertexList& pts, double textHeight, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw)
{
	if (!pData->isValidCurPoint) return;
	McGePoint3d curPoint = pData->curPoint;
	if (pts.length() >= 2)
	{
		McCmColor color;
		color.setRGB(230, 81, 0);
		pWorldDraw->subEntityTraits().setTrueColor(color);
		auto spMeasurement = std::make_unique<Mx2dCustomContinuousMeasurement>(pts, curPoint, textHeight);
		spMeasurement->worldDraw(pWorldDraw);
	}
}

void MxCADCommand::worldDrawSegmentLengths(Mx2dCustomContinuousMeasurement* pContinuousMeasurement, double textHeight, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw)
{
	if (pContinuousMeasurement)
	{

		Mx2d::PLVertexList arr = pContinuousMeasurement->points();
		for (int i = 0; i < arr.length() - 1; i++)
		{
			double bulge = arr[i].bulge;
			McGePoint3d startPt = arr[i].pt;
			McGePoint3d endPt = arr[i + 1].pt;
			McGeVector3d dir = (endPt - startPt).normal();

			double rotAngle = 0.0;
			int region = 1;
			if (dir.x > 0 && dir.y > 0 || dir.x < 0 && dir.y < 0) // 1,3
			{
				McGeVector3d dir1(fabs(dir.x), fabs(dir.y), 0);

				rotAngle = dir1.angleTo(McGeVector3d::kXAxis);
				if (dir.x > 0 && dir.y > 0)
				{
					region = 1;
				}
				else
				{
					region = 3;
				}
			}
			else // 2,4
			{
				McGeVector3d dir4(fabs(dir.x), -fabs(dir.y), 0);
				rotAngle = -dir4.angleTo(McGeVector3d::kXAxis);
				if (dir.x < 0 && dir.y > 0)
				{
					region = 2;
				}
				else
				{
					region = 4;
				}
			}

			McGePoint3d midPt;

			double length = 0.0;
			McDbObjectId textStyle = Mx::mcdbCurDwg()->textstyle();
			auto spText = std::make_unique<McDbText>();
			spText->setHeight(textHeight);
			spText->setTextStyle(textStyle);

			if (bulge != 0.0) // arc
			{
				McGePoint3d center;
				double radius = 0.0;
				double angle = 0.0;
				Mx2d::calcArc(startPt, endPt, bulge, center, radius, angle);
				length = radius * angle;
				double arcMidRot = angle / 2.0;
				McGeVector3d vecArcMid(startPt - center);
				if (bulge < 0.0) // clockwise
				{
					arcMidRot = -arcMidRot;
				}
				vecArcMid.rotateBy(arcMidRot, McGeVector3d::kZAxis);
				midPt = center + vecArcMid;
			}
			else // line segment
			{
				length = startPt.distanceTo(endPt);

				midPt = McGePoint3d((startPt.x + endPt.x) / 2.0, (startPt.y + endPt.y) / 2.0, 0);
			}
			QString str = QString::number(length, 'f', 2);
			spText->setTextString(str.toLocal8Bit().constData());
			McDbExtents ext;
			spText->getGeomExtents(ext, false);
			McGePoint3d minPt = ext.minPoint();
			McGePoint3d maxPt = ext.maxPoint();
			McGePoint3d mtAnchorPt((minPt.x + maxPt.x) / 2.0, maxPt.y + textHeight / 4.0, 0);
			McGePoint3d mbAnchorPt((minPt.x + maxPt.x) / 2.0, minPt.y - textHeight / 4.0, 0);

			if (region == 1 || region == 4)
			{
				spText->transformBy(McGeMatrix3d::rotation(rotAngle, McGeVector3d::kZAxis, mbAnchorPt));
				spText->transformBy(McGeMatrix3d::translation(midPt - mbAnchorPt));
			}
			else
			{
				spText->transformBy(McGeMatrix3d::rotation(rotAngle, McGeVector3d::kZAxis, mtAnchorPt));
				spText->transformBy(McGeMatrix3d::translation(midPt - mtAnchorPt));
			}
			McCmColor color;
			color.setRGB(230, 81, 0);
			pWorldDraw->subEntityTraits().setTrueColor(color);
			spText->worldDraw(pWorldDraw);
		}

	}

}

void MxCADCommand::worldDrawRadiusDim(Mx2dCustomRadiusDim* pRadiusDim, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw)
{
	if (!pData->isValidCurPoint) return;
	McGePoint3d curPoint = pData->curPoint;
	McCmColor color;
	color.setRGB(230, 81, 0);
	pWorldDraw->subEntityTraits().setTrueColor(color);
	pRadiusDim->setDimPt(curPoint);
	double scaled = Mx2d::getViewToDocScaleRatio();
	pRadiusDim->setTextHeight(25.0 / scaled);
	pRadiusDim->worldDraw(pWorldDraw);
}

void MxCADCommand::worldDrawArcLengthDim(const McGePoint3d& startPt, const McGePoint3d& midPt, const McGePoint3d& endPt, double textHeight, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw)
{
	if (!pData->isValidCurPoint) return;
	McGePoint3d curPoint = pData->curPoint;
	McCmColor color;
	color.setRGB(230, 81, 0);
	pWorldDraw->subEntityTraits().setTrueColor(color);
	auto spCustomArcLengthDim = std::make_unique<Mx2dCustomArcLengthDim>(startPt, midPt, endPt, curPoint, textHeight);
	spCustomArcLengthDim->worldDraw(pWorldDraw);
}

void MxCADCommand::worldDrawCircleMeasurement(const McGePoint3d& centerPt, double radius, double textHeight, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw)
{
	if (!pData->isValidCurPoint) return;
	McGePoint3d curPoint = pData->curPoint;
	McCmColor color;
	color.setRGB(230, 81, 0);
	pWorldDraw->subEntityTraits().setTrueColor(color);
	auto spCustomCircleMeasurement = std::make_unique<Mx2dCustomCircleMeasurement>(centerPt, radius, curPoint, textHeight);
	spCustomCircleMeasurement->worldDraw(pWorldDraw);
}

void MxCADCommand::worldDrawAngleMeasurement(const McGePoint3d& l1s, const McGePoint3d& l1e, const McGePoint3d& l2s, const McGePoint3d& l2e, double textHeight, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw)
{
	if (!pData->isValidCurPoint) return;
	McGePoint3d curPoint = pData->curPoint;
	McCmColor color;
	color.setRGB(230, 81, 0);
	pWorldDraw->subEntityTraits().setTrueColor(color);
	auto spCustomAngleMeasurement = std::make_unique<Mx2dCustomAngleMeasurement>(l1s, l1e, l2s, l2e, curPoint, textHeight, true);
	spCustomAngleMeasurement->worldDraw(pWorldDraw);
}

void MxCADCommand::worldDrawArcPolyArea(const Mx2d::PLVertexList& pts, double textHeight, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw)
{
	if (!pData->isValidCurPoint) return;
	McGePoint3d curPoint = pData->curPoint;
	if (pts.length() >= 2)
	{
		McCmColor color;
		color.setRGB(230, 81, 0);
		pWorldDraw->subEntityTraits().setTrueColor(color);
		auto spCustomArcPolyArea = std::make_unique<Mx2dCustomArcPolyArea>(pts, curPoint, textHeight);
		spCustomArcPolyArea->worldDraw(pWorldDraw);
	}
}

void MxCADCommand::worldDrawHatchArea2(const Mx2d::HatchPLList& polyArray, double textHeight, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw)
{
	if (!pData->isValidCurPoint) return;
	McGePoint3d curPoint = pData->curPoint;
	McCmColor color;
	color.setRGB(230, 81, 0);
	pWorldDraw->subEntityTraits().setTrueColor(color);
	auto spCustomHatchArea2 = std::make_unique<Mx2dCustomHatchArea2>(polyArray, curPoint, textHeight);
	spCustomHatchArea2->worldDraw(pWorldDraw);
}

void MxCADCommand::worldDrawBatchMeasure(const Mx2d::CurveShapeList& curveShapes, double textHeight, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw, bool isDynamicDrawing)
{
	if (!pData->isValidCurPoint) return;
	McGePoint3d curPoint = pData->curPoint;

	auto spCustomBatchMeasurement = std::make_unique<Mx2dCustomBatchMeasurement>(curveShapes, curPoint, textHeight, isDynamicDrawing);

	McCmColor oldColor = pWorldDraw->subEntityTraits().trueColor();

	McCmColor newColor;
	newColor.setRGB(230, 81, 0);
	pWorldDraw->subEntityTraits().setTrueColor(newColor);

	spCustomBatchMeasurement->worldDraw(pWorldDraw);

	pWorldDraw->subEntityTraits().setupForEntity(spCustomBatchMeasurement.get());

	pWorldDraw->subEntityTraits().setTrueColor(oldColor);
}


void MxCADCommand::drawRectByCorner(const McGePoint3d& corner1, const McGePoint3d& corner2, MxColor color, bool isReserve)
{
	double minX = corner1.x < corner2.x ? corner1.x : corner2.x;
	double maxX = corner1.x < corner2.x ? corner2.x : corner1.x;
	double minY = corner1.y < corner2.y ? corner1.y : corner2.y;
	double maxY = corner1.y < corner2.y ? corner2.y : corner1.y;

	McGePoint3d pt1(minX, maxY, 0);
	McGePoint3d pt2(maxX, maxY, 0);
	McGePoint3d pt3(maxX, minY, 0);
	McGePoint3d pt4(minX, minY, 0);
	MxDrawApp::DrawVectorLine(pt1, pt2, color, isReserve);
	MxDrawApp::DrawVectorLine(pt2, pt3, color, isReserve);
	MxDrawApp::DrawVectorLine(pt3, pt4, color, isReserve);
	MxDrawApp::DrawVectorLine(pt4, pt1, color, isReserve);
}

double MxCADCommand::cross(const McGePoint3d& a, const McGePoint3d& b, const McGePoint3d& c)
{
	return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

// Check if point c lies on the line segment formed by points a and b
bool MxCADCommand::isPointOnSegment(const McGePoint3d& a, const McGePoint3d& b, const McGePoint3d& c)
{
	// Check if point c is within the bounding box of a and b (with epsilon for floating-point tolerance)
	if (c.x < qMin(a.x, b.x) - 1e-8 || c.x > qMax(a.x, b.x) + 1e-8)
		return false;
	if (c.y < qMin(a.y, b.y) - 1e-8 || c.y > qMax(a.y, b.y) + 1e-8)
		return false;

	// Check if points a, b, c are collinear (cross product equals 0 within tolerance)
	return qFabs(cross(a, b, c)) < 1e-8;
}

// Check if two line segments (a-b and c-d) intersect
bool MxCADCommand::segmentsIntersect(const McGePoint3d& a, const McGePoint3d& b, const McGePoint3d& c, const McGePoint3d& d)
{
	// Calculate cross products for orientation checks
	double c1 = cross(a, b, c);
	double c2 = cross(a, b, d);
	double c3 = cross(c, d, a);
	double c4 = cross(c, d, b);

	// Straddle test: endpoints of each segment lie on opposite sides of the other segment
	if (((c1 > 1e-8 && c2 < -1e-8) || (c1 < -1e-8 && c2 > 1e-8)) &&
		((c3 > 1e-8 && c4 < -1e-8) || (c3 < -1e-8 && c4 > 1e-8))) {
		return true;
	}

	// Handle collinear case: check if endpoints lie on the other segment (exclude non-overlapping)
	if (c1 == 0 && !isPointOnSegment(a, b, c)) return false;
	if (c2 == 0 && !isPointOnSegment(a, b, d)) return false;
	if (c3 == 0 && !isPointOnSegment(c, d, a)) return false;
	if (c4 == 0 && !isPointOnSegment(c, d, b)) return false;

	// Exclude endpoint-only contact (normal vertices of adjacent edges)
	// Check if collinear points are on the segment but not exactly at the endpoints
	if (isPointOnSegment(a, b, c) && !(c.x == a.x && c.y == a.y) && !(c.x == b.x && c.y == b.y))
		return true;
	if (isPointOnSegment(a, b, d) && !(d.x == a.x && d.y == a.y) && !(d.x == b.x && d.y == b.y))
		return true;
	if (isPointOnSegment(c, d, a) && !(a.x == c.x && a.y == c.y) && !(a.x == d.x && a.y == d.y))
		return true;
	if (isPointOnSegment(c, d, b) && !(b.x == c.x && b.y == c.y) && !(b.x == d.x && b.y == d.y))
		return true;

	return false;
}

// Check if a polygon (defined by a sequence of vertices) is self-intersecting
bool MxCADCommand::isPolygonSelfIntersecting(const McGePoint3dArray& polygon)
{
	int vertexCount = polygon.size();
	// Polygons with fewer than 4 vertices cannot be self-intersecting (triangle or line)
	if (vertexCount < 4) return false;

	// Iterate through each edge of the polygon
	for (int i = 0; i < vertexCount; ++i) {
		// Current edge: E_i = (V_i, V_{i+1}) - use modulo to wrap around to first vertex
		McGePoint3d a = polygon[i];
		McGePoint3d b = polygon[(i + 1) % vertexCount];

		// Check intersection with non-adjacent subsequent edges (start from i+2 to avoid duplicates/adjacent edges)
		for (int j = i + 2; j < vertexCount; ++j) {
			// Skip adjacent check between last edge and first edge (when i=0 and j=last vertex)
			if (i == 0 && j == vertexCount - 1) continue;

			// Edge to check against: E_j = (V_j, V_{j+1})
			McGePoint3d c = polygon[j];
			McGePoint3d d = polygon[(j + 1) % vertexCount];

			// Return true immediately if any intersecting edge pair is found
			if (segmentsIntersect(a, b, c, d)) {
				return true;
			}
		}
	}

	// No self-intersections found
	return false;
}


McDbExtents MxCADCommand::getPolygonGeomExtents(const McGePoint3dArray& pts)
{
	McDbPolyline pline;
	for (int i = 0; i < pts.length(); i++)
	{
		pline.addVertexAt(pts[i]);
	}
	pline.setClosed(true);
	McDbExtents ext;
	pline.getGeomExtents(ext, false);
	return ext;
}



// Determine the position of a point relative to a polygon using the winding number algorithm
// Return values:
//   -1: Point lies exactly on the polygon boundary (edge or vertex)
//    1: Point is inside the polygon
//    0: Point is outside the polygon
int MxCADCommand::isPointInPolygon(const McGePoint3d& pt, const McGePoint3dArray& pts)
{
	int vertexCount = pts.size();
	// A valid polygon requires at least 3 vertices; return 0 (outside) if not met
	if (vertexCount < 3) return 0;

	int windingNumber = 0;  // Winding number (determines inside/outside status)

	// Iterate through each edge of the polygon
	for (int i = 0; i < vertexCount; ++i) {
		// Get current vertex (a) and next vertex (b) - wrap around to first vertex for last edge
		const McGePoint3d& a = pts[i];
		const McGePoint3d& b = pts[(i + 1) % vertexCount];

		// Check if the point lies exactly on the current edge (a-b)
		if (isPointOnSegment(pt, a, b)) {
			return -1;  // Point is on boundary
		}

		// Calculate vectors from the test point to vertices a and b
		double dx1 = a.x - pt.x;
		double dy1 = a.y - pt.y;
		double dx2 = b.x - pt.x;
		double dy2 = b.y - pt.y;

		// Cross product: Determines the rotation direction of vectors (pt->a) and (pt->b)
		double cross = dx1 * dy2 - dy1 * dx2;

		// Dot product: Determines if vectors (pt->a) and (pt->b) point in opposite directions
		double dot = dx1 * dx2 + dy1 * dy2;

		// Handle collinear case (cross product is zero within floating-point tolerance)
		if (std::fabs(cross) < 1e-8) {
			// Dot product < 0 means vectors point in opposite directions
			if (dot < -1e-8) {
				// Check if the point lies on the extension of edge a-b
				if (qMin(a.x, b.x) - 1e-8 <= pt.x && pt.x <= qMax(a.x, b.x) + 1e-8 &&
					qMin(a.y, b.y) - 1e-8 <= pt.y && pt.y <= qMax(a.y, b.y) + 1e-8) {
					return -1;  // Point is on boundary extension (treated as on polygon)
				}
			}
			continue;  // Skip non-crossing collinear edges
		}

		// Check if the edge crosses the horizontal line passing through the test point
		// (with epsilon to handle floating-point precision issues)
		bool isCrossing = (a.y <= pt.y + 1e-8 && pt.y < b.y + 1e-8) ||
			(b.y <= pt.y + 1e-8 && pt.y < a.y + 1e-8);

		if (isCrossing) {
			// Calculate x-coordinate of the intersection between edge a-b and the horizontal line
			double xIntersect = ((pt.y - a.y) * (b.x - a.x)) / (b.y - a.y) + a.x;

			// Update winding number only if intersection is to the right of the test point
			if (pt.x < xIntersect - 1e-8) {
				if (b.y > a.y + 1e-8)  // Edge crosses upward (counterclockwise direction)
					windingNumber++;
				else  // Edge crosses downward (clockwise direction)
					windingNumber--;
			}
		}
	}

	// Determine final position: non-zero winding number means inside, zero means outside
	return (windingNumber != 0) ? 1 : 0;
}

int MxCADCommand::isEntityInPolygon(const McDbObjectId& id, const McGePoint3dArray& pts)
{
	McDbEntityPointer spEntity(id, McDb::kForRead);
	if (spEntity.openStatus() != Mcad::eOk)
		return 0;
	McDbExtents ext;
	if (spEntity->getGeomExtents(ext, false) != Mcad::eOk)
		return 0;
	McGePoint3d centralPoint((ext.minPoint().x + ext.maxPoint().x) / 2, (ext.minPoint().y + ext.maxPoint().y) / 2, 0.0);
	return isPointInPolygon(centralPoint, pts);
}

void MxCADCommand::getTextEntitiesInPolygon(Mx2d::TextInfoList& result, const McDbObjectIdArray& ids, const McGePoint3dArray& pts, const QString& containStr, bool isFull)
{
	for (int i = 0; i < ids.length(); i++)
	{
		McDbObjectId curId = ids[i];
		McDbObjectPointer<McDbEntity> spEntity(curId, McDb::kForRead);
		if (spEntity.openStatus() != Mcad::eOk)
			continue;

		McDbExtents ext;
		if (spEntity->getGeomExtents(ext, false) != Mcad::eOk)
			continue;
		McGePoint3d centralPoint((ext.minPoint().x + ext.maxPoint().x) / 2, (ext.minPoint().y + ext.maxPoint().y) / 2, 0.0);

		if (isPointInPolygon(centralPoint, pts) == 0)
			continue;

		if (spEntity->isA() == McDbText::desc())
		{
			McDbText* pText = McDbText::cast(spEntity.object());
			QString str = QString::fromLocal8Bit(pText->textString());
			if (isFull)
			{
				if (containStr == str)
				{
					McDbExtents ext;
					pText->getGeomExtents(ext, false);
					McGePoint3d minPt = ext.minPoint();
					McGePoint3d maxPt = ext.maxPoint();
					double minX, minY, maxX, maxY;
					minX = minPt.x; minY = minPt.y; maxX = maxPt.x; maxY = maxPt.y;
					Mx2d::Extents ext2d(minX, minY, maxX, maxY);

					result.append({ str, ext2d });
				}
			}
			else
			{
				if (str.contains(containStr, Qt::CaseInsensitive))
				{
					McDbExtents ext;
					pText->getGeomExtents(ext, false);
					McGePoint3d minPt = ext.minPoint();
					McGePoint3d maxPt = ext.maxPoint();
					double minX, minY, maxX, maxY;
					minX = minPt.x; minY = minPt.y; maxX = maxPt.x; maxY = maxPt.y;
					Mx2d::Extents ext2d(minX, minY, maxX, maxY);

					result.append({ str, ext2d });
				}
			}
		}
		else if (spEntity->isA() == McDbMText::desc())
		{
			McDbMText* pMText = McDbMText::cast(spEntity.object());
			McDbVoidPtrArray explodeMText;
			auto status = pMText->explode(explodeMText);
			if (status != Mcad::eOk)
			{
				continue;
			}
			for (int j = 0; j < explodeMText.length(); j++)
			{
				McDbText* pText = static_cast<McDbText*>(explodeMText[j]);
				QString str = QString::fromLocal8Bit(pText->textString());
				if (isFull)
				{
					if (containStr == str)
					{
						McDbExtents ext;
						pText->getGeomExtents(ext, false);
						McGePoint3d minPt = ext.minPoint();
						McGePoint3d maxPt = ext.maxPoint();
						double minX, minY, maxX, maxY;
						minX = minPt.x; minY = minPt.y; maxX = maxPt.x; maxY = maxPt.y;
						Mx2d::Extents ext2d(minX, minY, maxX, maxY);

						result.append({ str, ext2d });
					}
				}
				else
				{
					if (str.contains(containStr, Qt::CaseInsensitive))
					{
						McDbExtents ext;
						pText->getGeomExtents(ext, false);
						McGePoint3d minPt = ext.minPoint();
						McGePoint3d maxPt = ext.maxPoint();
						double minX, minY, maxX, maxY;
						minX = minPt.x; minY = minPt.y; maxX = maxPt.x; maxY = maxPt.y;
						Mx2d::Extents ext2d(minX, minY, maxX, maxY);

						result.append({ str, ext2d });
					}
				}
				delete pText;
			}
			explodeMText.clear();
		}
	}
}

McDbObjectIdArray MxCADCommand::getTextInPolygon(const McDbObjectIdArray& ids, const McGePoint3dArray& pts)
{
	McDbObjectIdArray result;
	for (int i = 0; i < ids.length(); i++)
	{
		McDbObjectId curId = ids[i];
		McDbObjectPointer<McDbEntity> spEntity(curId, McDb::kForRead);
		if (spEntity.openStatus() != Mcad::eOk)
			continue;
		McDbExtents ext;
		if (spEntity->getGeomExtents(ext, false) != Mcad::eOk)
			continue;

		McGePoint3d centralPoint((ext.minPoint().x + ext.maxPoint().x) / 2, (ext.minPoint().y + ext.maxPoint().y) / 2, 0.0);

		if (isPointInPolygon(centralPoint, pts) == 0)
			continue;
		if (spEntity->isA() == McDbText::desc() || spEntity->isA() == McDbMText::desc())
		{
			result.append(curId);
		}
	}
	return result;
}

void MxCADCommand::deepTextInBlockRef(Mx2d::TextInfoList& result, const McDbBlockReference& blockRef, const McDbObjectId& curId, const QString& containStr, bool isFull)
{
	McDbVoidPtrArray explodeBlkRef;
	if (blockRef.explode(explodeBlkRef) != Mcad::eOk)
		return;
	for (int i = 0; i < explodeBlkRef.length(); i++)
	{
		McDbEntity* pEntity = static_cast<McDbEntity*>(explodeBlkRef[i]);
		if (!pEntity) continue;
		if (pEntity->isA() == McDbText::desc())
		{
			McDbText* pText = McDbText::cast(pEntity);
			QString str = QString::fromLocal8Bit(pText->textString());
			if (isFull)
			{
				if (str == containStr)
				{
					McDbExtents ext;
					pText->getGeomExtents(ext, false);
					McGePoint3d minPt = ext.minPoint();
					McGePoint3d maxPt = ext.maxPoint();
					double minX, minY, maxX, maxY;
					minX = minPt.x; minY = minPt.y; maxX = maxPt.x; maxY = maxPt.y;
					Mx2d::Extents ext2d(minX, minY, maxX, maxY);

					result.append({ str, ext2d });
				}
			}
			else
			{
				if (str.contains(containStr, Qt::CaseInsensitive))
				{
					McDbExtents ext;
					pText->getGeomExtents(ext, false);
					McGePoint3d minPt = ext.minPoint();
					McGePoint3d maxPt = ext.maxPoint();
					double minX, minY, maxX, maxY;
					minX = minPt.x; minY = minPt.y; maxX = maxPt.x; maxY = maxPt.y;
					Mx2d::Extents ext2d(minX, minY, maxX, maxY);

					result.append({ str, ext2d });
				}
			}
		}
		else if (pEntity->isA() == McDbMText::desc())
		{
			McDbMText* pMText = McDbMText::cast(pEntity);
			McDbVoidPtrArray resultExplode;
			if (pMText->explode(resultExplode) != Mcad::eOk)
				continue;
			for (int j = 0; j < resultExplode.length(); j++)
			{
				McDbText* pText = static_cast<McDbText*>(resultExplode[j]);
				QString str = QString::fromLocal8Bit(pText->textString());
				if (isFull)
				{
					if (str == containStr)
					{
						McDbExtents ext;
						pText->getGeomExtents(ext, false);
						McGePoint3d minPt = ext.minPoint();
						McGePoint3d maxPt = ext.maxPoint();
						double minX, minY, maxX, maxY;
						minX = minPt.x; minY = minPt.y; maxX = maxPt.x; maxY = maxPt.y;
						Mx2d::Extents ext2d(minX, minY, maxX, maxY);

						result.append({ str, ext2d });
					}
				}
				else
				{
					if (str.contains(containStr, Qt::CaseInsensitive))
					{
						McDbExtents ext;
						pText->getGeomExtents(ext, false);
						McGePoint3d minPt = ext.minPoint();
						McGePoint3d maxPt = ext.maxPoint();
						double minX, minY, maxX, maxY;
						minX = minPt.x; minY = minPt.y; maxX = maxPt.x; maxY = maxPt.y;
						Mx2d::Extents ext2d(minX, minY, maxX, maxY);

						result.append({ str, ext2d });
					}
				}
				delete pText;
			}
			resultExplode.clear();
		}
		else if (pEntity->isA() == McDbBlockReference::desc())
		{
			McDbBlockReference* pBlkRef = McDbBlockReference::cast(pEntity);
			deepTextInBlockRef(result, *pBlkRef, curId, containStr, isFull);
		}
		delete pEntity;
	}
}


void MxCADCommand::Mx_Test()
{

}


void MxCADCommand::Mx_TextSearch()
{
	struct resbuf* pRb = Mx::mcedGetArgs();
	if (!pRb || pRb->restype != RTSTR || !pRb->rbnext || pRb->rbnext->restype != RTSHORT) return;

	QString qstr = QString::fromLocal8Bit(pRb->resval.rstring);
	short iIsFull = pRb->rbnext->resval.rint;
	bool isFull = false;
	if (iIsFull == 1) isFull = true;


	MrxDbgSelSet ss;
	MrxDbgRbList spFilter = Mx::mcutBuildList(RTDXF0, "TEXT,MTEXT,INSERT", 0);
	if (ss.allSelect(spFilter.data()) != MrxDbgSelSet::kSelected)
		return;


	McDbObjectIdArray aryId;
	ss.asArray(aryId);


	Mx2d::TextInfoList result;

	textFilter(result, aryId, qstr, isFull);

	emit Mx2dSignalTransfer::getInstance().signalTextSearched(result);
}

void MxCADCommand::Mx_TextSearchRect()
{
	struct resbuf* pRb = Mx::mcedGetArgs();
	if (!pRb || pRb->restype != RTSTR || !pRb->rbnext || pRb->rbnext->restype != RTSHORT) return;

	QString qstr = QString::fromLocal8Bit(pRb->resval.rstring);
	short iIsFull = pRb->rbnext->resval.rint;
	bool isFull = (iIsFull == 1);

	// Interactively select rectangular area
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select first corner of rectangular area"));
	MrxDbgUiPrPoint getCorner1(QCoreApplication::translate("MxCADCommand", "Select first corner of rectangular area").toStdString().c_str());
	MrxDbgUiPrBase::Status ret = getCorner1.go();
	if (ret != MrxDbgUiPrBase::kOk)
		return;
	McGePoint3d corner1 = getCorner1.value();

	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select second corner of rectangular area"));
	MrxDbgUiPrPoint getCorner2(QCoreApplication::translate("MxCADCommand", "Select second corner of rectangular area").toStdString().c_str());
	getCorner2.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
		worldDrawRect(corner1, pData, pWorldDraw);
		});
	ret = getCorner2.go();
	if (ret != MrxDbgUiPrBase::kOk)
		return;
	McGePoint3d corner2 = getCorner2.value();

	HIDE_PROMPT_MESSAGE;

	MrxDbgSelSet ss;
	MrxDbgRbList spFilter = Mx::mcutBuildList(RTDXF0, "TEXT,MTEXT,INSERT", 0);
	if (ss.crossingSelect(corner1, corner2, spFilter.data()) != MrxDbgSelSet::kSelected)
		return;

	McDbObjectIdArray aryId;
	ss.asArray(aryId);

	Mx2d::TextInfoList result;
	textFilter(result, aryId, qstr, isFull);

	MXAPP.CallMain([=]() {
		emit Mx2dSignalTransfer::getInstance().signalTextSearched(result);
		});
}

void MxCADCommand::Mx_TextSearchPolygon()
{
	// Get input arguments from CAD command line
	struct resbuf* pRb = Mx::mcedGetArgs();
	// Validate input: first arg must be string (search text), second must be short (full match flag)
	if (!pRb || pRb->restype != RTSTR || !pRb->rbnext || pRb->rbnext->restype != RTSHORT)
		return;

	// Convert GB2312 encoded search text to QString
	QString qstr = QString::fromLocal8Bit(pRb->resval.rstring);
	// Get full match flag (1 = full match, 0 = partial match)
	short iIsFull = pRb->rbnext->resval.rint;
	bool isFullMatch = (iIsFull == 1); // Simplify boolean conversion


	// Prompt user to select polygon start point
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select start point"));
	MrxDbgUiPrPoint getStartPoint(QCoreApplication::translate("MxCADCommand", "Select start point").toStdString().c_str());
	MrxDbgUiPrBase::Status retStatus = getStartPoint.go();
	// Exit if user cancels or fails to select point
	if (retStatus != MrxDbgUiPrBase::kOk)
		return;
	McGePoint3d startPoint = getStartPoint.value();

	// Array to store polygon vertices (starts with the first selected point)
	McGePoint3dArray polygonPoints;
	polygonPoints.append(startPoint);

	// Prompt user to select subsequent polygon vertices
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select next point"));
	while (true)
	{
		MrxDbgUiPrPoint getNextPoint(QCoreApplication::translate("MxCADCommand", "Select next point").toStdString().c_str());
		// Enable dynamic preview of the polygon while selecting points
		getNextPoint.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
			worldDrawPolygon(polygonPoints, pData, pWorldDraw);
			});
		retStatus = getNextPoint.go();

		// Handle user input status
		if (retStatus == MrxDbgUiPrBase::kOk)
		{
			// Add selected point to polygon vertex array and continue selecting
			polygonPoints.append(getNextPoint.value());
			continue;
		}
		else if (retStatus == MrxDbgUiPrBase::kCancel)
		{
			// Hide prompt and exit if user cancels the operation
			HIDE_PROMPT_MESSAGE;
			return;
		}
		else if (retStatus == MrxDbgUiPrBase::kNone)
		{
			// Finish polygon selection when user click RightMouseKey (no more points)
			HIDE_PROMPT_MESSAGE;
			break;
		}
	}

	// Check if the polygon is self-intersecting (invalid for region selection)
	if (isPolygonSelfIntersecting(polygonPoints))
	{
		// Show information message box and exit
		MXAPP.CallMain([=]() {
			emit MxSignalTransfer::getInstance().signalMessageBoxInformation(
				MxUtils::gCurrentTab,
				QCoreApplication::translate("MxCADCommand", "Self-intersecting polygons are not allowed. Please select again.")
			);
			});
		return;
	}

	// Calculate the axis-aligned bounding box (AABB) of the polygon
	McDbExtents polygonExtents = getPolygonGeomExtents(polygonPoints);
	MrxDbgSelSet selectionSet;
	// Create filter to select only TEXT, MTEXT, and INSERT entities
	MrxDbgRbList entityFilter = Mx::mcutBuildList(RTDXF0, "TEXT,MTEXT,INSERT", 0);

	// First select entities within the polygon's bounding box to narrow down candidates
	if (selectionSet.crossingSelect(
		polygonExtents.minPoint(),
		polygonExtents.maxPoint(),
		entityFilter.data()
	) != MrxDbgSelSet::kSelected)
		return;

	// Convert selection set to object ID array for further processing
	McDbObjectIdArray entityIds;
	selectionSet.asArray(entityIds);

	// Store final search results: (matched text content, entity 2D extents)
	Mx2d::TextInfoList searchResults;
	// Filter entities that are inside the polygon and match the search text
	getTextEntitiesInPolygon(searchResults, entityIds, polygonPoints, qstr, isFullMatch);

	// Trigger signal to pass results to main thread (UI update)
	MXAPP.CallMain([=]() {
		emit Mx2dSignalTransfer::getInstance().signalTextSearched(searchResults);
		});
}

void MxCADCommand::Mx_GetRectArea()
{
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select first corner of rectangular area"));
	MrxDbgUiPrPoint getCorner1(QCoreApplication::translate("MxCADCommand", "Select first corner of rectangular area").toStdString().c_str());
	MrxDbgUiPrBase::Status ret = getCorner1.go();
	if (ret != MrxDbgUiPrBase::kOk)
	{
		HIDE_PROMPT_MESSAGE; 
		return;
	}
	McGePoint3d corner1 = getCorner1.value();

	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select second corner of rectangular area"));
	MrxDbgUiPrPoint getCorner2(QCoreApplication::translate("MxCADCommand", "Select second corner of rectangular area").toStdString().c_str());
	getCorner2.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
		worldDrawRect(corner1, pData, pWorldDraw);
		});
	ret = getCorner2.go();
	if (ret != MrxDbgUiPrBase::kOk)
	{
		HIDE_PROMPT_MESSAGE;
		return;
	}
	McGePoint3d corner2 = getCorner2.value();

	HIDE_PROMPT_MESSAGE;
	Mx2d::Rect2D result{ {corner1.x, corner1.y}, {corner2.x, corner2.y} };
	MXAPP.CallMain([=]() {
		emit Mx2dSignalTransfer::getInstance().signalTextSearchRect(MxUtils::gCurrentTab, result);
		});
}

void MxCADCommand::Mx_GetPolyArea()
{
	// Prompt user to select polygon start point
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select start point"));
	MrxDbgUiPrPoint getStartPoint(QCoreApplication::translate("MxCADCommand", "Select start point").toStdString().c_str());
	MrxDbgUiPrBase::Status retStatus = getStartPoint.go();
	// Exit if user cancels or fails to select point
	if (retStatus != MrxDbgUiPrBase::kOk)
	{
		HIDE_PROMPT_MESSAGE;
		return;
	}
	McGePoint3d startPoint = getStartPoint.value();

	// Array to store polygon vertices (starts with the first selected point)
	McGePoint3dArray polygonPoints;
	polygonPoints.append(startPoint);

	// Prompt user to select subsequent polygon vertices
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select next point"));
	while (true)
	{
		MrxDbgUiPrPoint getNextPoint(QCoreApplication::translate("MxCADCommand", "Select next point").toStdString().c_str());
		// Enable dynamic preview of the polygon while selecting points
		getNextPoint.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
			worldDrawPolygon(polygonPoints, pData, pWorldDraw);
			});
		retStatus = getNextPoint.go();

		// Handle user input status
		if (retStatus == MrxDbgUiPrBase::kOk)
		{
			// Add selected point to polygon vertex array and continue selecting
			polygonPoints.append(getNextPoint.value());
			continue;
		}
		else if (retStatus == MrxDbgUiPrBase::kCancel)
		{
			// Hide prompt and exit if user cancels the operation
			HIDE_PROMPT_MESSAGE;
			return;
		}
		else if (retStatus == MrxDbgUiPrBase::kNone)
		{
			// Finish polygon selection when user click RightMouseKey (no more points)
			HIDE_PROMPT_MESSAGE;
			break;
		}
	}

	if (polygonPoints.size() < 3)
	{
		return;
	}

	Mx2d::Point2DList polygonPoints2D;
	for (int i = 0; i < polygonPoints.size(); i++)
	{
		polygonPoints2D.append(Mx2d::Point2D{ polygonPoints[i].x, polygonPoints[i].y });
	}

	MXAPP.CallMain([=]() {
		emit Mx2dSignalTransfer::getInstance().signalTextSearchPoly(MxUtils::gCurrentTab, polygonPoints2D);
		});
}

void MxCADCommand::Mx_DrawLine()
{
	// Continuous line drawing loop (repeat until user cancels)
	while (true)
	{
		// Prompt user to select line start point
		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select start point"));
		MrxDbgUiPrPoint getStartPoint(QCoreApplication::translate("MxCADCommand", "Select start point").toStdString().c_str());
		MrxDbgUiPrBase::Status retStatus = getStartPoint.go();

		// Exit command if user cancels or fails to select start point
		if (retStatus != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}
		McGePoint3d startPoint = getStartPoint.value();

		// Prompt user to select line end point
		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select end point"));
		MrxDbgUiPrPoint getEndPoint(QCoreApplication::translate("MxCADCommand", "Select end point").toStdString().c_str());
		// Dynamic rubber band preview
		getEndPoint.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
			worldDrawLine(startPoint, pData, pWorldDraw);
			});
		retStatus = getEndPoint.go();

		// Exit command if user cancels or fails to select end point
		if (retStatus != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}
		McGePoint3d endPoint = getEndPoint.value();

		// Hide command prompt after selecting both points
		HIDE_PROMPT_MESSAGE;

		// Draw the line in main thread (ensure UI thread safety)
		MXAPP.CallMain([=]() {
			auto* pAnno = new Mx2dCustomLine(startPoint, endPoint, 1.0);
			McDbObjectId id = Mx2d::addEntityToAnnotationLayerAndClose(pAnno);
			emit Mx2dSignalTransfer::getInstance().signalAddAnnotation(MxUtils::gCurrentTab, id);
			});
	}
}



void MxCADCommand::Mx_MoveViewCenterTo()
{
	MxDrawApp::ClearVectorLine();

	struct resbuf* pRb = Mx::mcedGetArgs();
	if (!pRb || pRb->restype != RTREAL ||
		!pRb->rbnext || pRb->rbnext->restype != RTREAL ||
		!pRb->rbnext->rbnext || pRb->rbnext->rbnext->restype != RTREAL ||
		!pRb->rbnext->rbnext->rbnext || pRb->rbnext->rbnext->rbnext->restype != RTREAL) return;

	double minX = pRb->resval.rreal;
	double minY = pRb->rbnext->resval.rreal;
	double maxX = pRb->rbnext->rbnext->resval.rreal;
	double maxY = pRb->rbnext->rbnext->rbnext->resval.rreal;

	Mx2d::Extents ext2d(minX, minY, maxX, maxY);

	double width = maxX - minX;
	double height = maxY - minY;
	McGePoint3d centerPoint(ext2d.centerX(), ext2d.centerY(), 0);

	MxDrawApp::ZoomW(centerPoint.x - width * 1, centerPoint.y - height * 6, centerPoint.x + width * 1, centerPoint.y + height * 6);
	drawRectByCorner(McGePoint3d(minX, minY, 0), McGePoint3d(maxX, maxY, 0), 0xE65100, true);
	MxDrawApp::UpdateDisplay();

}

void MxCADCommand::Mx_ZoomAll()
{
	MxDrawApp::ZoomE();
	MxDrawApp::UpdateDisplay();
}

void MxCADCommand::Mx_ZoomWin()
{
	// Get rectangular area interactively
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select first corner of zoom window"));
	MrxDbgUiPrPoint getCorner1(QCoreApplication::translate("MxCADCommand", "Select first corner of zoom window").toStdString().c_str());
	MrxDbgUiPrBase::Status ret = getCorner1.go();
	if (ret != MrxDbgUiPrBase::kOk)
		return;
	McGePoint3d corner1 = getCorner1.value();

	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select second corner of zoom window"));
	MrxDbgUiPrPoint getCorner2(QCoreApplication::translate("MxCADCommand", "Select second corner of zoom window").toStdString().c_str());
	getCorner2.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
		worldDrawRect(corner1, pData, pWorldDraw);
		});
	ret = getCorner2.go();
	if (ret != MrxDbgUiPrBase::kOk)
		return;
	McGePoint3d corner2 = getCorner2.value();

	HIDE_PROMPT_MESSAGE;

	// Calculate bounding box of selected area
	double minX = qMin(corner1.x, corner2.x);
	double minY = qMin(corner1.y, corner2.y);
	double maxX = qMax(corner1.x, corner2.x);
	double maxY = qMax(corner1.y, corner2.y);

	// Perform zoom and refresh display
	MXAPP.CallMain([=]() {
		MxDrawApp::ZoomW(minX, minY, maxX, maxY);
		MxDrawApp::UpdateDisplay();
		});
}


// Get all layer information
void MxCADCommand::Mx_GetAllLayer()
{
	McDbLayerTablePointer spLayerTable(Mx::mcdbCurDwg(), McDb::kForRead);
	if (spLayerTable.openStatus() != Mcad::eOk)
		return;

	McDbLayerTableIterator* pLayerIter = nullptr;
	spLayerTable->newIterator(pLayerIter);
	if (pLayerIter)
	{
		Mx2d::LayerInfo layerInfo;

		for (pLayerIter->start(); !pLayerIter->done(); pLayerIter->step())
		{
			McDbLayerTableRecord* pRecord = nullptr;
			pLayerIter->getRecord(pRecord, McDb::kForWrite);
			if (pRecord)
			{
				MxString sName;
				pRecord->getName(sName);
				QString qsName = QString::fromLocal8Bit(sName.getString().c_str());

				// Skip internal annotation layer
				if (qsName == "MxCADAnnotationLayer") {
					pRecord->close();
					continue;
				}

				// Collect layer data: ID, visibility, name, color
				layerInfo.append({ pRecord->objectId(), pRecord->isOff(), qsName, pRecord->color() });
				pRecord->close();
			}
		}

		// Send layer info to main thread
		MXAPP.CallMain([=]() {
			emit Mx2dSignalTransfer::getInstance().signalLayerInformation(layerInfo);
			});

		delete pLayerIter;
	}
}



void MxCADCommand::Mx_SetSomeLayersOffStatus()
{
	QList<McDbObjectId> ids;
	QList<bool> offStatus;

	struct resbuf* first = Mx::mcedGetArgs();
	if (!first) return;
	struct resbuf* second = first->rbnext;
	if (!second) return;
	McDbObjectId id;
	id.setFromOldId(first->resval.objId);
	ids.append(id);
	offStatus.append(second->resval.rint == 1 ? true : false);

	while (second->rbnext)
	{
		first = second->rbnext;
		second = first->rbnext;
		McDbObjectId id;
		id.setFromOldId(first->resval.objId);
		ids.append(id);
		offStatus.append(second->resval.rint == 1 ? true : false);
	}

	for (int i = 0; i < ids.length(); ++i)
	{
		McDbObjectId id = ids[i];
		bool status = offStatus[i];
		if (!id.isValid()) continue;
		McDbLayerTableRecordPointer spLayerRecord(id, McDb::kForWrite);
		if (spLayerRecord.openStatus() != Mcad::eOk) continue;

		spLayerRecord->setIsOff(status);
	}
	emit Mx2dSignalTransfer::getInstance().signalLayersOffStatusSetted(MxUtils::gCurrentTab);
}


void MxCADCommand::Mx_SetSelectedLayersOff()
{
	McDbObjectIdArray layerIds;
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Click an entity to select its layer"));

	while (true)
	{
		McDbObjectId entId;
		McGePoint3d pickPoint;
		// Select entity to get its layer; exit loop if selection is canceled
		if (MrxDbgUtils::selectEnt(QCoreApplication::translate("MxCADCommand", "Click an entity to select its layer").toStdString().c_str(), entId, pickPoint) != RTNORM)
		{
			HIDE_PROMPT_MESSAGE;
			break;
		}

		// Get the layer ID of the selected entity
		McDbEntityPointer spEntity(entId, McDb::kForRead);
		McDbObjectId layerId = spEntity->layerId();

		layerIds.append(layerId);
	}
	HIDE_PROMPT_MESSAGE;

	// Turn off all selected layers (skip internal annotation layer)
	for (int i = 0; i < layerIds.length(); ++i)
	{
		McDbLayerTableRecordPointer spLayerRecord(layerIds[i], McDb::kForWrite);
		if (spLayerRecord.openStatus() != Mcad::eOk) continue;

		MxString layerName;
		spLayerRecord->getName(layerName);
		QString qsLayerName = QString::fromLocal8Bit(layerName.getString().c_str());

		if (qsLayerName == "MxCADAnnotationLayer") continue;
		spLayerRecord->setIsOff(true);
	}

	// Update display and notify UI of layer status change
	MXAPP.CallMain([=]() {
		emit Mx2dSignalTransfer::getInstance().signalLayersOffStatusSetted(MxUtils::gCurrentTab);
		});
}

void MxCADCommand::Mx_SetNotSelectedLayersOff()
{
	McDbObjectIdArray selectedLayerIds;
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Click an entity to select its layer"));

	// Loop to select entities and collect their layer IDs
	while (true)
	{
		McDbObjectId entId;
		McGePoint3d pickPoint;
		if (MrxDbgUtils::selectEnt(QCoreApplication::translate("MxCADCommand", "Click an entity to select its layer").toStdString().c_str(), entId, pickPoint) != RTNORM)
		{
			HIDE_PROMPT_MESSAGE;
			break;
		}

		McDbEntityPointer spEntity(entId, McDb::kForRead);
		McDbObjectId layerId = spEntity->layerId();
		selectedLayerIds.append(layerId);
	}
	HIDE_PROMPT_MESSAGE;

	// Open layer table to get all layers
	McDbLayerTablePointer spLayerTable(Mx::mcdbCurDwg(), McDb::kForRead);
	if (spLayerTable.openStatus() != Mcad::eOk)
		return;

	// Iterate layer table and collect all layer IDs
	McDbLayerTableIterator* pLayerIter = nullptr;
	spLayerTable->newIterator(pLayerIter);
	//std::unique_ptr<McDbLayerTableIterator> spIter(pLayerIter); // why can't use unique_ptr?
	McDbObjectIdArray allLayerIds;

	if (pLayerIter)
	{
		for (pLayerIter->start(); !pLayerIter->done(); pLayerIter->step())
		{
			McDbLayerTableRecord* pRecord = nullptr;
			pLayerIter->getRecord(pRecord, McDb::kForRead);
			if (pRecord)
			{
				allLayerIds.append(pRecord->objectId());
				pRecord->close();
			}
		}
		delete pLayerIter;
		pLayerIter = nullptr;
	}

	// Lambda: Get layers from arr1 that are NOT in arr2
	auto excludeSubset = [](const McDbObjectIdArray& arr1, const McDbObjectIdArray& arr2) {
		McDbObjectIdArray result;
		for (int i = 0; i < arr1.length(); ++i)
		{
			const McDbObjectId& id = arr1[i];
			bool existsInArr2 = false;
			for (int j = 0; j < arr2.length(); ++j)
			{
				if (id == arr2[j])
				{
					existsInArr2 = true;
					break;
				}
			}
			if (!existsInArr2)
				result.append(id);
		}
		return result;
		};

	// Get layers to turn off (all layers except selected ones)
	McDbObjectIdArray layersToTurnOff = excludeSubset(allLayerIds, selectedLayerIds);

	// Turn off target layers
	for (int i = 0; i < layersToTurnOff.length(); ++i)
	{
		McDbLayerTableRecordPointer spLayerRecord(layersToTurnOff[i], McDb::kForWrite);
		if (spLayerRecord.openStatus() != Mcad::eOk) continue;

		MxString layerName;
		spLayerRecord->getName(layerName);
		QString qsLayerName = QString::fromLocal8Bit(layerName.getString().c_str());

		if (qsLayerName == "MxCADAnnotationLayer") continue;

		spLayerRecord->setIsOff(true);
	}

	// Update display and notify UI of layer status change
	MXAPP.CallMain([=]() {
		emit Mx2dSignalTransfer::getInstance().signalLayersOffStatusSetted(MxUtils::gCurrentTab);
		});
}

void MxCADCommand::Mx_ExtractTextRect()
{
	// Get rectangular area interactively
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select first corner of rectangular area"));
	MrxDbgUiPrPoint getCorner1(QCoreApplication::translate("MxCADCommand", "Select first corner of rectangular area").toStdString().c_str());
	MrxDbgUiPrBase::Status ret = getCorner1.go();
	if (ret != MrxDbgUiPrBase::kOk)
		return;
	McGePoint3d corner1 = getCorner1.value();

	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select second corner of rectangular area"));
	MrxDbgUiPrPoint getCorner2(QCoreApplication::translate("MxCADCommand", "Select second corner of rectangular area").toStdString().c_str());
	getCorner2.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
		worldDrawRect(corner1, pData, pWorldDraw);
		});
	ret = getCorner2.go();
	if (ret != MrxDbgUiPrBase::kOk)
		return;
	McGePoint3d corner2 = getCorner2.value();

	HIDE_PROMPT_MESSAGE;

	// Select TEXT/MTEXT entities within the rectangular area
	MrxDbgSelSet ss;
	MrxDbgRbList spFilter = Mx::mcutBuildList(RTDXF0, "TEXT,MTEXT", 0);
	if (ss.crossingSelect(corner1, corner2, spFilter.data()) != MrxDbgSelSet::kSelected)
		return;

	McDbObjectIdArray entityIds;
	ss.asArray(entityIds);

	// List to store text line groups (for sorting later)
	QList<QList<Mx2d::TextLine>> textLineGroups;

	// Extract text content from selected entities
	for (int i = 0; i < entityIds.length(); ++i)
	{
		QList<Mx2d::TextLine> textLines = Mx2d::getTextContent(entityIds[i]);
		if (!textLines.isEmpty())
		{
			textLineGroups.append(textLines);
		}
	}

	// Sort text groups from top to bottom (by Y-coordinate of first line)
	std::sort(textLineGroups.begin(), textLineGroups.end(), [](const QList<Mx2d::TextLine>& a, const QList<Mx2d::TextLine>& b) {
		return a.first().pos.y > b.first().pos.y;
		});

	// Flatten sorted groups into a single list
	QList<Mx2d::TextLine> resultLines;
	for (const auto& group : textLineGroups)
	{
		resultLines.append(group);
	}

	// Convert text lines to string list
	QStringList extractedTexts;
	for (const auto& line : resultLines)
	{
		extractedTexts.append(line.text);
	}

	// Notify UI that text extraction is completed
	MXAPP.CallMain([=]() {
		emit Mx2dSignalTransfer::getInstance().signalExtractTextFinished(MxUtils::gCurrentTab, extractedTexts);
		});
}

void MxCADCommand::Mx_ExtractTextPoly()
{
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select start point"));
	MrxDbgUiPrPoint getStartPoint(QCoreApplication::translate("MxCADCommand", "Select start point").toStdString().c_str());
	MrxDbgUiPrBase::Status ret = getStartPoint.go();
	if (ret != MrxDbgUiPrBase::kOk) return;

	McGePoint3d startPoint = getStartPoint.value();
	McGePoint3dArray polygonPoints;
	polygonPoints.append(startPoint);

	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select next point"));
	while (true)
	{
		MrxDbgUiPrPoint getNextPoint(QCoreApplication::translate("MxCADCommand", "Select next point").toStdString().c_str());
		// Dynamic preview of the polygon during selection
		getNextPoint.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
			worldDrawPolygon(polygonPoints, pData, pWorldDraw);
			});
		ret = getNextPoint.go();

		if (ret == MrxDbgUiPrBase::kOk) {
			polygonPoints.append(getNextPoint.value());
			continue;
		}
		else if (ret == MrxDbgUiPrBase::kCancel) {
			HIDE_PROMPT_MESSAGE;
			return;
		}
		else if (ret == MrxDbgUiPrBase::kNone) {
			HIDE_PROMPT_MESSAGE;
			break;
		}
	}

	// Reject self-intersecting polygons
	if (isPolygonSelfIntersecting(polygonPoints))
	{
		MXAPP.CallMain([=]() {
			emit MxSignalTransfer::getInstance().signalMessageBoxInformation(
				MxUtils::gCurrentTab,
				QCoreApplication::translate("MxCADCommand", "Self-intersecting polygons are not allowed. Please select again.")
			);
			});
		return;
	}

	// Get polygon bounding box and select TEXT/MTEXT within it
	McDbExtents polygonExtents = getPolygonGeomExtents(polygonPoints);
	MrxDbgSelSet selectionSet;
	MrxDbgRbList entityFilter = Mx::mcutBuildList(RTDXF0, "TEXT,MTEXT", 0);

	if (selectionSet.crossingSelect(
		polygonExtents.minPoint(),
		polygonExtents.maxPoint(),
		entityFilter.data()
	) != MrxDbgSelSet::kSelected)
		return;

	McDbObjectIdArray entityIds;
	selectionSet.asArray(entityIds);

	// Filter entities that are strictly inside the polygon
	entityIds = getTextInPolygon(entityIds, polygonPoints);

	// Group extracted text lines (for sorting)
	QList<QList<Mx2d::TextLine>> textLineGroups;
	for (int i = 0; i < entityIds.length(); ++i)
	{
		QList<Mx2d::TextLine> textLines = Mx2d::getTextContent(entityIds[i]);
		if (!textLines.isEmpty())
		{
			textLineGroups.append(textLines);
		}
	}

	// Sort text groups from top to bottom (by Y-coordinate of first line)
	std::sort(textLineGroups.begin(), textLineGroups.end(), [](const QList<Mx2d::TextLine>& a, const QList<Mx2d::TextLine>& b) {
		return a.first().pos.y > b.first().pos.y;
		});

	// Flatten sorted groups and convert to string list
	QList<Mx2d::TextLine> resultLines;
	for (const auto& group : textLineGroups)
	{
		resultLines.append(group);
	}

	QStringList extractedTexts;
	for (const auto& line : resultLines)
	{
		extractedTexts.append(line.text);
	}

	// Notify UI of completed text extraction
	MXAPP.CallMain([=]() {
		emit Mx2dSignalTransfer::getInstance().signalExtractTextFinished(MxUtils::gCurrentTab, extractedTexts);
		});
}
#if 1
void MxCADCommand::Mx_ExtractTable() 
{
	double m_gap = 0.1;
	MrxDbgUiPrPoint getCorner1(QCoreApplication::translate("MxCADCommand", "Select first corner of rectangular area").toStdString().c_str()), getCorner2(QCoreApplication::translate("MxCADCommand", "Select second corner of rectangular area").toStdString().c_str());
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select first corner of rectangular area"));
	MrxDbgUiPrBase::Status ret = getCorner1.go();
	if (ret != MrxDbgUiPrBase::kOk)
	{
		HIDE_PROMPT_MESSAGE;
		return;
	}
	McGePoint3d corner1 = getCorner1.value();
	getCorner2.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
		worldDrawRect(corner1, pData, pWorldDraw);
		});
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select second corner of rectangular area"));
	ret = getCorner2.go();
	if (ret != MrxDbgUiPrBase::kOk)
	{
		HIDE_PROMPT_MESSAGE;
		return;
	}
	HIDE_PROMPT_MESSAGE;
	McGePoint3d corner2 = getCorner2.value();

	McGePoint3d new_corner1(min(corner1.x, corner2.x) - m_gap, min(corner1.y, corner2.y) - m_gap, 0);
	McGePoint3d new_corner2(max(corner1.x, corner2.x) + m_gap, max(corner1.y, corner2.y) + m_gap, 0);
	MXAPP.CallMain([=]() {
		emit Mx2dSignalTransfer::getInstance().signalTableRect(MxUtils::gCurrentTab, new_corner1, new_corner2);
		});
}

#else
void MxCADCommand::Mx_ExtractTable()
{
	double m_gap = 0.1;
	MrxDbgUiPrPoint getCorner1(QCoreApplication::translate("MxCADCommand", "Select first corner of rectangular area").toStdString().c_str()), getCorner2(QCoreApplication::translate("MxCADCommand", "Select second corner of rectangular area").toStdString().c_str());
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select first corner of rectangular area"));
	MrxDbgUiPrBase::Status ret = getCorner1.go();
	if (ret != MrxDbgUiPrBase::kOk)
	{
		HIDE_PROMPT_MESSAGE;
		return;
	}
	McGePoint3d corner1 = getCorner1.value();
	getCorner2.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
		worldDrawRect(corner1, pData, pWorldDraw);
		});
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select second corner of rectangular area"));
	ret = getCorner2.go();
	if (ret != MrxDbgUiPrBase::kOk)
	{
		HIDE_PROMPT_MESSAGE;
		return;
	}
	HIDE_PROMPT_MESSAGE;
	McGePoint3d corner2 = getCorner2.value();

	McGePoint3d new_corner1(min(corner1.x, corner2.x) - m_gap, min(corner1.y, corner2.y) - m_gap, 0);
	McGePoint3d new_corner2(max(corner1.x, corner2.x) + m_gap, max(corner1.y, corner2.y) + m_gap, 0);

	McGePoint3d ptLT(min(new_corner1.x, new_corner2.x), max(new_corner1.y, new_corner2.y), 0);
	McGePoint3d ptRT(max(new_corner1.x, new_corner2.x), max(new_corner1.y, new_corner2.y), 0);
	McGePoint3d ptRB(max(new_corner1.x, new_corner2.x), min(new_corner1.y, new_corner2.y), 0);
	McGePoint3d ptLB(min(new_corner1.x, new_corner2.x), min(new_corner1.y, new_corner2.y), 0);

	double new_sel_rect_left = min(new_corner1.x, new_corner2.x);
	double new_sel_rect_right = max(new_corner1.x, new_corner2.x);
	double new_sel_rect_bottom = min(new_corner1.y, new_corner2.y);
	double new_sel_rect_top = max(new_corner1.y, new_corner2.y);

	MrxDbgSelSet ss;

	if (ss.crossingSelect(new_corner1, new_corner2/*, spFilter.data()*/) != MrxDbgSelSet::kSelected)
		return;
	McDbObjectIdArray aryId;
	ss.asArray(aryId);

	std::vector<McDbEntity*>  ent_ptr_array_all;
	for (int i = 0; i < aryId.length(); i++)
	{
		McDbObjectPointer<McDbEntity> spEntity(aryId[i], McDb::kForWrite);
		if (spEntity.openStatus() != Mcad::eOk)
			continue;

		if (spEntity->isA() == McDbBlockReference::desc())
		{
			McDbBlockReference* pBlockRef = McDbBlockReference::cast(spEntity.object());
			McDbVoidPtrArray entitySet;
			Mx2d::recursiveExplodeBlock(pBlockRef, entitySet);
			for (int j = 0; j < entitySet.length(); j++)
			{
				McDbEntity* pEntity = (McDbEntity*)entitySet[j];
				ent_ptr_array_all.emplace_back(pEntity);
			}
		}
		else if (spEntity->isA() == McDbProxyEntity::desc())
		{
			McDbProxyEntity* pProxyEntity = McDbProxyEntity::cast(spEntity.object());
			McDbVoidPtrArray entitySet;
			pProxyEntity->explode(entitySet);
			for (int j = 0; j < entitySet.length(); j++)
			{
				McDbEntity* pEntity = (McDbEntity*)entitySet[j];
				ent_ptr_array_all.emplace_back(pEntity);
			}
		}
		else
		{
			McDbEntity* pEntity = (McDbEntity*)spEntity->clone();
			ent_ptr_array_all.emplace_back(pEntity);
		}
	}

	std::vector<McDbText*> all_texts;
	double text_height_avg, text_height_sum = 0;
	double text_width_height_ratio_avg, text_width_height_ratio_sum = 0;
	for (int i = 0; i < (int)ent_ptr_array_all.size(); i++)
	{
		McDbEntity* pEntity = ent_ptr_array_all[i];
		if (pEntity->isA() == McDbText::desc())
		{
			McDbText* pText = (McDbText*)pEntity->clone();
			McDbExtents ext;
			pText->getGeomExtents(ext, false);
			McGePoint3d centerPoint((ext.minPoint().x + ext.maxPoint().x) / 2, (ext.minPoint().y + ext.maxPoint().y) / 2, 0);
			if (centerPoint.x >= new_sel_rect_left && centerPoint.x <= new_sel_rect_right && centerPoint.y >= new_sel_rect_bottom && centerPoint.y <= new_sel_rect_top)
			{
				all_texts.emplace_back(pText);
				text_height_sum += pText->height();
				text_width_height_ratio_sum += pText->widthFactor();
			}
		}
		else if (pEntity->isA() == McDbMText::desc())
		{
			McDbMText* pMText = McDbMText::cast(pEntity);
			McDbVoidPtrArray entitySet;
			pMText->explode(entitySet);
			for (int j = 0; j < entitySet.length(); j++)
			{
				McDbText* pText = (McDbText*)entitySet[j];

				McDbExtents ext;
				pText->getGeomExtents(ext, false);
				McGePoint3d centerPoint((ext.minPoint().x + ext.maxPoint().x) / 2, (ext.minPoint().y + ext.maxPoint().y) / 2, 0);
				if (centerPoint.x >= new_sel_rect_left && centerPoint.x <= new_sel_rect_right && centerPoint.y >= new_sel_rect_bottom && centerPoint.y <= new_sel_rect_top)
				{
					all_texts.emplace_back(pText);
					text_height_sum += pText->height();
					text_width_height_ratio_sum += pText->widthFactor();
				}
			}
		}
	}
	bool set_ratio = false;

	if (all_texts.size() != 0)
	{
		set_ratio = true;
		text_height_avg = text_height_sum / all_texts.size();
		text_width_height_ratio_avg = text_width_height_ratio_sum / all_texts.size();
	}

	std::vector<McDbLine*> linesPtr;

	for (int i = 0; i < (int)ent_ptr_array_all.size(); i++)
	{
		McDbEntity* pEntity = ent_ptr_array_all[i];
		if (pEntity->isA() == McDbPolyline::desc())
		{
			McDbPolyline* pPLine = McDbPolyline::cast(pEntity);
			McDbVoidPtrArray entitySet;
			pPLine->explode(entitySet);
			for (int j = 0; j < entitySet.length(); j++)
			{
				McDbEntity* pEntity = (McDbEntity*)entitySet[j];
				if (pEntity->isA() == McDbLine::desc())
				{
					linesPtr.emplace_back((McDbLine*)pEntity);
				}
				else
				{
					delete pEntity;
				}
			}
			continue;
		}
		else if (pEntity->isA() == McDbLine::desc())
		{
			McDbLine* pLine = (McDbLine*)pEntity->clone();
			linesPtr.emplace_back(pLine);
		}
	}

	for (int i = 0; i < (int)ent_ptr_array_all.size(); i++)
	{
		delete ent_ptr_array_all[i];
	}

	std::vector<McDbLine*> horizontalLines, verticalLines;
	std::vector<double> Xcoords, Ycoords;
	for (int i = 0; i < (int)linesPtr.size(); i++)
	{
		if (Mx2d::IsEqual(linesPtr[i]->startPoint().y, linesPtr[i]->endPoint().y, m_gap))
		{
			bool intersectTop = Mx2d::isIntersect(linesPtr[i]->startPoint(), linesPtr[i]->endPoint(), ptLT, ptRT);
			bool intersectRight = Mx2d::isIntersect(linesPtr[i]->startPoint(), linesPtr[i]->endPoint(), ptRT, ptRB);
			bool intersectBottom = Mx2d::isIntersect(linesPtr[i]->startPoint(), linesPtr[i]->endPoint(), ptRB, ptLB);
			bool intersectLeft = Mx2d::isIntersect(linesPtr[i]->startPoint(), linesPtr[i]->endPoint(), ptLB, ptLT);

			bool inRect = Mx2d::isInRect(linesPtr[i]->startPoint(), linesPtr[i]->endPoint(), ptLT.x, ptRB.x, ptRB.y, ptLT.y);

			if (intersectTop || intersectRight || intersectBottom || intersectLeft || inRect)
			{
				horizontalLines.emplace_back(linesPtr[i]);
				bool findSame = false;
				for (int j = 0; j < (int)Ycoords.size(); j++)
				{
					if (Mx2d::IsEqual(linesPtr[i]->startPoint().y, Ycoords[j], m_gap))
					{
						findSame = true;
						break;
					}
				}
				if (!findSame)
				{
					Ycoords.emplace_back(linesPtr[i]->startPoint().y);
				}
				continue;
			}
		}
		else if (Mx2d::IsEqual(linesPtr[i]->startPoint().x, linesPtr[i]->endPoint().x, m_gap))
		{
			bool intersectTop = Mx2d::isIntersect(linesPtr[i]->startPoint(), linesPtr[i]->endPoint(), ptLT, ptRT);
			bool intersectRight = Mx2d::isIntersect(linesPtr[i]->startPoint(), linesPtr[i]->endPoint(), ptRT, ptRB);
			bool intersectBottom = Mx2d::isIntersect(linesPtr[i]->startPoint(), linesPtr[i]->endPoint(), ptRB, ptLB);
			bool intersectLeft = Mx2d::isIntersect(linesPtr[i]->startPoint(), linesPtr[i]->endPoint(), ptLB, ptLT);

			bool inRect = Mx2d::isInRect(linesPtr[i]->startPoint(), linesPtr[i]->endPoint(), ptLT.x, ptRB.x, ptRB.y, ptLT.y);

			if (intersectTop || intersectRight || intersectBottom || intersectLeft || inRect)
			{
				verticalLines.emplace_back(linesPtr[i]);
				bool findSame = false;
				for (int j = 0; j < (int)Xcoords.size(); j++)
				{
					if (Mx2d::IsEqual(linesPtr[i]->startPoint().x, Xcoords[j], m_gap))
					{
						findSame = true;
						break;
					}
				}
				if (!findSame)
				{
					Xcoords.emplace_back(linesPtr[i]->startPoint().x);
				}
			}

		}
	}

	if ((int)Xcoords.size() < 2 || (int)Ycoords.size() < 2)
	{
		LOG_ERROR(QString("can not be a table"));
		return;
	}

	std::vector<double>::iterator itS = Xcoords.begin(), itE = Xcoords.end();
	std::sort(itS, itE);
	itS = Ycoords.begin(), itE = Ycoords.end();
	std::sort(itS, itE);

	std::map<double, std::vector< McDbLine* >> sortedHorizontalLines, sortedVerticalLines;

	std::map<double, std::vector<Mx2d::ContinueRange>> sortedHorizontalRanges, sortedVerticalRanges;

	for (int i = 0; i < (int)Ycoords.size(); i++)
	{
		std::vector<McDbLine*> sameYcoordLines;
		std::vector<Mx2d::ContinueRange> ranges;
		for (int j = 0; j < (int)horizontalLines.size(); j++)
		{
			if (Mx2d::IsEqual(Ycoords[i], horizontalLines[j]->startPoint().y, m_gap))
			{
				sameYcoordLines.emplace_back(horizontalLines[j]);
			}
		}
		std::vector<McDbLine*>::iterator itS = sameYcoordLines.begin(), itE = sameYcoordLines.end();
		std::sort(itS, itE, Mx2d::cmpHLine);
		sortedHorizontalLines.insert(make_pair(Ycoords[i], sameYcoordLines));
		McDbLine* pFirst = *itS;
		Mx2d::ContinueRange curRange(min(pFirst->startPoint().x, pFirst->endPoint().x), max(pFirst->startPoint().x, pFirst->endPoint().x));
		for (; itS != itE - 1; itS++)
		{
			McDbLine* pNext = *(itS + 1);
			double nextStart = min(pNext->startPoint().x, pNext->endPoint().x);
			double nextEnd = max(pNext->startPoint().x, pNext->endPoint().x);
			if ((curRange.end >= nextStart || curRange.end + m_gap > nextStart) && curRange.end <= nextEnd)
			{
				curRange = Mx2d::ContinueRange(curRange.start, nextEnd);
			}
			else
			{
				ranges.emplace_back(curRange);
				curRange = Mx2d::ContinueRange(nextStart, nextEnd);
			}
		}
		ranges.emplace_back(curRange);
		sortedHorizontalRanges.insert(make_pair(Ycoords[i], ranges));

	}
	for (int i = 0; i < (int)Xcoords.size(); i++)
	{
		std::vector<McDbLine*> sameXcoordLines;
		std::vector<Mx2d::ContinueRange> ranges;
		for (int j = 0; j < (int)verticalLines.size(); j++)
		{
			if (Mx2d::IsEqual(Xcoords[i], verticalLines[j]->startPoint().x, m_gap))
			{
				sameXcoordLines.emplace_back(verticalLines[j]);
			}
		}
		std::vector<McDbLine*>::iterator itS = sameXcoordLines.begin(), itE = sameXcoordLines.end();
		sort(itS, itE, Mx2d::cmpVLine);
		sortedVerticalLines.insert(make_pair(Xcoords[i], sameXcoordLines));
		McDbLine* pFirst = *itS;
		Mx2d::ContinueRange curRange(min(pFirst->startPoint().y, pFirst->endPoint().y), max(pFirst->startPoint().y, pFirst->endPoint().y));
		for (; itS != itE - 1; itS++)
		{
			McDbLine* pNext = *(itS + 1);
			double nextStart = min(pNext->startPoint().y, pNext->endPoint().y);
			double nextEnd = max(pNext->startPoint().y, pNext->endPoint().y);
			if ((curRange.end >= nextStart || curRange.end + m_gap > nextStart) && curRange.end <= nextEnd)
			{
				curRange = Mx2d::ContinueRange(curRange.start, nextEnd);
			}
			else
			{
				ranges.emplace_back(curRange);
				curRange = Mx2d::ContinueRange(nextStart, nextEnd);
			}
		}
		ranges.emplace_back(curRange);
		sortedVerticalRanges.insert(make_pair(Xcoords[i], ranges));

	}

	for (int i = 0; i < (int)linesPtr.size(); i++)
	{
		delete linesPtr[i];
	}

	std::vector<Mx2d::CellRect> uniqueRects;

	for (int i = 0; i < (int)Xcoords.size() - 1; i++)
	{
		for (int j = i + 1; j < (int)Xcoords.size(); j++)
		{
			for (int k = 0; k < (int)Ycoords.size() - 1; k++)
			{
				for (int m = k + 1; m < (int)Ycoords.size(); m++)
				{
					uniqueRects.emplace_back(Mx2d::CellRect(Xcoords[i], Xcoords[j], Ycoords[k], Ycoords[m]));
				}
			}
		}
	}

	std::vector<Mx2d::CellRect> noCrossRangeRects;
	std::vector<Mx2d::CellRect> cellRects;

	for (int i = 0; i < (int)uniqueRects.size(); i++)
	{
		Mx2d::CellRect rect = uniqueRects[i];
		std::map<double, std::vector<Mx2d::ContinueRange>>::iterator itLeft, itRight, itBottom, itTop, itEndH, itEndV;
		itLeft = sortedVerticalRanges.find(rect.left);
		itRight = sortedVerticalRanges.find(rect.right);
		itBottom = sortedHorizontalRanges.find(rect.bottom);
		itTop = sortedHorizontalRanges.find(rect.top);
		itEndH = sortedHorizontalRanges.end();
		itEndV = sortedVerticalRanges.end();
		if (itLeft != itEndV && itRight != itEndV && itBottom != itEndH && itTop != itEndH)
		{
			std::vector<Mx2d::ContinueRange> rangesLeft = itLeft->second;
			std::vector<Mx2d::ContinueRange> rangesRight = itRight->second;
			Mx2d::ContinueRange rect_left_right_edge(rect.bottom, rect.top);
			std::vector<Mx2d::ContinueRange> rangesBottom = itBottom->second;
			std::vector<Mx2d::ContinueRange> rangesTop = itTop->second;
			Mx2d::ContinueRange rect_bottom_top_edge(rect.left, rect.right);

			if (find(rangesLeft.begin(), rangesLeft.end(), rect_left_right_edge) == rangesLeft.end())
				continue;
			if (find(rangesRight.begin(), rangesRight.end(), rect_left_right_edge) == rangesRight.end())
				continue;
			if (find(rangesBottom.begin(), rangesBottom.end(), rect_bottom_top_edge) == rangesBottom.end())
				continue;
			if (find(rangesTop.begin(), rangesTop.end(), rect_bottom_top_edge) == rangesTop.end())
				continue;

			std::map<double, std::vector<Mx2d::ContinueRange>>::iterator it = sortedHorizontalRanges.begin();
			bool beCrossedByH = false;
			for (; it != sortedHorizontalRanges.end(); it++)
			{
				double y = it->first;
				std::vector<Mx2d::ContinueRange> ranges = it->second;
				if (y > rect.bottom && y < rect.top)
				{
					for (int j = 0; j < (int)ranges.size(); j++)
					{
						if (rect.left >= ranges[j].start - m_gap && rect.right <= ranges[j].end + m_gap)
						{
							beCrossedByH = true;
							break;
						}
					}
					if (!beCrossedByH)
					{
						continue;
					}
					else
					{
						break;
					}
				}
			}
			if (beCrossedByH)
			{
				continue;
			}
			it = sortedVerticalRanges.begin();
			bool beCrossedByV = false;
			for (; it != sortedVerticalRanges.end(); it++)// vertical line
			{
				double x = it->first;
				std::vector<Mx2d::ContinueRange> ranges = it->second;
				if (x > rect.left && x < rect.right)
				{
					for (int j = 0; j < (int)ranges.size(); j++)
					{
						if (rect.bottom >= ranges[j].start - m_gap && rect.top <= ranges[j].end + m_gap)
						{
							beCrossedByV = true;
							break;
						}
					}
					if (!beCrossedByV)
					{
						continue;
					}
					else
					{
						break;
					}
				}
			}
			if (beCrossedByV)
			{
				continue;
			}

			noCrossRangeRects.emplace_back(rect);
		}
		else
		{
			continue;
		}
	}

	for (int i = 0; i < (int)noCrossRangeRects.size(); i++)
	{
		bool isInnerRect = false;
		for (int j = 0; j < (int)noCrossRangeRects.size(); j++)
		{
			if (i == j)
			{
				continue;
			}
			if (noCrossRangeRects[i].beLongTo(noCrossRangeRects[j]))
			{
				isInnerRect = true;
				break;
			}
		}
		if (!isInnerRect)
		{
			cellRects.emplace_back(noCrossRangeRects[i]);
		}
	}

	if ((int)cellRects.size() == 0)
	{
		return;
	}

	std::vector<double> newXcoords, newYcoords;

	for (int i = 0; i < (int)cellRects.size(); i++)
	{
		Mx2d::CellRect rect = cellRects[i];
		bool findLeft = false;
		bool findRight = false;
		bool findBottom = false;
		bool findTop = false;
		for (int j = 0; j < (int)newXcoords.size(); j++)
		{
			if (Mx2d::IsEqual(rect.left, newXcoords[j], m_gap))
			{
				findLeft = true;
				break;
			}
		}
		if (!findLeft)
		{
			newXcoords.emplace_back(rect.left);
		}
		for (int j = 0; j < (int)newXcoords.size(); j++)
		{
			if (Mx2d::IsEqual(rect.right, newXcoords[j], m_gap))
			{
				findRight = true;
				break;
			}
		}
		if (!findRight)
		{
			newXcoords.emplace_back(rect.right);
		}
		for (int j = 0; j < (int)newYcoords.size(); j++)
		{
			if (Mx2d::IsEqual(rect.bottom, newYcoords[j], m_gap))
			{
				findBottom = true;
				break;
			}
		}
		if (!findBottom)
		{
			newYcoords.emplace_back(rect.bottom);
		}
		for (int j = 0; j < (int)newYcoords.size(); j++)
		{
			if (Mx2d::IsEqual(rect.top, newYcoords[j], m_gap))
			{
				findTop = true;
				break;
			}
		}
		if (!findTop)
		{
			newYcoords.emplace_back(rect.top);
		}
	}

	std::vector<double>::iterator it_newS = newXcoords.begin(), it_newE = newXcoords.end();
	std::sort(it_newS, it_newE);
	it_newS = newYcoords.begin(), it_newE = newYcoords.end();
	std::sort(it_newS, it_newE);

	int cnt = 0;
	std::map<int, double> row, colomn;
	for (int i = 0; i < (int)newYcoords.size() - 1; i++)
	{
		double YcoordS = newYcoords[i];
		double YcoordE = newYcoords[i + 1];
		double Ycoord = (YcoordS + YcoordE) / 2;
		row.insert(std::make_pair(cnt, Ycoord));
		cnt++;
	}
	cnt = 0;
	for (int i = 0; i < (int)newXcoords.size() - 1; i++)
	{
		double XcoordS = newXcoords[i];
		double XcoordE = newXcoords[i + 1];
		double Xcoord = (XcoordS + XcoordE) / 2;
		colomn.insert(std::make_pair(cnt, Xcoord));
		cnt++;
	}
	for (int i = 0; i < (int)cellRects.size(); i++)
	{
		Mx2d::CellRect& cell_rect = cellRects[i];
		std::map<int, double>::iterator it_r = row.begin(), it_c = colomn.begin();
		std::vector<int> rows_buf, colomns_buf;
		for (; it_r != row.end(); it_r++)
		{
			double Ycoord = it_r->second;
			int rowNum = it_r->first;
			if (Ycoord > cell_rect.bottom && Ycoord < cell_rect.top)
			{
				rows_buf.emplace_back(rowNum);
			}
		}
		cell_rect.rowStart = rows_buf.front();
		cell_rect.rowEnd = rows_buf.back();
		for (; it_c != colomn.end(); it_c++)
		{
			double Xcoord = it_c->second;
			int colomnNum = it_c->first;
			if (Xcoord > cell_rect.left && Xcoord < cell_rect.right)
			{
				colomns_buf.emplace_back(colomnNum);
			}
		}
		cell_rect.columnStart = colomns_buf.front();
		cell_rect.columnEnd = colomns_buf.back();
	}


	for (int i = 0; i < (int)cellRects.size(); i++)
	{
		Mx2d::CellRect& cell = cellRects[i];

		std::vector<McDbText*> texts;
		std::map<double, std::vector<McDbText*>> rows;
		QString text;
		for (int j = 0; j < (int)all_texts.size(); j++)
		{
			McDbExtents ext;
			all_texts[j]->getGeomExtents(ext, false);
			McGePoint3d pt((ext.minPoint().x + ext.maxPoint().x) / 2, (ext.minPoint().y + ext.maxPoint().y) / 2, 0);
			if (pt.x<cell.left || pt.x > cell.right || pt.y > cell.top || pt.y < cell.bottom)
				continue;
			texts.emplace_back(all_texts[j]);

		}

		if ((int)texts.size() != 0)
		{
			for (int m = 0; m < (int)texts.size() - 1; m++)
			{
				for (int n = 0; n < (int)texts.size() - 1 - m; n++)
				{
					if (texts[n]->position().x > texts[n + 1]->position().x)
					{
						McDbText* tmp_text_ptr;
						tmp_text_ptr = texts[n];
						texts[n] = texts[n + 1];
						texts[n + 1] = tmp_text_ptr;
					}
				}
			}
		}

		while ((int)texts.size() >= 1)
		{
			std::vector<McDbText*> tmp_texts;

			McDbExtents ext_firstText;
			texts[0]->getGeomExtents(ext_firstText, false);
			double row = (ext_firstText.minPoint().y + ext_firstText.maxPoint().y) / 2;
			std::vector<McDbText*> this_row_texts;
			this_row_texts.emplace_back(texts[0]);
			for (int k = 1; k < (int)texts.size(); k++)
			{
				McDbExtents ext;
				texts[k]->getGeomExtents(ext, false);

				if (ext.minPoint().y <= ext_firstText.maxPoint().y && ext.maxPoint().y >= ext_firstText.minPoint().y)
				{
					this_row_texts.emplace_back(texts[k]);
				}
				else
				{
					tmp_texts.emplace_back(texts[k]);
				}
			}
			rows.insert(make_pair(row, this_row_texts));
			texts = tmp_texts;
		}

		for (auto it = rows.rbegin(); it != rows.rend(); it++)
		{
			QString this_row_strs;
			for (int k = 0; k < (int)it->second.size(); k++)
			{
				this_row_strs += QString::fromLocal8Bit(it->second[k]->textString());
			}
			if (it != --rows.rend())
			{
				text = text + this_row_strs + "\n";
			}
			else
			{
				text += this_row_strs;
			}
		}
		cell.texts = text;
	}

	for (int i = 0; i < (int)all_texts.size(); i++)
	{
		delete all_texts[i];
	}

	MXAPP.CallMain([=]() {

		QString defaultDir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
		QString fileName = QCoreApplication::translate("MxCADCommand", "table.xlsx");
		QString filter = QCoreApplication::translate("MxCADCommand", "Spreadsheet files (*.xlsx *.xls);;All files (*)");

		QFileDialog saveFileDlg(nullptr, QCoreApplication::translate("MxCADCommand", "Save Spreadsheet File"), defaultDir, filter);
		saveFileDlg.setDefaultSuffix("xlsx");
		saveFileDlg.setFileMode(QFileDialog::AnyFile);
		saveFileDlg.setAcceptMode(QFileDialog::AcceptSave);
		saveFileDlg.setOption(QFileDialog::DontConfirmOverwrite, false);

		if (saveFileDlg.exec() != QDialog::Accepted)
		{
			return;
		}

		QString filePath = saveFileDlg.selectedFiles().first();

		lxw_workbook* workbook = workbook_new(filePath.toUtf8().constData());

		lxw_worksheet* worksheet = workbook_add_worksheet(workbook, NULL);

		if (set_ratio)
		{
			for (int i = 0; i < (int)newXcoords.size() - 1; i++)
			{
				worksheet_set_column(worksheet, i, i, (newXcoords[i + 1] - newXcoords[i]) * 1.5 / (text_height_avg * text_width_height_ratio_avg), NULL);
			}
			for (int i = (int)newYcoords.size() - 1; i > 0; i--)
			{
				worksheet_set_row(worksheet, (int)newYcoords.size() - i - 1, (newYcoords[i] - newYcoords[i - 1]) * 8 / text_height_avg, NULL);
			}
		}

		lxw_format* cell_format = workbook_add_format(workbook);
		format_set_align(cell_format, LXW_ALIGN_CENTER);
		format_set_align(cell_format, LXW_ALIGN_VERTICAL_CENTER);
		format_set_border(cell_format, LXW_BORDER_THIN);
		format_set_text_wrap(cell_format);
		format_set_font_name(cell_format, "Arial");
		format_set_font_size(cell_format, 10);

		for (int i = 0; i < (int)cellRects.size(); i++)
		{
			Mx2d::CellRect cell = cellRects[i];

			if (cell.rowStart == cell.rowEnd && cell.columnStart == cell.columnEnd)
			{
				worksheet_write_string(worksheet, (int)row.size() - cell.rowStart - 1, cell.columnStart, cell.texts.toUtf8().constData(), cell_format);
			}
			else
			{
				worksheet_merge_range(worksheet, (int)row.size() - cell.rowStart - 1, cell.columnStart, (int)row.size() - cell.rowEnd - 1, cell.columnEnd, cell.texts.toUtf8().constData(), cell_format);
			}
		}

		lxw_error err = workbook_close(workbook);
		if (LXW_NO_ERROR != err) {
			LOG_ERROR(QString("Failed to export table, please check if file is already open, close it and try again!"));
			// TODO: show error message
			return;
		}
		});
}

#endif

void MxCADCommand::Mx_DrawNumberedText()
{
	struct resbuf* pRb = Mx::mcedGetArgs();
	if (!pRb || pRb->restype != RTSTR ||
		!pRb->rbnext || pRb->rbnext->restype != RTLONG ||
		!pRb->rbnext->rbnext || pRb->rbnext->rbnext->restype != RTSHORT)
	{
		return;
	}

	QString baseStr = QString::fromLocal8Bit(pRb->resval.rstring);
	long startNumber = pRb->rbnext->resval.rlong;
	short prefix = pRb->rbnext->rbnext->resval.rint;
	bool isPrefix = (prefix == 0);

	double scaled = Mx2d::getViewToDocScaleRatio();
	double textHeight = 25 / scaled;
	McDbObjectId textStyle = Mx::mcdbCurDwg()->textstyle();

	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select text insertion point"));

	while (true)
	{
		QString qstr;
		if (isPrefix)
		{
			qstr = QString("%1%2").arg(startNumber++).arg(baseStr);
		}
		else {
			qstr = QString("%1%2").arg(baseStr).arg(startNumber++);
		}

		MrxDbgUiPrPoint getStartPoint(QCoreApplication::translate("MxCADCommand", "Select text insertion point").toStdString().c_str());

		getStartPoint.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
			worldDrawText(qstr, textHeight, textStyle, pData, pWorldDraw);
			});

		MrxDbgUiPrBase::Status ret = getStartPoint.go();


		if (ret != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}
		auto pt = getStartPoint.value();

		MXAPP.CallMain([=]() {
			auto* pAnno = new Mx2dCustomText(qstr, pt, textHeight);
			McDbObjectId id = Mx2d::addEntityToAnnotationLayerAndClose(pAnno);
			emit Mx2dSignalTransfer::getInstance().signalAddAnnotation(MxUtils::gCurrentTab, id);
			});
	}

}

void MxCADCommand::Mx_DrawText()
{
	struct resbuf* pRb = Mx::mcedGetArgs();
	if (!pRb || pRb->restype != RTSTR)
		return;
	QString textStr = QString::fromLocal8Bit(pRb->resval.rstring);

	double scaled = Mx2d::getViewToDocScaleRatio();
	double textHeight = 25 / scaled;
	McDbObjectId textStyle = Mx::mcdbCurDwg()->textstyle();
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select text insertion point"));

	while (true)
	{
		MrxDbgUiPrPoint getStartPoint(QCoreApplication::translate("MxCADCommand", "Select text insertion point").toStdString().c_str());
		getStartPoint.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
			worldDrawText(textStr, textHeight, textStyle, pData, pWorldDraw);
			});
		MrxDbgUiPrBase::Status ret = getStartPoint.go();

		if (ret != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}
		auto pt = getStartPoint.value();
		MXAPP.CallMain([=]() {
			auto* pAnno = new Mx2dCustomText(textStr, pt, textHeight);
			McDbObjectId id = Mx2d::addEntityToAnnotationLayerAndClose(pAnno);
			emit Mx2dSignalTransfer::getInstance().signalAddAnnotation(MxUtils::gCurrentTab, id);
			});
	}
}

void MxCADCommand::Mx_DrawMText()
{
	struct resbuf* pRb = Mx::mcedGetArgs();
	if (!pRb || pRb->restype != RTSTR)
	{
		return;
	}
	QString textStr = QString::fromLocal8Bit(pRb->resval.rstring);

	double scaled = Mx2d::getViewToDocScaleRatio();
	double textHeight = 25 / scaled;
	McDbObjectId textStyle = Mx::mcdbCurDwg()->textstyle();
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select text insertion point"));

	while (true)
	{
		MrxDbgUiPrPoint getStartPoint(QCoreApplication::translate("MxCADCommand", "Select text insertion point").toStdString().c_str());
		getStartPoint.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
			worldDrawMText(textStr, textHeight, textStyle, pData, pWorldDraw);
			});
		MrxDbgUiPrBase::Status ret = getStartPoint.go();

		if (ret != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}
		auto pt = getStartPoint.value();
		MXAPP.CallMain([=]() {
			auto* pAnno = new Mx2dCustomMText(textStr, pt, textHeight);
			McDbObjectId id = Mx2d::addEntityToAnnotationLayerAndClose(pAnno);
			emit Mx2dSignalTransfer::getInstance().signalAddAnnotation(MxUtils::gCurrentTab, id);
			});
	}
}

void MxCADCommand::Mx_DrawRectMark()
{
    double scaled = Mx2d::getViewToDocScaleRatio();
	double textHeight = 25 / scaled;
	while (true)
	{
		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select first corner of rectangular area"));
		MrxDbgUiPrPoint getPoint1(QCoreApplication::translate("MxCADCommand", "Select first corner of rectangular area").toStdString().c_str());
		MrxDbgUiPrBase::Status ret = getPoint1.go();
		if (ret != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}
		McGePoint3d pt1 = getPoint1.value();
		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select second corner of rectangular area"));
		MrxDbgUiPrPoint getPoint2(QCoreApplication::translate("MxCADCommand", "Select second corner of rectangular area").toStdString().c_str());
		getPoint2.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
			worldDrawRect(pt1, pData, pWorldDraw);
			});
		ret = getPoint2.go();
		if (ret != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}
		McGePoint3d pt2 = getPoint2.value();
		HIDE_PROMPT_MESSAGE;

		MXAPP.CallMain([=]() {
			auto* pAnno = new Mx2dCustomRect(pt1, pt2, textHeight);
			McDbObjectId id = Mx2d::addEntityToAnnotationLayerAndClose(pAnno);
			emit Mx2dSignalTransfer::getInstance().signalAddAnnotation(MxUtils::gCurrentTab, id);
			});
	}
}

void MxCADCommand::Mx_DrawEllipseMark()
{
	double scaled = Mx2d::getViewToDocScaleRatio();
	double textHeight = 25 / scaled;
	// Ellipse is constrained within a rectangle; select diagonal points first
	while (true)
	{
		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select diagonal point 1"));
		MrxDbgUiPrPoint getPoint1(QCoreApplication::translate("MxCADCommand", "Select diagonal point 1").toStdString().c_str());
		MrxDbgUiPrBase::Status ret = getPoint1.go();
		if (ret != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}
		McGePoint3d pt1 = getPoint1.value();

		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select diagonal point 2"));
		MrxDbgUiPrPoint getPoint2(QCoreApplication::translate("MxCADCommand", "Select diagonal point 2").toStdString().c_str());
		// Dynamic preview of the ellipse during selection
		getPoint2.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
			worldDrawEllipse(pt1, pData, pWorldDraw);
			});
		ret = getPoint2.go();
		if (ret != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}
		McGePoint3d pt2 = getPoint2.value();

		HIDE_PROMPT_MESSAGE;

		// Draw ellipse mark in main thread
		MXAPP.CallMain([=]() {
			auto* pAnno = new Mx2dCustomEllipse(pt1, pt2, textHeight);
			McDbObjectId id = Mx2d::addEntityToAnnotationLayerAndClose(pAnno);
			emit Mx2dSignalTransfer::getInstance().signalAddAnnotation(MxUtils::gCurrentTab, id);
			});
	}
}

void MxCADCommand::Mx_DrawRectCloudMark()
{
	while (true)
	{
		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select first corner of rectangular area"));
		MrxDbgUiPrPoint getPoint1(QCoreApplication::translate("MxCADCommand", "Select first corner of rectangular area").toStdString().c_str());
		MrxDbgUiPrBase::Status ret = getPoint1.go();
		if (ret != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}

		double scaled = Mx2d::getViewToDocScaleRatio();
		double textHeight = 25 / scaled;
		McGePoint3d pt1 = getPoint1.value();
		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select second corner of rectangular area"));
		MrxDbgUiPrPoint getPoint2(QCoreApplication::translate("MxCADCommand", "Select second corner of rectangular area").toStdString().c_str());
		getPoint2.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
			worldDrawRectCloud(pt1, textHeight, pData, pWorldDraw);
			});
		ret = getPoint2.go();
		if (ret != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}
		McGePoint3d pt2 = getPoint2.value();


		HIDE_PROMPT_MESSAGE;
		MXAPP.CallMain([=]() {
			auto* pAnno = new Mx2dCustomRectCloud(pt1, pt2, textHeight);
			McDbObjectId id = Mx2d::addEntityToAnnotationLayerAndClose(pAnno);
			emit Mx2dSignalTransfer::getInstance().signalAddAnnotation(MxUtils::gCurrentTab, id);
			});
	}
}

void MxCADCommand::Mx_DrawLeaderMark()
{
	struct resbuf* pRb = Mx::mcedGetArgs();
	if (!pRb || pRb->restype != RTSTR)
	{
		return;
	}
	QString textStr = QString::fromLocal8Bit(pRb->resval.rstring);

	double scaled = Mx2d::getViewToDocScaleRatio();
	double textHeight = 25 / scaled;
	while (true)
	{
		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select leader start point"));
		MrxDbgUiPrPoint getPoint1(QCoreApplication::translate("MxCADCommand", "Select leader start point").toStdString().c_str());
		MrxDbgUiPrBase::Status ret = getPoint1.go();
		if (ret != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}
		McGePoint3d pt1 = getPoint1.value();
		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select leader end point"));
		MrxDbgUiPrPoint getPoint2(QCoreApplication::translate("MxCADCommand", "Select leader end point").toStdString().c_str());
		getPoint2.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
			worldDrawLeader(pt1, textStr, textHeight, pData, pWorldDraw);
			});

		ret = getPoint2.go();
		if (ret != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}
		McGePoint3d pt2 = getPoint2.value();
		HIDE_PROMPT_MESSAGE;
		MXAPP.CallMain([=]() {
			auto* pAnno = new Mx2dCustomLeader(pt1, pt2, textStr, textHeight);
			McDbObjectId id = Mx2d::addEntityToAnnotationLayerAndClose(pAnno);
			emit Mx2dSignalTransfer::getInstance().signalAddAnnotation(MxUtils::gCurrentTab, id);
			});
	}
}

void MxCADCommand::Mx_DrawPolyAreaMark()
{
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select start point"));
	MrxDbgUiPrPoint getStartPoint(QCoreApplication::translate("MxCADCommand", "Select start point").toStdString().c_str());
	MrxDbgUiPrBase::Status ret = getStartPoint.go();
	if (ret != MrxDbgUiPrBase::kOk) return;
	McGePoint3d startPoint = getStartPoint.value();

	McGePoint3dArray pts;
	pts.append(startPoint);

	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select next point"));
	while (true)
	{
		MrxDbgUiPrPoint getNextPoint(QCoreApplication::translate("MxCADCommand", "Select next point").toStdString().c_str());
		getNextPoint.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
			worldDrawPolygon(pts, pData, pWorldDraw);
			});
		ret = getNextPoint.go();
		if (ret == MrxDbgUiPrBase::kOk) {
			pts.append(getNextPoint.value());
			continue;
		}
		else if (ret == MrxDbgUiPrBase::kCancel) {
			HIDE_PROMPT_MESSAGE;
			return;
		}
		else if (ret == MrxDbgUiPrBase::kNone) {
			HIDE_PROMPT_MESSAGE;
			break;
		}
	}
	if (pts.length() < 3)
		return;
	if (isPolygonSelfIntersecting(pts))
	{
		MXAPP.CallMain([=]() {
			emit MxSignalTransfer::getInstance().signalMessageBoxInformation(MxUtils::gCurrentTab, QCoreApplication::translate("MxCADCommand", "Self crossed poly!"));
			});
		return;
	}


	double scaled = Mx2d::getViewToDocScaleRatio();
	double textHeight = 25 / scaled;

	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select text insertion point"));
	MrxDbgUiPrPoint getTextPoint(QCoreApplication::translate("MxCADCommand", "Select text insertion point").toStdString().c_str());
	getTextPoint.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
		worldDrawPolyArea(pts, textHeight, pData, pWorldDraw);
		});

	ret = getTextPoint.go();
	if (ret != MrxDbgUiPrBase::kOk) {
		HIDE_PROMPT_MESSAGE;
		return;
	}
	McGePoint3d textPoint = getTextPoint.value();
	HIDE_PROMPT_MESSAGE;
	MXAPP.CallMain([=]() {
		auto* pAnno = new Mx2dCustomPolyArea(pts, textPoint, textHeight);
		McDbObjectId id = Mx2d::addEntityToAnnotationLayerAndClose(pAnno);
		emit Mx2dSignalTransfer::getInstance().signalAddAnnotation(MxUtils::gCurrentTab, id);
		});
}

void MxCADCommand::Mx_DrawArcPolyAreaMark()
{
	Mx2d::PLVertexList pts;

	while (true)
	{
		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select point or arc segment"));
		MrxDbgUiPrPoint getPt(QCoreApplication::translate("MxCADCommand", "Select point or arc segment").toStdString().c_str());
		getPt.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
			worldDrawPolygon(pts, pData, pWorldDraw);
			});
		MrxDbgUiPrBase::Status ret = getPt.go();
		McGePoint3d pt;
		if (ret == MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			pt = getPt.value();
		}
		else if (ret == MrxDbgUiPrBase::kCancel)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}
		else if (ret == MrxDbgUiPrBase::kNone)
		{
			HIDE_PROMPT_MESSAGE;
			break;
		}

		McGePoint3d arcStartPt, arcEndPt;
		double bulge = 0.0;
		if (Mx2d::getArcSegmentAtPoint(pt, arcStartPt, arcEndPt, bulge))
		{
			if (pts.length() > 0)
			{
				if (pts.last().pt.distanceTo(arcStartPt) < MxDblEpsilon)
				{
					pts.last().bulge = bulge;
					pts.append({ arcEndPt, 0 });
				}
				else if (pts.last().pt.distanceTo(arcEndPt) < MxDblEpsilon)
				{
					pts.last().bulge = -bulge;
					pts.append({ arcStartPt, 0 });
				}
				else
				{
					continue;
				}
			}
			else
			{
				if (pt.distanceTo(arcStartPt) < pt.distanceTo(arcEndPt))
				{
					pts.append({ arcStartPt ,bulge });
					pts.append({ arcEndPt ,0 });
				}
				else
				{
					pts.append({ arcEndPt ,-bulge });
					pts.append({ arcStartPt ,0 });
				}
			}
		}
		else
		{
			pts.append({ pt ,0 });
		}
	}

	if (pts.length() < 3)
		return;

	double scaled = Mx2d::getViewToDocScaleRatio();
	double textHeight = 25 / scaled;

	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select text insertion point"));
	MrxDbgUiPrPoint getTextPoint(QCoreApplication::translate("MxCADCommand", "Select text insertion point").toStdString().c_str());
	getTextPoint.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
		worldDrawArcPolyArea(pts, textHeight, pData, pWorldDraw);
		});
	MrxDbgUiPrBase::Status ret = getTextPoint.go();
	if (ret != MrxDbgUiPrBase::kOk) {
		HIDE_PROMPT_MESSAGE;
		return;
	}
	McGePoint3d textPoint = getTextPoint.value();
	HIDE_PROMPT_MESSAGE;

	MXAPP.CallMain([=]() {
		auto* pAnno = new Mx2dCustomArcPolyArea(pts, textPoint, textHeight);
		McDbObjectId id = Mx2d::addEntityToAnnotationLayerAndClose(pAnno);
		emit Mx2dSignalTransfer::getInstance().signalAddAnnotation(MxUtils::gCurrentTab, id);
		});
}

void MxCADCommand::Mx_DrawRectAreaMark()
{
	double scaled = Mx2d::getViewToDocScaleRatio();
	double textHeight = 25 / scaled;
	while (true)
	{
		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select first corner of rectangle"));
		MrxDbgUiPrPoint getPoint1(QCoreApplication::translate("MxCADCommand", "Select first corner of rectangle").toStdString().c_str());
		MrxDbgUiPrBase::Status ret = getPoint1.go();
		if (ret != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}
		McGePoint3d pt1 = getPoint1.value();
		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select second corner of rectangle"));
		MrxDbgUiPrPoint getPoint2(QCoreApplication::translate("MxCADCommand", "Select second corner of rectangle").toStdString().c_str());
		getPoint2.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
			worldDrawRect(pt1, pData, pWorldDraw);
			});
		ret = getPoint2.go();
		if (ret != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}
		McGePoint3d pt2 = getPoint2.value();
		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select text insertion point"));
		MrxDbgUiPrPoint getTextPoint(QCoreApplication::translate("MxCADCommand", "Select text insertion point").toStdString().c_str());
		getTextPoint.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
			worldDrawRectArea(pt1, pt2, textHeight, pData, pWorldDraw);
			});
		ret = getTextPoint.go();
		if (ret != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}
		McGePoint3d textPoint = getTextPoint.value();

		HIDE_PROMPT_MESSAGE;

		MXAPP.CallMain([=]() {
			auto* pAnno = new Mx2dCustomRectArea(pt1, pt2, textPoint, textHeight);
			McDbObjectId id = Mx2d::addEntityToAnnotationLayerAndClose(pAnno);
			emit Mx2dSignalTransfer::getInstance().signalAddAnnotation(MxUtils::gCurrentTab, id);
			});
	}
}

void MxCADCommand::Mx_DrawCartesianCoordMark()
{
	double viewToDocScale, textHeight;
	bool isFirstAnnotation = true;

	while (true)
	{
		// Prompt to select point for Cartesian coordinate annotation
		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select point to annotate"));
		MrxDbgUiPrPoint getTargetPoint(QCoreApplication::translate("MxCADCommand", "Select point to annotate").toStdString().c_str());
		MrxDbgUiPrBase::Status retStatus = getTargetPoint.go();

		if (retStatus != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}
		McGePoint3d targetPoint = getTargetPoint.value();

		// Get view-to-document scale on first annotation (reuse for subsequent annotations)
		if (isFirstAnnotation)
		{
			viewToDocScale = Mx2d::getViewToDocScaleRatio();
            textHeight = 25 / viewToDocScale;
			isFirstAnnotation = false;
		}

		// Prompt to select text insertion point
		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select text insertion point"));
		MrxDbgUiPrPoint getTextInsertPoint(QCoreApplication::translate("MxCADCommand", "Select text insertion point").toStdString().c_str());
		// Dynamic preview of Cartesian coordinate annotation during selection
		getTextInsertPoint.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
			worldDrawCartesianCoord(targetPoint, textHeight, pData, pWorldDraw);
			});
		retStatus = getTextInsertPoint.go();

		if (retStatus != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}
		McGePoint3d textInsertPoint = getTextInsertPoint.value();

		HIDE_PROMPT_MESSAGE;

		// Create and add Cartesian coordinate annotation in main thread
		MXAPP.CallMain([=]() {
			auto* pAnno = new Mx2dCustomCartesianCoord(targetPoint, textInsertPoint, textHeight);
			McDbObjectId id = Mx2d::addEntityToAnnotationLayerAndClose(pAnno);
			emit Mx2dSignalTransfer::getInstance().signalAddAnnotation(MxUtils::gCurrentTab, id);
			});
	}
}

void MxCADCommand::Mx_DrawAlignedDimMark()
{
	// Select start and end points for aligned dimension
	double viewToDocScale = Mx2d::getViewToDocScaleRatio();
	double textHeight = 25 / viewToDocScale;
	while (true)
	{
		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select aligned dimension start point"));
		MrxDbgUiPrPoint getStartPoint(QCoreApplication::translate("MxCADCommand", "Select aligned dimension start point").toStdString().c_str());
		MrxDbgUiPrBase::Status retStatus = getStartPoint.go();

		if (retStatus != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}
		McGePoint3d startPoint = getStartPoint.value();

		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select aligned dimension end point"));
		MrxDbgUiPrPoint getEndPoint(QCoreApplication::translate("MxCADCommand", "Select aligned dimension end point").toStdString().c_str());
		// Set base point for rubber band preview
		getEndPoint.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
			worldDrawLine(startPoint, pData, pWorldDraw);
			});
		retStatus = getEndPoint.go();

		if (retStatus != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}
		McGePoint3d endPoint = getEndPoint.value();

		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select aligned dimension text insertion point"));
		MrxDbgUiPrPoint getTextInsertPoint(QCoreApplication::translate("MxCADCommand", "Select aligned dimension text insertion point").toStdString().c_str());
		// Dynamic preview of aligned dimension during selection
		getTextInsertPoint.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
			worldDrawAlignedDim(startPoint, endPoint, textHeight, pData, pWorldDraw);
			});
		retStatus = getTextInsertPoint.go();

		if (retStatus != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}
		McGePoint3d textInsertPoint = getTextInsertPoint.value();

		HIDE_PROMPT_MESSAGE;

		// Create and add aligned dimension in main thread
		MXAPP.CallMain([=]() {
			auto* pAnno = new Mx2dCustomAlignedDim(startPoint, endPoint, textInsertPoint,textHeight);
			McDbObjectId id = Mx2d::addEntityToAnnotationLayerAndClose(pAnno);
			emit Mx2dSignalTransfer::getInstance().signalAddAnnotation(MxUtils::gCurrentTab, id);
			});
	}
}

void MxCADCommand::Mx_DrawLinearDimMark()
{
	// Select start and end points for linear dimension
	double viewToDocScale = Mx2d::getViewToDocScaleRatio();
	double textHeight = 25 / viewToDocScale;
	while (true)
	{
		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select linear dimension start point"));
		MrxDbgUiPrPoint getStartPoint(QCoreApplication::translate("MxCADCommand", "Select linear dimension start point").toStdString().c_str());
		MrxDbgUiPrBase::Status retStatus = getStartPoint.go();

		if (retStatus != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}
		McGePoint3d startPoint = getStartPoint.value();

		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select linear dimension end point"));
		MrxDbgUiPrPoint getEndPoint(QCoreApplication::translate("MxCADCommand", "Select linear dimension end point").toStdString().c_str());
		// Set base point for rubber band preview
		getEndPoint.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
			worldDrawLine(startPoint, pData, pWorldDraw);
			});
		retStatus = getEndPoint.go();

		if (retStatus != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}
		McGePoint3d endPoint = getEndPoint.value();

		// Create temporary linear dimension object for dynamic preview
		Mx2dCustomLinearDim* pLinearDim = new Mx2dCustomLinearDim(startPoint, endPoint, endPoint, textHeight);

		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select linear dimension text insertion point"));
		MrxDbgUiPrPoint getTextInsertPoint(QCoreApplication::translate("MxCADCommand", "Select linear dimension text insertion point").toStdString().c_str());
		// Dynamic preview of linear dimension during selection
		getTextInsertPoint.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
			worldDrawLinearDim(pLinearDim, pData, pWorldDraw);
			});
		retStatus = getTextInsertPoint.go();

		if (retStatus != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			delete pLinearDim; // Clean up temporary object on cancel
			return;
		}
		McGePoint3d textInsertPoint = getTextInsertPoint.value();

		HIDE_PROMPT_MESSAGE;

		// Set dimension properties and add to drawing in main thread
		MXAPP.CallMain([=]() {
			McDbObjectId id = Mx2d::addEntityToAnnotationLayerAndClose(pLinearDim);
			emit Mx2dSignalTransfer::getInstance().signalAddAnnotation(MxUtils::gCurrentTab, id);
			});
	}
}

void MxCADCommand::Mx_DrawContinuousMeasurementMark()
{
	Mx2d::PLVertexList polylineVertices;

	while (true)
	{
		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select point or arc segment"));
		MrxDbgUiPrPoint getTargetPoint(QCoreApplication::translate("MxCADCommand", "Select point or arc segment").toStdString().c_str());
		// Dynamic preview of the polyline during selection
		getTargetPoint.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
			worldDrawPolygon(polylineVertices, pData, pWorldDraw);
			});
		MrxDbgUiPrBase::Status retStatus = getTargetPoint.go();
		McGePoint3d selectedPoint;

		if (retStatus == MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			selectedPoint = getTargetPoint.value();
		}
		else if (retStatus == MrxDbgUiPrBase::kCancel)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}
		else if (retStatus == MrxDbgUiPrBase::kNone)
		{
			HIDE_PROMPT_MESSAGE;
			break;
		}

		McGePoint3d arcStartPt, arcEndPt;
		double bulgeValue = 0.0;
		// Check if selected position is on an arc segment
		if (Mx2d::getArcSegmentAtPoint(selectedPoint, arcStartPt, arcEndPt, bulgeValue))
		{
			if (!polylineVertices.isEmpty())
			{
				// Connect to existing polyline (match arc start/end with last vertex)
				if (polylineVertices.last().pt.distanceTo(arcStartPt) < MxDblEpsilon)
				{
					polylineVertices.last().bulge = bulgeValue;
					polylineVertices.append({ arcEndPt, 0 });
				}
				else if (polylineVertices.last().pt.distanceTo(arcEndPt) < MxDblEpsilon)
				{
					polylineVertices.last().bulge = -bulgeValue;
					polylineVertices.append({ arcStartPt, 0 });
				}
				else
				{
					continue;
				}
			}
			else
			{
				// Start new polyline with arc segment (based on closest arc endpoint)
				if (selectedPoint.distanceTo(arcStartPt) < selectedPoint.distanceTo(arcEndPt))
				{
					polylineVertices.append({ arcStartPt, bulgeValue });
					polylineVertices.append({ arcEndPt, 0 });
				}
				else
				{
					polylineVertices.append({ arcEndPt, -bulgeValue });
					polylineVertices.append({ arcStartPt, 0 });
				}
			}
		}
		else
		{
			// Add regular point (non-arc) to polyline
			polylineVertices.append({ selectedPoint, 0 });
		}
	}

	if (polylineVertices.length() < 2)
		return;
	double viewToDocScale = Mx2d::getViewToDocScaleRatio();
	double textHeight = 25 / viewToDocScale;
	// Prompt to select text insertion point
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select text insertion point"));
	MrxDbgUiPrPoint getTextInsertPoint(QCoreApplication::translate("MxCADCommand", "Select text insertion point").toStdString().c_str());
	// Dynamic preview of continuous measurement
	getTextInsertPoint.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
		worldDrawContinuousMeasurement(polylineVertices, textHeight, pData, pWorldDraw);
		});
	MrxDbgUiPrBase::Status retStatus = getTextInsertPoint.go();
	if (retStatus != MrxDbgUiPrBase::kOk)
	{
		HIDE_PROMPT_MESSAGE;
		return;
	}
	McGePoint3d textInsertPoint = getTextInsertPoint.value();
	HIDE_PROMPT_MESSAGE;

	// Create and add continuous measurement mark in main thread
	MXAPP.CallMain([=]() {
		auto* pAnno = new Mx2dCustomContinuousMeasurement(polylineVertices, textInsertPoint, textHeight);
		McDbObjectId id = Mx2d::addEntityToAnnotationLayerAndClose(pAnno);
		emit Mx2dSignalTransfer::getInstance().signalAddAnnotation(MxUtils::gCurrentTab, id);
		});
}

void MxCADCommand::Mx_ShowSegmentLengths()
{
	Mx2dCustomContinuousMeasurement* pContinuousMark = nullptr;
	double viewToDocScale = Mx2d::getViewToDocScaleRatio();
	double textHeight = 25 / viewToDocScale;
	while (true)
	{
		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select continuous measurement object"));
		MrxDbgUiPrPoint getSelectPoint(QCoreApplication::translate("MxCADCommand", "Select continuous measurement object").toStdString().c_str());
		// Dynamic preview of segment lengths during selection
		getSelectPoint.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
			worldDrawSegmentLengths(pContinuousMark, textHeight, pData, pWorldDraw);
			});
		MrxDbgUiPrBase::Status retStatus = getSelectPoint.go();

		// Close previous mark object if exists
		if (pContinuousMark)
		{
			pContinuousMark->close();
		}

		// Exit if selection is canceled
		if (retStatus != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}

		McGePoint3d selectedPoint = getSelectPoint.value();
		// Find entity at selected point
		McDbObjectId targetEntId = MrxDbgUtils::findEntAtPoint(selectedPoint);
		if (targetEntId.isNull())
		{
			HIDE_PROMPT_MESSAGE;
			continue;
		}

		// Get view-to-document scale
		viewToDocScale = Mx2d::getViewToDocScaleRatio();
		textHeight = 25 / viewToDocScale;

		McDbObject* pObject = nullptr;
		Mcad::ErrorStatus es = Mx::mcdbOpenMcDbObject(pObject, targetEntId, McDb::kForRead);
		if (es == Mcad::eOk)
		{
			// Cast to continuous measurement object
			Mx2dCustomContinuousMeasurement* pTempMark = Mx2dCustomContinuousMeasurement::cast(pObject);
			if (pTempMark)
			{
				pContinuousMark = pTempMark;
			}
		}
	}
}

void MxCADCommand::Mx_DrawRadiusDimMark()
{
	McDbObjectId targetEntId;
	McGePoint3d pickPoint;

	// Prompt to select a circle or arc
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select a circle or arc"));
	if (MrxDbgUtils::selectEnt(QCoreApplication::translate("MxCADCommand", "Select a circle or arc").toStdString().c_str(), targetEntId, pickPoint/*, spFilter.data()*/) != RTNORM)
	{
		HIDE_PROMPT_MESSAGE;
		return;
	}

	// Open selected entity for read
	McDbEntityPointer spEntity(targetEntId, McDb::kForRead);
	if (spEntity.openStatus() != Mcad::eOk)
	{
		HIDE_PROMPT_MESSAGE;
		return;
	}

	// Handle Arc or Polyline (arc segment)
	if (spEntity->isA() == McDbArc::desc() || spEntity->isA() == McDbPolyline::desc())
	{
		McGePoint3d arcStartPt, arcEndPt, arcMidPt;

		if (spEntity->isA() == McDbArc::desc())
		{
			// Get arc properties: start/end/mid points
			McDbArc* pArc = McDbArc::cast(spEntity.object());
			pArc->getStartPoint(arcStartPt);
			pArc->getEndPoint(arcEndPt);

			double startParam = 0.0, endParam = 0.0, midParam = 0.0;
			pArc->getStartParam(startParam);
			pArc->getEndParam(endParam);
			midParam = (startParam + endParam) / 2.0;
			pArc->getPointAtParam(midParam, arcMidPt);
		}
		else
		{
			// Handle polyline arc segment
			McDbPolyline* pPolyline = McDbPolyline::cast(spEntity.object());
			int arcVertIndex = Mx2d::indexArcBefore(pickPoint, pPolyline);

			// Exit if pick point is not on a polyline arc segment
			if (arcVertIndex == -1)
			{
				HIDE_PROMPT_MESSAGE;
				return;
			}

			// Get arc segment start/end points from polyline
			pPolyline->getPointAt(arcVertIndex, arcStartPt);
			if (arcVertIndex == pPolyline->numVerts() - 1 && pPolyline->isClosed())
			{
				pPolyline->getStartPoint(arcEndPt);
			}
			else
			{
				pPolyline->getPointAt(arcVertIndex + 1, arcEndPt);
			}

			// Calculate arc center, radius and mid point using bulge value
			double bulge = 0.0;
			pPolyline->getBulgeAt(arcVertIndex, bulge);

			McGePoint3d arcCenter;
			double radius = 0.0, totalAngle = 0.0;
			Mx2d::calcArc(arcStartPt, arcEndPt, bulge, arcCenter, radius, totalAngle);

			double midAngleRot = totalAngle / 2.0;
			McGeVector3d midPointVec(arcStartPt - arcCenter);

			// Reverse rotation for clockwise arc (negative bulge)
			if (bulge < 0.0)
			{
				midAngleRot = -midAngleRot;
			}

			midPointVec.rotateBy(midAngleRot, McGeVector3d::kZAxis);
			arcMidPt = arcCenter + midPointVec;
		}

		// Create temporary radius dimension for dynamic preview
		Mx2dCustomRadiusDim* pRadiusDim = new Mx2dCustomRadiusDim(arcStartPt, arcMidPt, arcEndPt, McGePoint3d(), 1.0);

		// Prompt to select dimension placement point
		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select dimension placement point"));
		MrxDbgUiPrPoint getPlacementPoint(QCoreApplication::translate("MxCADCommand", "Select dimension placement point").toStdString().c_str());
		getPlacementPoint.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
			worldDrawRadiusDim(pRadiusDim, pData, pWorldDraw);
			});

		MrxDbgUiPrBase::Status retStatus = getPlacementPoint.go();
		if (retStatus != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}

		// Add radius dimension to drawing in main thread
		MXAPP.CallMain([&]() {
			McDbObjectId id = Mx2d::addEntityToAnnotationLayerAndClose(pRadiusDim);
			emit Mx2dSignalTransfer::getInstance().signalAddAnnotation(MxUtils::gCurrentTab, id);
			});
	}
	// Handle Circle
	else if (spEntity->isA() == McDbCircle::desc())
	{
		McDbCircle* pCircle = McDbCircle::cast(spEntity.object());
		McGePoint3d circleCenter = pCircle->center();
		double circleRadius = pCircle->radius();

		// Create temporary radius dimension for dynamic preview
		Mx2dCustomRadiusDim* pRadiusDim = new Mx2dCustomRadiusDim(circleCenter, circleRadius, McGePoint3d(), 1.0);

		// Prompt to select dimension placement point
		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select dimension placement point"));
		MrxDbgUiPrPoint getPlacementPoint(QCoreApplication::translate("MxCADCommand", "Select dimension placement point").toStdString().c_str());
		getPlacementPoint.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
			worldDrawRadiusDim(pRadiusDim, pData, pWorldDraw);
			});

		MrxDbgUiPrBase::Status retStatus = getPlacementPoint.go();
		if (retStatus != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}

		// Add radius dimension to drawing in main thread
		MXAPP.CallMain([&]() {
			McDbObjectId id = Mx2d::addEntityToAnnotationLayerAndClose(pRadiusDim);
			emit Mx2dSignalTransfer::getInstance().signalAddAnnotation(MxUtils::gCurrentTab, id);
			});
	}
	// Unsupported entity type
	else
	{
		HIDE_PROMPT_MESSAGE;
		return;
	}

	HIDE_PROMPT_MESSAGE;
}

void MxCADCommand::Mx_DrawArcLengthDimMark()
{
	McDbObjectId targetEntId;
	McGePoint3d pickPoint;

	// Prompt to select an arc
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select an arc"));
	if (MrxDbgUtils::selectEnt(QCoreApplication::translate("MxCADCommand", "Select an arc").toStdString().c_str(), targetEntId, pickPoint/*, spFilter.data()*/) != RTNORM)
	{
		HIDE_PROMPT_MESSAGE;
		return;
	}

	// Open selected entity for read
	McDbEntityPointer spEntity(targetEntId, McDb::kForRead);
	if (spEntity.openStatus() != Mcad::eOk)
	{
		HIDE_PROMPT_MESSAGE;
		return;
	}

	// Handle standalone Arc
	if (spEntity->isA() == McDbArc::desc())
	{
		McDbArc* pArc = McDbArc::cast(spEntity.object());
		McGePoint3d arcStartPt, arcEndPt, arcMidPt;

		// Get arc key points: start, end, and midpoint
		pArc->getStartPoint(arcStartPt);
		pArc->getEndPoint(arcEndPt);

		double startParam = 0.0, endParam = 0.0, midParam = 0.0;
		pArc->getStartParam(startParam);
		pArc->getEndParam(endParam);
		midParam = (startParam + endParam) / 2.0;
		pArc->getPointAtParam(midParam, arcMidPt);

		// Prompt to select dimension placement point
		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select dimension placement point"));
		MrxDbgUiPrPoint getPlacementPoint(QCoreApplication::translate("MxCADCommand", "Select dimension placement point").toStdString().c_str());
		double viewToDocScale = Mx2d::getViewToDocScaleRatio();
		double textHeight = 25 / viewToDocScale;
		// Dynamic preview of arc length dimension
		getPlacementPoint.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
			worldDrawArcLengthDim(arcStartPt, arcMidPt, arcEndPt, textHeight, pData, pWorldDraw);
			});

		MrxDbgUiPrBase::Status retStatus = getPlacementPoint.go();
		if (retStatus != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}

		McGePoint3d dimPlacementPoint = getPlacementPoint.value();

		// Create and add arc length dimension in main thread
		MXAPP.CallMain([&]() {
			auto* pAnno = new Mx2dCustomArcLengthDim(arcStartPt, arcMidPt, arcEndPt, dimPlacementPoint, textHeight);
			McDbObjectId id = Mx2d::addEntityToAnnotationLayerAndClose(pAnno);
			emit Mx2dSignalTransfer::getInstance().signalAddAnnotation(MxUtils::gCurrentTab, id);
			});
	}
	// Handle Polyline arc segment
	else if (spEntity->isA() == McDbPolyline::desc())
	{
		McDbPolyline* pPolyline = McDbPolyline::cast(spEntity.object());
		int arcVertIndex = Mx2d::indexArcBefore(pickPoint, pPolyline);

		// Exit if pick point is not on a polyline arc segment
		if (arcVertIndex == -1)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}

		// Get arc segment start/end points from polyline
		McGePoint3d arcStartPt, arcEndPt;
		pPolyline->getPointAt(arcVertIndex, arcStartPt);

		if (arcVertIndex == pPolyline->numVerts() - 1 && pPolyline->isClosed())
		{
			pPolyline->getStartPoint(arcEndPt);
		}
		else
		{
			pPolyline->getPointAt(arcVertIndex + 1, arcEndPt);
		}

		// Calculate arc center, radius, midpoint using bulge value
		double bulge = 0.0;
		pPolyline->getBulgeAt(arcVertIndex, bulge);

		McGePoint3d arcCenter;
		double radius = 0.0, totalAngle = 0.0;
		Mx2d::calcArc(arcStartPt, arcEndPt, bulge, arcCenter, radius, totalAngle);

		double midAngleRot = totalAngle / 2.0;
		McGeVector3d midPointVec(arcStartPt - arcCenter);

		// Reverse rotation for clockwise arc (negative bulge)
		if (bulge < 0.0)
		{
			midAngleRot = -midAngleRot;
		}

		midPointVec.rotateBy(midAngleRot, McGeVector3d::kZAxis);
		McGePoint3d arcMidPt = arcCenter + midPointVec;

		// Prompt to select dimension placement point
		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select dimension placement point"));
		MrxDbgUiPrPoint getPlacementPoint(QCoreApplication::translate("MxCADCommand", "Select dimension placement point").toStdString().c_str());
		double viewToDocScale = Mx2d::getViewToDocScaleRatio();
		double textHeight = 25 / viewToDocScale;
		// Dynamic preview of arc length dimension
		getPlacementPoint.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
			worldDrawArcLengthDim(arcStartPt, arcMidPt, arcEndPt, textHeight, pData, pWorldDraw);
			});

		MrxDbgUiPrBase::Status retStatus = getPlacementPoint.go();
		if (retStatus != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}

		McGePoint3d dimPlacementPoint = getPlacementPoint.value();

		// Create and add arc length dimension in main thread
		MXAPP.CallMain([&]() {
			auto* pAnno = new Mx2dCustomArcLengthDim(arcStartPt, arcMidPt, arcEndPt, dimPlacementPoint, textHeight);
			McDbObjectId id = Mx2d::addEntityToAnnotationLayerAndClose(pAnno);
			emit Mx2dSignalTransfer::getInstance().signalAddAnnotation(MxUtils::gCurrentTab, id);
			});
	}
	// Unsupported entity type
	else
	{
		HIDE_PROMPT_MESSAGE;
		return;
	}

	HIDE_PROMPT_MESSAGE;
}

void MxCADCommand::Mx_DrawCircleMeasurementMark()
{
	// Filter to select only CIRCLE entities
	MrxDbgRbList entityFilter = Mx::mcutBuildList(RTDXF0, "CIRCLE", 0);
	McDbObjectId targetEntId;
	McGePoint3d pickPoint;

	// Prompt to select a circle
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select a circle"));
	if (MrxDbgUtils::selectEnt(QCoreApplication::translate("MxCADCommand", "Select a circle").toStdString().c_str(), targetEntId, pickPoint, entityFilter.data()) != RTNORM)
	{
		HIDE_PROMPT_MESSAGE;
		return;
	}

	// Open selected entity for read
	McDbEntityPointer spEntity(targetEntId, McDb::kForRead);
	if (spEntity.openStatus() != Mcad::eOk)
	{
		HIDE_PROMPT_MESSAGE;
		return;
	}

	// Handle Circle entity
	if (spEntity->isA() == McDbCircle::desc())
	{
		McDbCircle* pCircle = McDbCircle::cast(spEntity.object());
		McGePoint3d circleCenter = pCircle->center();
		double circleRadius = pCircle->radius();

		// Prompt to select measurement placement point
		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select measurement placement point"));
		MrxDbgUiPrPoint getPlacementPoint(QCoreApplication::translate("MxCADCommand", "Select measurement placement point").toStdString().c_str());
		double viewToDocScale = Mx2d::getViewToDocScaleRatio();
		double textHeight = 25 / viewToDocScale;
		// Dynamic preview of circle measurement
		getPlacementPoint.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
			worldDrawCircleMeasurement(circleCenter, circleRadius, textHeight, pData, pWorldDraw);
			});

		MrxDbgUiPrBase::Status retStatus = getPlacementPoint.go();
		if (retStatus != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}

		McGePoint3d measurementPlacementPoint = getPlacementPoint.value();

		// Create and add circle measurement mark in main thread
		MXAPP.CallMain([&]() {
			auto* pAnno = new Mx2dCustomCircleMeasurement(circleCenter, circleRadius, measurementPlacementPoint, textHeight);
			McDbObjectId id = Mx2d::addEntityToAnnotationLayerAndClose(pAnno);
			emit Mx2dSignalTransfer::getInstance().signalAddAnnotation(MxUtils::gCurrentTab, id);
			});
	}
	// Unsupported entity type
	else
	{
		HIDE_PROMPT_MESSAGE;
		return;
	}

	HIDE_PROMPT_MESSAGE;
}

void MxCADCommand::Mx_UpdateDisplay()
{
	MxDrawApp::UpdateDisplay();
}

void MxCADCommand::Mx_DrawDistPointToLineMark()
{
	// Prompt to select a point
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select a point"));
	MrxDbgUiPrPoint getTargetPoint(QCoreApplication::translate("MxCADCommand", "Select a point").toStdString().c_str());
	MrxDbgUiPrBase::Status retStatus = getTargetPoint.go();
	if (retStatus != MrxDbgUiPrBase::kOk)
	{
		HIDE_PROMPT_MESSAGE;
		return;
	}
	McGePoint3d targetPoint = getTargetPoint.value();

	// Prompt to select a line
	McDbObjectId targetEntId;
	McGePoint3d pickPoint;
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select a line"));
	if (MrxDbgUtils::selectEnt(QCoreApplication::translate("MxCADCommand", "Select a line").toStdString().c_str(), targetEntId, pickPoint/*, spFilter.data()*/) != RTNORM)
	{
		HIDE_PROMPT_MESSAGE;
		return;
	}

	// Open selected entity for read
	McDbEntityPointer spEntity(targetEntId, McDb::kForRead);
	if (spEntity.openStatus() != Mcad::eOk)
	{
		HIDE_PROMPT_MESSAGE;
		return;
	}

	// Handle standalone Line entity
	if (spEntity->isA() == McDbLine::desc())
	{
		McDbLine* pLine = McDbLine::cast(spEntity.object());

		// Calculate vectors for projection (2D: ignore Z-axis)
		McGeVector3d lineVec(pLine->endPoint() - pLine->startPoint());
		McGeVector3d pointToLineVec(targetPoint - pLine->startPoint());

		Mx2d::Vector2d lineVec2d(lineVec.x, lineVec.y);
		Mx2d::Vector2d pointVec2d(pointToLineVec.x, pointToLineVec.y);
		Mx2d::Vector2d projectedVec2d = Mx2d::vectorProjection(pointVec2d, lineVec2d);

		// Get projection point (foot of perpendicular from target point to line)
		McGePoint3d projPoint = pLine->startPoint() + McGeVector3d(projectedVec2d.x, projectedVec2d.y, 0);
		double viewToDocScale = Mx2d::getViewToDocScaleRatio();
		double textHeight = 25 / viewToDocScale;
		// Prompt to select aligned dimension text insertion point
		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select aligned dimension text insertion point"));
		MrxDbgUiPrPoint getTextInsertPoint(QCoreApplication::translate("MxCADCommand", "Select aligned dimension text insertion point").toStdString().c_str());
		// Dynamic preview of the distance dimension
		getTextInsertPoint.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
			worldDrawAlignedDim(targetPoint, projPoint, textHeight, pData, pWorldDraw);
			});

		retStatus = getTextInsertPoint.go();
		if (retStatus != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}
		McGePoint3d textInsertPoint = getTextInsertPoint.value();

		HIDE_PROMPT_MESSAGE;

		// Create and add point-to-line distance dimension in main thread
		MXAPP.CallMain([=]() {
			auto* pAnno = new Mx2dCustomAlignedDim(targetPoint, projPoint, textInsertPoint, textHeight);
			McDbObjectId id = Mx2d::addEntityToAnnotationLayerAndClose(pAnno);
			emit Mx2dSignalTransfer::getInstance().signalAddAnnotation(MxUtils::gCurrentTab, id);
			});
	}
	// Handle Polyline line segment
	else if (spEntity->isA() == McDbPolyline::desc())
	{
		McDbPolyline* pPolyline = McDbPolyline::cast(spEntity.object());
		int lineVertIndex = Mx2d::indexLineBefore(pickPoint, pPolyline);

		// Exit if pick point is not on a polyline line segment
		if (lineVertIndex == -1)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}

		// Get line segment start/end points from polyline
		McGePoint3d segStartPt, segEndPt;
		pPolyline->getPointAt(lineVertIndex, segStartPt);
		if (lineVertIndex == pPolyline->numVerts() - 1 && pPolyline->isClosed())
		{
			pPolyline->getStartPoint(segEndPt);
		}
		else
		{
			pPolyline->getPointAt(lineVertIndex + 1, segEndPt);
		}

		// Calculate vectors for projection (2D: ignore Z-axis)
		McGeVector3d lineVec(segEndPt - segStartPt);
		McGeVector3d pointToLineVec(targetPoint - segStartPt);

		Mx2d::Vector2d lineVec2d(lineVec.x, lineVec.y);
		Mx2d::Vector2d pointVec2d(pointToLineVec.x, pointToLineVec.y);
		Mx2d::Vector2d projectedVec2d = Mx2d::vectorProjection(pointVec2d, lineVec2d);

		// Get projection point (foot of perpendicular from target point to line segment)
		McGePoint3d projPoint = segStartPt + McGeVector3d(projectedVec2d.x, projectedVec2d.y, 0);
		double viewToDocScale = Mx2d::getViewToDocScaleRatio();
		double textHeight = 25 / viewToDocScale;
		// Prompt to select aligned dimension text insertion point
		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select aligned dimension text insertion point"));
		MrxDbgUiPrPoint getTextInsertPoint(QCoreApplication::translate("MxCADCommand", "Select aligned dimension text insertion point").toStdString().c_str());
		// Dynamic preview of the distance dimension
		getTextInsertPoint.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
			worldDrawAlignedDim(targetPoint, projPoint, textHeight, pData, pWorldDraw);
			});

		retStatus = getTextInsertPoint.go();
		if (retStatus != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}
		McGePoint3d textInsertPoint = getTextInsertPoint.value();

		HIDE_PROMPT_MESSAGE;

		// Create and add point-to-line distance dimension in main thread
		MXAPP.CallMain([=]() {
			auto* pAnno = new Mx2dCustomAlignedDim(targetPoint, projPoint, textInsertPoint, textHeight);
			McDbObjectId id = Mx2d::addEntityToAnnotationLayerAndClose(pAnno);
			emit Mx2dSignalTransfer::getInstance().signalAddAnnotation(MxUtils::gCurrentTab, id);
			});
	}
	// Unsupported entity type
	else
	{
		HIDE_PROMPT_MESSAGE;
		return;
	}

	HIDE_PROMPT_MESSAGE;
}

void MxCADCommand::Mx_DrawAngleMeasurementMark()
{
	McDbObjectId firstLineEntId;
	McGePoint3d firstPickPoint;

	// Prompt to select first line
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select first line"));
	if (MrxDbgUtils::selectEnt(QCoreApplication::translate("MxCADCommand", "Select first line").toStdString().c_str(), firstLineEntId, firstPickPoint/*, spFilter.data()*/) != RTNORM)
	{
		HIDE_PROMPT_MESSAGE;
		return;
	}

	// Open first selected entity for read
	McDbEntityPointer spFirstEntity(firstLineEntId, McDb::kForRead);
	if (spFirstEntity.openStatus() != Mcad::eOk)
	{
		HIDE_PROMPT_MESSAGE;
		return;
	}
	// Validate entity type (Line or Polyline)
	if (spFirstEntity->isA() != McDbLine::desc() && spFirstEntity->isA() != McDbPolyline::desc())
	{
		HIDE_PROMPT_MESSAGE;
		return;
	}

	McDbObjectId secondLineEntId;
	McGePoint3d secondPickPoint;

	// Prompt to select second line
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select second line"));
	if (MrxDbgUtils::selectEnt(QCoreApplication::translate("MxCADCommand", "Select second line").toStdString().c_str(), secondLineEntId, secondPickPoint/*, spFilter.data()*/) != RTNORM)
	{
		HIDE_PROMPT_MESSAGE;
		return;
	}

	// Open second selected entity for read
	McDbEntityPointer spSecondEntity(secondLineEntId, McDb::kForRead);
	if (spSecondEntity.openStatus() != Mcad::eOk)
	{
		HIDE_PROMPT_MESSAGE;
		return;
	}
	// Validate entity type (Line or Polyline)
	if (spSecondEntity->isA() != McDbLine::desc() && spSecondEntity->isA() != McDbPolyline::desc())
	{
		HIDE_PROMPT_MESSAGE;
		return;
	}

	// Get start/end points of first line (supports Line and Polyline line segment)
	McGePoint3d line1Start, line1End;
	if (spFirstEntity->isA() == McDbLine::desc())
	{
		McDbLine* pFirstLine = McDbLine::cast(spFirstEntity.object());
		pFirstLine->getStartPoint(line1Start);
		pFirstLine->getEndPoint(line1End);
	}
	else if (spFirstEntity->isA() == McDbPolyline::desc())
	{
		McDbPolyline* pFirstPolyline = McDbPolyline::cast(spFirstEntity.object());
		int line1VertIndex = Mx2d::indexLineBefore(firstPickPoint, pFirstPolyline);

		// Exit if pick point is not on a polyline line segment
		if (line1VertIndex == -1)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}

		pFirstPolyline->getPointAt(line1VertIndex, line1Start);
		if (line1VertIndex == pFirstPolyline->numVerts() - 1 && pFirstPolyline->isClosed())
		{
			pFirstPolyline->getStartPoint(line1End);
		}
		else
		{
			pFirstPolyline->getPointAt(line1VertIndex + 1, line1End);
		}
	}

	// Get start/end points of second line (supports Line and Polyline line segment)
	McGePoint3d line2Start, line2End;
	if (spSecondEntity->isA() == McDbLine::desc())
	{
		McDbLine* pSecondLine = McDbLine::cast(spSecondEntity.object());
		pSecondLine->getStartPoint(line2Start);
		pSecondLine->getEndPoint(line2End);
	}
	else if (spSecondEntity->isA() == McDbPolyline::desc())
	{
		McDbPolyline* pSecondPolyline = McDbPolyline::cast(spSecondEntity.object());
		int line2VertIndex = Mx2d::indexLineBefore(secondPickPoint, pSecondPolyline);

		// Fix: Correct comment (line segment, not arc segment)
		if (line2VertIndex == -1)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}

		pSecondPolyline->getPointAt(line2VertIndex, line2Start);
		if (line2VertIndex == pSecondPolyline->numVerts() - 1 && pSecondPolyline->isClosed())
		{
			pSecondPolyline->getStartPoint(line2End);
		}
		else
		{
			pSecondPolyline->getPointAt(line2VertIndex + 1, line2End);
		}
	}

	double viewToDocScale = Mx2d::getViewToDocScaleRatio();
	double textHeight = 25 / viewToDocScale;
	// Prompt to select angle measurement placement point
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select angle measurement placement point"));
	MrxDbgUiPrPoint getPlacementPoint(QCoreApplication::translate("MxCADCommand", "Select angle measurement placement point").toStdString().c_str());

	// Dynamic preview of angle measurement
	getPlacementPoint.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
		worldDrawAngleMeasurement(line1Start, line1End, line2Start, line2End, textHeight, pData, pWorldDraw);
		});

	MrxDbgUiPrBase::Status retStatus = getPlacementPoint.go();
	if (retStatus != MrxDbgUiPrBase::kOk)
	{
		HIDE_PROMPT_MESSAGE;
		return;
	}

	McGePoint3d measurementPlacementPoint = getPlacementPoint.value();
	HIDE_PROMPT_MESSAGE;

	// Create and add angle measurement mark in main thread
	MXAPP.CallMain([=]() {
		auto* pAnno = new Mx2dCustomAngleMeasurement(line1Start, line1End, line2Start, line2End, measurementPlacementPoint, textHeight);
		McDbObjectId id = Mx2d::addEntityToAnnotationLayerAndClose(pAnno);
		emit Mx2dSignalTransfer::getInstance().signalAddAnnotation(MxUtils::gCurrentTab, id);
		});
}

void MxCADCommand::Mx_CalculateSiderArea()
{
	// Prompt to select area annotation entity
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select area annotation entity"));
	McDbObjectId targetEntId;
	McGePoint3d pickPoint;

	if (MrxDbgUtils::selectEnt(QCoreApplication::translate("MxCADCommand", "Select area annotation entity").toStdString().c_str(), targetEntId, pickPoint/*, spFilter.data()*/) != RTNORM)
	{
		HIDE_PROMPT_MESSAGE;
		return;
	}
	HIDE_PROMPT_MESSAGE;

	// Open selected entity for read
	McDbEntityPointer spEntity(targetEntId, McDb::kForRead);
	Mx2d::PLVertexList polylineVertices;

	// Handle Arc Polyline Area entity (reverse bulge direction)
	if (spEntity->isA() == Mx2dCustomArcPolyArea::desc())
	{
		Mx2dCustomArcPolyArea* pArcPolyArea = Mx2dCustomArcPolyArea::cast(spEntity.object());
		polylineVertices = pArcPolyArea->points();
	}
	// Handle Regular Polyline Area entity
	else if (spEntity->isA() == Mx2dCustomPolyArea::desc())
	{
		Mx2dCustomPolyArea* pPolyArea = Mx2dCustomPolyArea::cast(spEntity.object());
		McGePoint3dArray areaPoints = pPolyArea->points();

		// Convert point array to polyline vertex array (bulge = 0 for straight segments)
		for (int i = 0; i < areaPoints.length(); ++i)
		{
			polylineVertices.append({ areaPoints[i], 0.0 });
		}
	}
	// Handle Rectangular Area entity
	else if (spEntity->isA() == Mx2dCustomRectArea::desc())
	{
		Mx2dCustomRectArea* pRectArea = Mx2dCustomRectArea::cast(spEntity.object());
		McGePoint3dArray rectPoints = pRectArea->getRectPoints();

		// Convert rectangle points to polyline vertex array (bulge = 0)
		for (int i = 0; i < rectPoints.length(); ++i)
		{
			polylineVertices.append({ rectPoints[i], 0.0 });
		}
	}
	// Unsupported entity type
	else
	{
		return;
	}

	// Transfer area annotation data to main thread
	MXAPP.CallMain([=]() {
		emit Mx2dSignalTransfer::getInstance().signalAreaMarkInformation(MxUtils::gCurrentTab, polylineVertices);
		});
}


void MxCADCommand::Mx_DrawHatchArea2Mark()
{
	// Prompt to select a hatch entity
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select a hatch entity"));
	MrxDbgRbList entityFilter = Mx::mcutBuildList(RTDXF0, "HATCH", 0);
	McDbObjectId targetEntId;
	McGePoint3d pickPoint;

	if (MrxDbgUtils::selectEnt(QCoreApplication::translate("MxCADCommand", "Select a hatch entity").toStdString().c_str(), targetEntId, pickPoint, entityFilter.data()) != RTNORM)
	{
		HIDE_PROMPT_MESSAGE;
		return;
	}
	HIDE_PROMPT_MESSAGE;

	// Open selected hatch entity for read
	McDbEntityPointer spEntity(targetEntId, McDb::kForRead);
	if (spEntity->isA() == McDbHatch::desc())
	{
		Mx2d::HatchPLList hatchPolyArray;
		McDbHatch* pHatch = McDbHatch::cast(spEntity.object());

		// Process each hatch loop (convert to polyline with length/area/type info)
		for (int i = 0; i < pHatch->numLoops(); ++i)
		{
			Mdesk::Int32 loopType = -1;
			McGePoint2dArray loopVertices;
			McGeDoubleArray loopBulges;

			// Get loop data in polyline format (vertices + bulges)
			pHatch->getLoopAt(i, loopType, loopVertices, loopBulges);

			if (!loopVertices.isEmpty())
			{
				Mx2d::PLVertexList polylineVertices;
				McDbPolyline* pTempPolyline = new McDbPolyline();
				pTempPolyline->setClosed(true);

				// Build temporary polyline to calculate length and area
				for (int j = 0; j < loopVertices.length(); ++j)
				{
					pTempPolyline->addVertexAt(loopVertices[j], loopBulges[j]);
					polylineVertices.append({ loopVertices[j], loopBulges[j] });
				}

				// Calculate polyline length and area
				double polylineLength = Mx2d::getPolylineLength(pTempPolyline);

				double polylineArea = 0.0;
				pTempPolyline->getArea(polylineArea);
				delete pTempPolyline; // Clean up temporary polyline

				// Determine if the loop is an external boundary
				bool isExternalLoop = (loopType & McDbHatch::kExternal) != 0;

				// Package polyline data with additional info
				Mx2d::PLVertexListWithMetricsAndExtFlag polylineWithInfo = {
					polylineVertices, polylineLength, polylineArea, isExternalLoop
				};
				hatchPolyArray.append(polylineWithInfo);
			}
		}

		double viewToDocScale = Mx2d::getViewToDocScaleRatio();
		double textHeight = 25 / viewToDocScale;
		// Prompt to select text insertion point
		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select text insertion point"));
		MrxDbgUiPrPoint getTextInsertPoint(QCoreApplication::translate("MxCADCommand", "Select text insertion point").toStdString().c_str());

		// Dynamic preview of hatch area annotation
		getTextInsertPoint.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
			worldDrawHatchArea2(hatchPolyArray, textHeight, pData, pWorldDraw);
			});

		MrxDbgUiPrBase::Status retStatus = getTextInsertPoint.go();
		if (retStatus != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}

		McGePoint3d textInsertPoint = getTextInsertPoint.value();
		HIDE_PROMPT_MESSAGE;

		// Create and add hatch area annotation in main thread
		MXAPP.CallMain([=]() {
			auto* pAnno = new Mx2dCustomHatchArea2(hatchPolyArray, textInsertPoint, textHeight);
			McDbObjectId id = Mx2d::addEntityToAnnotationLayerAndClose(pAnno);
			emit Mx2dSignalTransfer::getInstance().signalAddAnnotation(MxUtils::gCurrentTab, id);
			});
	}
}

void MxCADCommand::Mx_SelectTextToModify()
{
	// Prompt to select text to modify
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select text to modify"));
	// Filter to select only single-line text (TextMark) and multi-line text (MTextMark) entities
	MrxDbgRbList entityFilter = Mx::mcutBuildList(RTDXF0, "TextMark,MTextMark", 0);
	McDbObjectId targetEntId;
	McGePoint3d pickPoint;

	if (MrxDbgUtils::selectEnt(QCoreApplication::translate("MxCADCommand", "Select text to modify").toStdString().c_str(), targetEntId, pickPoint, entityFilter.data()) != RTNORM)
	{
		HIDE_PROMPT_MESSAGE;
		return;
	}
	HIDE_PROMPT_MESSAGE;

	// Open selected text entity for read
	McDbEntityPointer spEntity(targetEntId, McDb::kForRead);
	QString currentTextContent;
	bool isSingleLineText = true;

	// Handle single-line text entity (TextMark)
	if (spEntity->isA() == Mx2dCustomText::desc())
	{
		Mx2dCustomText* pSingleLineText = Mx2dCustomText::cast(spEntity.object());
		currentTextContent = pSingleLineText->textString();
	}
	// Handle multi-line text entity (MTextMark)
	else if (spEntity->isA() == Mx2dCustomMText::desc())
	{
		Mx2dCustomMText* pMultiLineText = Mx2dCustomMText::cast(spEntity.object());
		currentTextContent = pMultiLineText->contents();
		isSingleLineText = false;
	}

	// Trigger text modification dialog via signal in main thread
	MXAPP.CallMain([=]() {
		if (isSingleLineText)
		{
			// Signal for single-line text modification
			emit Mx2dSignalTransfer::getInstance().signalTextModify(MxUtils::gCurrentTab, currentTextContent, targetEntId);
		}
		else
		{
			// Signal for multi-line text modification
			emit Mx2dSignalTransfer::getInstance().signalMTextModify(MxUtils::gCurrentTab, currentTextContent, targetEntId);
		}
		});
}

void MxCADCommand::Mx_ModifyText()
{
	struct resbuf* pRb = Mx::mcedGetArgs();
	if (!pRb || pRb->restype != RTSTR ||
		!pRb->rbnext || pRb->rbnext->restype != RTId)
	{
		return;
	}

	QString newStr = QString::fromLocal8Bit(pRb->resval.rstring);
	McDbObjectId id;
	id.setFromOldId(pRb->rbnext->resval.objId);
	MXAPP.CallMain([=]() {
		McDbEntityPointer spEntity(id, McDb::kForWrite);
		if (spEntity->isA() == Mx2dCustomText::desc())
		{
			Mx2dCustomText* pTextMark = Mx2dCustomText::cast(spEntity.object());
			pTextMark->setTextString(newStr);
		}
		else if (spEntity->isA() == Mx2dCustomMText::desc())
		{
			Mx2dCustomMText* pMTextMark = Mx2dCustomMText::cast(spEntity.object());
			pMTextMark->setContents(newStr);
		}
		});
}

void MxCADCommand::Mx_MoveText()
{
	// Prompt to select text to move
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select text to move"));
	// Filter to select only single-line text (TextMark) and multi-line text (MTextMark) entities
	MrxDbgRbList entityFilter = Mx::mcutBuildList(RTDXF0, "TextMark,MTextMark", 0);
	McDbObjectId targetEntId;
	McGePoint3d pickPoint;

	if (MrxDbgUtils::selectEnt(QCoreApplication::translate("MxCADCommand", "Select text to move").toStdString().c_str(), targetEntId, pickPoint, entityFilter.data()) != RTNORM)
	{
		HIDE_PROMPT_MESSAGE;
		return;
	}
	HIDE_PROMPT_MESSAGE;

	// Open selected text entity for write (to modify position and visibility)
	McDbEntityPointer spEntity(targetEntId, McDb::kForWrite);
	if (spEntity.openStatus() != Mcad::eOk)
	{
		return;
	}

	// Hide original text during movement preview
	spEntity->setVisibility(McDb::kInvisible);

	// Prompt to select new text placement point
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select new placement point"));
	MrxDbgUiPrPoint getNewPlacementPoint(QCoreApplication::translate("MxCADCommand", "Select new placement point").toStdString().c_str());

	// Dynamic preview: Draw cloned text at current cursor position
	getNewPlacementPoint.setUserDraw([&](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
		if (!pData->isValidCurPoint) return;

		McGePoint3d currentCursorPoint = pData->curPoint;
		// Clone original text entity for preview
		McDbEntity* pClonedEntity = static_cast<McDbEntity*>(spEntity->clone());
		std::unique_ptr<McDbEntity> upClonedEntity(pClonedEntity); // Auto-cleanup

		// Translate cloned text from original pick point to current cursor position
		McGeVector3d translationVec = currentCursorPoint - pickPoint;
		upClonedEntity->transformBy(McGeMatrix3d::translation(translationVec));

		// Draw preview of moved text
		upClonedEntity->worldDraw(pWorldDraw);
		});

	MrxDbgUiPrBase::Status retStatus = getNewPlacementPoint.go();
	if (retStatus != MrxDbgUiPrBase::kOk)
	{
		HIDE_PROMPT_MESSAGE;
		// Restore original text visibility if operation is canceled
		spEntity->setVisibility(McDb::kVisible);
		return;
	}
	HIDE_PROMPT_MESSAGE;
	spEntity->close();
	// Get final new placement point
	McGePoint3d newPlacementPoint = getNewPlacementPoint.value();


	MXAPP.CallMain([=]() {
		 emit Mx2dSignalTransfer::getInstance().signalMoveAnnotation(MxUtils::gCurrentTab, targetEntId, newPlacementPoint - pickPoint);
		});
}

void MxCADCommand::Mx_CopyText()
{
	// Prompt to select text to copy
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select text to copy"));
	// Filter to select only single-line text (TextMark) and multi-line text (MTextMark) entities
	MrxDbgRbList entityFilter = Mx::mcutBuildList(RTDXF0, "TextMark,MTextMark", 0);
	McDbObjectId targetEntId;
	McGePoint3d pickPoint;

	if (MrxDbgUtils::selectEnt(QCoreApplication::translate("MxCADCommand", "Select text to copy").toStdString().c_str(), targetEntId, pickPoint, entityFilter.data()) != RTNORM)
	{
		HIDE_PROMPT_MESSAGE;
		return;
	}
	HIDE_PROMPT_MESSAGE;

	// Open selected text entity for write (to clone)
	McDbEntityPointer spEntity(targetEntId, McDb::kForWrite);
	if (spEntity.openStatus() != Mcad::eOk)
	{
		return;
	}

	// Support multiple copies (loop until user cancels)
	while (true)
	{
		// Prompt to select copy placement point
		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select copy placement point"));
		MrxDbgUiPrPoint getCopyPlacementPoint(QCoreApplication::translate("MxCADCommand", "Select copy placement point").toStdString().c_str());

		// Dynamic preview: Draw cloned text at current cursor position
		getCopyPlacementPoint.setUserDraw([&](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
			if (!pData->isValidCurPoint) return;

			McGePoint3d currentCursorPoint = pData->curPoint;
			// Clone original text entity for preview
			McDbEntity* pClonedEntity = static_cast<McDbEntity*>(spEntity->clone());
			std::unique_ptr<McDbEntity> upClonedEntity(pClonedEntity); // Auto-cleanup

			// Translate cloned text from original pick point to current cursor position
			McGeVector3d translationVec = currentCursorPoint - pickPoint;
			upClonedEntity->transformBy(McGeMatrix3d::translation(translationVec));

			// Draw preview of copied text
			upClonedEntity->worldDraw(pWorldDraw);
			});

		MrxDbgUiPrBase::Status retStatus = getCopyPlacementPoint.go();
		if (retStatus != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			return; // Exit copy loop if user cancels
		}
		HIDE_PROMPT_MESSAGE;

		// Get final copy placement point
		McGePoint3d copyPlacementPoint = getCopyPlacementPoint.value();

		// Clone original text and place at target position
		McDbEntity* pFinalClonedEntity = static_cast<McDbEntity*>(spEntity->clone());
		McGeVector3d finalTranslationVec = copyPlacementPoint - pickPoint;
		pFinalClonedEntity->transformBy(McGeMatrix3d::translation(finalTranslationVec));

		MXAPP.CallMain([=]() {
			McDbObjectId id = Mx2d::addEntityToAnnotationLayerAndClose(pFinalClonedEntity);
			emit Mx2dSignalTransfer::getInstance().signalAddAnnotation(MxUtils::gCurrentTab, id);
			});

	}
}

void MxCADCommand::Mx_SelectAreaToOffset()
{
	// Prompt to select area annotation object
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select area annotation object"));
	// Filter to select supported area annotation types: PolyAreaMark, ArcPolyAreaMark, RectAreaMark
	MrxDbgRbList entityFilter = Mx::mcutBuildList(RTDXF0, "PolyAreaMark,ArcPolyAreaMark,RectAreaMark", 0);
	McDbObjectId targetEntId;
	McGePoint3d pickPoint;

	if (MrxDbgUtils::selectEnt(QCoreApplication::translate("MxCADCommand", "Select area annotation object").toStdString().c_str(), targetEntId, pickPoint, entityFilter.data()) != RTNORM)
	{
		HIDE_PROMPT_MESSAGE;
		return;
	}
	HIDE_PROMPT_MESSAGE;

	// Open selected area annotation entity for write
	McDbEntityPointer spEntity(targetEntId, McDb::kForWrite);
	if (spEntity.openStatus() != Mcad::eOk)
	{
		return;
	}

	// Send area annotation ID to main thread via signal
	MXAPP.CallMain([=]() {
		emit Mx2dSignalTransfer::getInstance().signalAreaMarkId(MxUtils::gCurrentTab, targetEntId);
		});
}

void MxCADCommand::Mx_DoOffsetArea()
{
	struct resbuf* pRb = Mx::mcedGetArgs();
	if (!pRb || pRb->restype != RTREAL ||
		!pRb->rbnext || pRb->rbnext->restype != RTId ||
		!pRb->rbnext->rbnext || pRb->rbnext->rbnext->restype != RTSHORT)
	{
		return;
	}

	double offset = pRb->resval.rreal;
	McDbObjectId id;
	id.setFromOldId(pRb->rbnext->resval.objId);
	short mode = pRb->rbnext->rbnext->resval.rint;


	McDbEntityPointer spEntity(id, McDb::kForRead);
	if (spEntity->isA() == Mx2dCustomPolyArea::desc())
	{
		Mx2dCustomPolyArea* pPolyArea = Mx2dCustomPolyArea::cast(spEntity.object());
		McGePoint3dArray pts = pPolyArea->points();
		McDbPolyline* pPoly = new McDbPolyline();
		pPoly->setClosed(true);
		for (int i = 0; i < pts.length(); ++i) {
			pPoly->addVertexAt(pts[i]);
		}

		McGePoint3d innerPt, outerPt;
		bool success = Mx2d::getPolygonOffsetPoints(pPoly, innerPt, outerPt);


		if (success)
		{
			McDbVoidPtrArray offsetCurves;
			Mcad::ErrorStatus es = pPoly->getOffsetCurvesEx(offset, offsetCurves, mode == 1 ? outerPt : innerPt);
			if (es == Mcad::eOk)
			{
				McDbPolyline* pOffsetCurveMax = static_cast<McDbPolyline*>(offsetCurves[0]);
				double areaMax = 0.0; pOffsetCurveMax->getArea(areaMax);
				for (int i = 1; i < offsetCurves.length(); i++)
				{
					McDbPolyline* pOffsetCurve = static_cast<McDbPolyline*>(offsetCurves[i]);
					double area = 0.0; pOffsetCurve->getArea(area);
					if (area > areaMax)
					{
						pOffsetCurveMax = pOffsetCurve;
						areaMax = area;
					}
				}
				if (pOffsetCurveMax)
				{
					McGePoint3dArray newPts;
					for (unsigned int i = 0; i < pOffsetCurveMax->numVerts() - 1; ++i) {
						McGePoint3d pt; pOffsetCurveMax->getPointAt(i, pt);
						newPts.append(pt);
					}
					//pPolyArea->setPoints(newPts);
					pPolyArea->close();
					MXAPP.CallMain([=]() {
						emit Mx2dSignalTransfer::getInstance().signalModifyPolyArea(MxUtils::gCurrentTab, id, pts, newPts);
						});
				}
				for (int i = 0; i < offsetCurves.length(); i++)
				{
					McDbPolyline* pOffsetCurve = static_cast<McDbPolyline*>(offsetCurves[i]);
					delete pOffsetCurve;
				}
				offsetCurves.clear();
			}
		}
		delete pPoly;
	}
	else if (spEntity->isA() == Mx2dCustomArcPolyArea::desc())
	{
		Mx2dCustomArcPolyArea* pArcPolyArea = Mx2dCustomArcPolyArea::cast(spEntity.object());
		Mx2d::PLVertexList pts = pArcPolyArea->points();
		McDbPolyline* pPoly = new McDbPolyline();
		pPoly->setClosed(true);
		for (int i = 0; i < pts.length(); ++i) {
			McGePoint3d pt = pts[i].pt;
			double bulge = pts[i].bulge;
			pPoly->addVertexAt(pt, bulge);
		}

		McGePoint3d innerPt, outerPt;
		bool success = Mx2d::getPolygonOffsetPoints(pPoly, innerPt, outerPt);

		if (success)
		{
			McDbVoidPtrArray offsetCurves;
			Mcad::ErrorStatus es = pPoly->getOffsetCurvesEx(offset, offsetCurves, mode == 1 ? outerPt : innerPt);
			if (es == Mcad::eOk)
			{
				McDbPolyline* pOffsetCurveMax = static_cast<McDbPolyline*>(offsetCurves[0]);
				double areaMax = 0.0; pOffsetCurveMax->getArea(areaMax);
				for (int i = 1; i < offsetCurves.length(); i++)
				{
					McDbPolyline* pOffsetCurve = static_cast<McDbPolyline*>(offsetCurves[i]);
					double area = 0.0; pOffsetCurve->getArea(area);
					if (area > areaMax)
					{
						pOffsetCurveMax = pOffsetCurve;
						areaMax = area;
					}
				}
				if (pOffsetCurveMax)
				{
					Mx2d::PLVertexList newPts;
					for (unsigned int i = 0; i < pOffsetCurveMax->numVerts() - 1; ++i) {
						McGePoint3d pt; pOffsetCurveMax->getPointAt(i, pt);
						double bulge; pOffsetCurveMax->getBulgeAt(i, bulge);
						newPts.append({ pt, bulge });
					}
					//pArcPolyArea->setPoints(newPts);
                    pArcPolyArea->close();
                    MXAPP.CallMain([=]() {
						emit Mx2dSignalTransfer::getInstance().signalModifyArcPolyArea(MxUtils::gCurrentTab, id, pts, newPts);
						});
				}
				for (int i = 0; i < offsetCurves.length(); i++)
				{
					McDbPolyline* pOffsetCurve = static_cast<McDbPolyline*>(offsetCurves[i]);
					delete pOffsetCurve;
				}
				offsetCurves.clear();
			}
		}
		delete pPoly;
	}
	else if (spEntity->isA() == Mx2dCustomRectArea::desc())
	{ 
        Mx2dCustomRectArea* pRectArea = Mx2dCustomRectArea::cast(spEntity.object());
		McGePoint3d corner1 = pRectArea->corner1();
		McGePoint3d corner2 = pRectArea->corner2();
		McGeVector3d dir = corner2 - corner1;
		McGePoint3d newCorner1, newCorner2;
		if (mode == 1) // offset to outer
		{
			if (dir.x > 0.0 && dir.y > 0.0)
			{
				newCorner1 = McGePoint3d(corner1.x - offset, corner1.y - offset, 0.0);
                newCorner2 = McGePoint3d(corner2.x + offset, corner2.y + offset, 0.0);
			}
			else if (dir.x < 0.0 && dir.y > 0.0)
			{
                newCorner1 = McGePoint3d(corner1.x + offset, corner1.y - offset, 0.0);
                newCorner2 = McGePoint3d(corner2.x - offset, corner2.y + offset, 0.0);
			}
			else if (dir.x < 0.0 && dir.y < 0.0)
			{
                newCorner1 = McGePoint3d(corner1.x + offset, corner1.y + offset, 0.0);
                newCorner2 = McGePoint3d(corner2.x - offset, corner2.y - offset, 0.0);
			}
			else if (dir.x > 0.0 && dir.y < 0.0)
			{
                newCorner1 = McGePoint3d(corner1.x - offset, corner1.y + offset, 0.0);
                newCorner2 = McGePoint3d(corner2.x + offset, corner2.y - offset, 0.0);
			}
		}
		else
		{
			if (dir.x > 0.0 && dir.y > 0.0)
			{
				newCorner1 = McGePoint3d(corner1.x + offset, corner1.y + offset, 0.0);
				newCorner2 = McGePoint3d(corner2.x - offset, corner2.y - offset, 0.0);
			}
			else if (dir.x < 0.0 && dir.y > 0.0)
			{
				newCorner1 = McGePoint3d(corner1.x - offset, corner1.y + offset, 0.0);
				newCorner2 = McGePoint3d(corner2.x + offset, corner2.y - offset, 0.0);
			}
			else if (dir.x < 0.0 && dir.y < 0.0)
			{
				newCorner1 = McGePoint3d(corner1.x - offset, corner1.y - offset, 0.0);
				newCorner2 = McGePoint3d(corner2.x + offset, corner2.y + offset, 0.0);
			}
			else if (dir.x > 0.0 && dir.y < 0.0)
			{
				newCorner1 = McGePoint3d(corner1.x + offset, corner1.y - offset, 0.0);
				newCorner2 = McGePoint3d(corner2.x - offset, corner2.y + offset, 0.0);
			}
		}

		MXAPP.CallMain([=]() {
			emit Mx2dSignalTransfer::getInstance().signalModifyRectArea(MxUtils::gCurrentTab, id, corner1, corner2, newCorner1, newCorner2);
			});

	}


	
}

void MxCADCommand::Mx_BatchMeasure()
{
	Mx2d::CurveShapeList shapes;

	while (true)
	{
		McDbObjectId id;
		McGePoint3d ptPick;
		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select a curve"));
		int ret = MrxDbgUtils::selectEnt(QCoreApplication::translate("MxCADCommand", "Select a curve").toStdString().c_str(), id, ptPick);
		if (ret != RTNORM)
		{
			HIDE_PROMPT_MESSAGE;
			break;
		}

		McDbEntityPointer spEntity(id, McDb::kForRead);
		if (spEntity.openStatus() != Mcad::eOk)
			continue;

		if (spEntity->isA() == McDbLine::desc())
		{
			McDbLine* pLine = McDbLine::cast(spEntity.object());
			auto spLineShape = std::make_shared<Mx2d::LineShape>(pLine->startPoint(), pLine->endPoint());
			shapes.append(spLineShape);
		}
		else if (spEntity->isA() == McDbArc::desc())
		{
			McDbArc* pArc = McDbArc::cast(spEntity.object());
			McGePoint3d startPt, endPt, midPt;
			pArc->getStartPoint(startPt);
			pArc->getEndPoint(endPt);
			double startParam = 0.0, endParam = 0.0, midParam = 0.0;
			pArc->getStartParam(startParam);
			pArc->getEndParam(endParam);
			midParam = (startParam + endParam) / 2.0;
			pArc->getPointAtParam(midParam, midPt);
			auto spArcShape = std::make_shared<Mx2d::ArcShape>(startPt, midPt, endPt);
			shapes.append(spArcShape);
		}
		else if (spEntity->isA() == McDbCircle::desc())
		{
			McDbCircle* pCircle = McDbCircle::cast(spEntity.object());
			auto spCircleShape = std::make_shared<Mx2d::CircleShape>(pCircle->center(), pCircle->radius());
			shapes.append(spCircleShape);
		}
		else if (spEntity->isA() == McDbEllipse::desc())
		{
			McDbEllipse* pEllipse = McDbEllipse::cast(spEntity.object());
			auto spEllipseShape = std::make_shared<Mx2d::EllipseShape>(pEllipse->center(), pEllipse->majorAxis(), pEllipse->radiusRatio(), pEllipse->startAngle(), pEllipse->endAngle());
			shapes.append(spEllipseShape);
		}
		else if (spEntity->isA() == McDbPolyline::desc())
		{
			McDbPolyline* pPoly = McDbPolyline::cast(spEntity.object());
			int indexArc = Mx2d::indexArcBefore(ptPick, pPoly);
			int indexLine = Mx2d::indexLineBefore(ptPick, pPoly);
			if (indexArc != -1)
			{
				McGePoint3d startPt, endPt;
				pPoly->getPointAt(indexArc, startPt);

				if (indexArc == pPoly->numVerts() - 1 && pPoly->isClosed())
				{
					pPoly->getStartPoint(endPt);
				}
				else
				{
					pPoly->getPointAt(indexArc + 1, endPt);
				}

				double bulge = 0.0;
				pPoly->getBulgeAt(indexArc, bulge);

				McGePoint3d center;
				double radius = 0.0;
				double angle = 0.0;
				Mx2d::calcArc(startPt, endPt, bulge, center, radius, angle);
				double arcMidRot = angle / 2.0;
				McGeVector3d vecArcMid(startPt - center);
				if (bulge < 0.0) // clockwise
				{
					arcMidRot = -arcMidRot;
				}
				vecArcMid.rotateBy(arcMidRot, McGeVector3d::kZAxis);
				McGePoint3d midPt = center + vecArcMid;
				auto spArcShape = std::make_shared<Mx2d::ArcShape>(startPt, midPt, endPt);
				shapes.append(spArcShape);
			}
			else if (indexLine != -1)
			{
				McGePoint3d startPt, endPt;
				pPoly->getPointAt(indexLine, startPt);
				if (indexLine == pPoly->numVerts() - 1 && pPoly->isClosed())
				{
					pPoly->getStartPoint(endPt);
				}
				else
				{
					pPoly->getPointAt(indexLine + 1, endPt);
				}
				auto spLineShape = std::make_shared<Mx2d::LineShape>(startPt, endPt);
				shapes.append(spLineShape);
			}

		}
		else
		{
			continue;
		}
	}
	if (shapes.length() == 0)
	{
		return;
	}

	double scaled = Mx2d::getViewToDocScaleRatio();
	double textHeight = 25 / scaled;

	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select text placement point"));
	MrxDbgUiPrPoint getDimPoint(QCoreApplication::translate("MxCADCommand", "Select text placement point").toStdString().c_str());
	getDimPoint.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
		worldDrawBatchMeasure(shapes, textHeight, pData, pWorldDraw);
		});
	MrxDbgUiPrBase::Status ret = getDimPoint.go();
	if (ret != MrxDbgUiPrBase::kOk)
	{
		HIDE_PROMPT_MESSAGE;
		return;
	}
	HIDE_PROMPT_MESSAGE;
	McGePoint3d dimPt = getDimPoint.value();


	MXAPP.CallMain([=]() {
		auto* pAnno = new Mx2dCustomBatchMeasurement(shapes, dimPt, textHeight);
		McDbObjectId id = Mx2d::addEntityToAnnotationLayerAndClose(pAnno);
		emit Mx2dSignalTransfer::getInstance().signalAddAnnotation(MxUtils::gCurrentTab, id);
		});
}

void MxCADCommand::Mx_MoveAnnotation()
{
	auto setEntitiesVisibility = [](McDbObjectIdArray& ids, bool visible) {
		for (int i = 0; i < ids.length(); i++) {
			McDbEntityPointer spEntity(ids[i], McDb::kForWrite);
			if (spEntity.openStatus() != Mcad::eOk)
			{
				continue;
			}
			spEntity->setVisibility(visible ? McDb::kVisible : McDb::kInvisible);
		}
		};

	McDbObjectIdArray ids;

	while (true)
	{
		// Prompt to select annotation objects
		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select annotation objects"));
		McDbObjectId entId;
		McGePoint3d ptPick;
		if (MrxDbgUtils::selectEnt(QCoreApplication::translate("MxCADCommand", "Select annotation objects").toStdString().c_str(), entId, ptPick) != RTNORM)
		{
			HIDE_PROMPT_MESSAGE;
			break;
		}
		HIDE_PROMPT_MESSAGE;
		McDbEntityPointer spEntity(entId, McDb::kForRead);
		if (spEntity.openStatus() != Mcad::eOk)
		{
			continue;
		}
		if (!spEntity->isKindOf(Mx2dCustomAnnotation::desc()))
		{
			continue;
		}
		ids.append(entId);
	}

	if (ids.length() == 0)
	{
		return;
	}

	// Prompt to select base point
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select base point"));
	MrxDbgUiPrPoint getBasePoint(QCoreApplication::translate("MxCADCommand", "Select base point").toStdString().c_str());
	MrxDbgUiPrBase::Status ret = getBasePoint.go();
	HIDE_PROMPT_MESSAGE;
	if (ret != MrxDbgUiPrBase::kOk) {
		return;
	}
	McGePoint3d basePt = getBasePoint.value();
	// Hide annotation objects
	setEntitiesVisibility(ids, false);

	// Prompt to select placement point
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select placement point"));
	MrxDbgUiPrPoint getPoint(QCoreApplication::translate("MxCADCommand", "Select placement point").toStdString().c_str());
	getPoint.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
		worldDrawLine(basePt, pData, pWorldDraw);
		});
	// Dynamic preview: Draw cloned annotations at current cursor position
	getPoint.setUserDraw([&](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
		if (!pData->isValidCurPoint) return;
		McGePoint3d curPoint = pData->curPoint;

		for (int i = 0; i < ids.length(); i++) {
			McDbEntityPointer spEntity(ids[i], McDb::kForRead);
			if (spEntity.openStatus() != Mcad::eOk)
			{
				continue;
			}
			// Clone original annotation for preview
			McDbEntity* pEntityCopy = static_cast<McDbEntity*>(spEntity->clone());
			std::unique_ptr<McDbEntity> uspEntity(pEntityCopy); // Auto-cleanup
			// Translate cloned annotation from base point to current cursor position
			uspEntity->transformBy(McGeMatrix3d::translation(curPoint - basePt));
			// Draw preview of moved annotations
			uspEntity->worldDraw(pWorldDraw);
		}
		});
	ret = getPoint.go();
	HIDE_PROMPT_MESSAGE;
	if (ret != MrxDbgUiPrBase::kOk) {
		// Restore annotation visibility if operation is canceled
		setEntitiesVisibility(ids, true);
		return;
	}
	McGePoint3d pt = getPoint.value();


    MXAPP.CallMain([=]() {
		 emit Mx2dSignalTransfer::getInstance().signalMoveAnnotations(MxUtils::gCurrentTab, ids, pt - basePt);
		});
}

void MxCADCommand::Mx_CopyAnnotation()
{
	McDbObjectIdArray ids;

	while (true)
	{
		// Prompt to select annotation objects
		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select annotation objects"));
		McDbObjectId entId;
		McGePoint3d ptPick;
		if (MrxDbgUtils::selectEnt(QCoreApplication::translate("MxCADCommand", "Select annotation objects").toStdString().c_str(), entId, ptPick) != RTNORM)
		{
			HIDE_PROMPT_MESSAGE;
			break;
		}
		HIDE_PROMPT_MESSAGE;
		McDbEntityPointer spEntity(entId, McDb::kForRead);
		if (spEntity.openStatus() != Mcad::eOk)
		{
			continue;
		}
		if (!spEntity->isKindOf(Mx2dCustomAnnotation::desc()))
		{
			continue;
		}
		ids.append(entId);
	}

	if (ids.length() == 0)
	{
		return;
	}

	// Prompt to select base point
	SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select base point"));
	MrxDbgUiPrPoint getBasePoint(QCoreApplication::translate("MxCADCommand", "Select base point").toStdString().c_str());
	MrxDbgUiPrBase::Status ret = getBasePoint.go();
	HIDE_PROMPT_MESSAGE;
	if (ret != MrxDbgUiPrBase::kOk) {
		return;
	}
	McGePoint3d basePt = getBasePoint.value();

	while (true) {
		// Prompt to select placement point
		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select placement point"));
		MrxDbgUiPrPoint getPoint(QCoreApplication::translate("MxCADCommand", "Select placement point").toStdString().c_str());
		getPoint.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
			worldDrawLine(basePt, pData, pWorldDraw);
			});
		getPoint.setUserDraw([&](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
			if (!pData->isValidCurPoint) return;
			McGePoint3d curPoint = pData->curPoint;
			for (int i = 0; i < ids.length(); i++) {
				McDbEntityPointer spEntity(ids[i], McDb::kForRead);
				if (spEntity.openStatus() != Mcad::eOk)
				{
					continue;
				}
				McDbEntity* pEntityCopy = static_cast<McDbEntity*>(spEntity->clone());
				std::unique_ptr<McDbEntity> uspEntity(pEntityCopy);
				uspEntity->transformBy(McGeMatrix3d::translation(curPoint - basePt));
				uspEntity->worldDraw(pWorldDraw);
			}
			});
		MrxDbgUiPrBase::Status ret = getPoint.go();
		HIDE_PROMPT_MESSAGE;
		if (ret != MrxDbgUiPrBase::kOk) {
			break;
		}
		McGePoint3d pt = getPoint.value();
		McDbObjectIdArray newIds;
		for (int i = 0; i < ids.length(); i++) {
			McDbEntityPointer spEntity(ids[i], McDb::kForRead);
			McDbEntity* pEntityCopy = static_cast<McDbEntity*>(spEntity->clone());
			pEntityCopy->transformBy(McGeMatrix3d::translation(pt - basePt));
			//MrxDbgUtils::addToCurrentSpaceAndClose(pEntityCopy);
			McDbObjectId id = Mx2d::addEntityToAnnotationLayerAndClose(pEntityCopy);
            newIds.append(id);
			
		}
		MXAPP.CallMain([=]() {
			emit Mx2dSignalTransfer::getInstance().signalAddAnnotations(MxUtils::gCurrentTab, newIds);
			});
	}
}

void MxCADCommand::Mx_EraseAnnotation()
{
	McDbObjectIdArray ids;
	while (true)
	{
		// Prompt to select annotation objects
		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select annotation objects"));
		McDbObjectId entId;
		McGePoint3d ptPick;
		if (MrxDbgUtils::selectEnt(QCoreApplication::translate("MxCADCommand", "Select annotation objects").toStdString().c_str(), entId, ptPick) != RTNORM)
		{
			HIDE_PROMPT_MESSAGE;
			break;
		}
		HIDE_PROMPT_MESSAGE;
		McDbEntityPointer spEntity(entId, McDb::kForRead);
		if (spEntity.openStatus() != Mcad::eOk)
		{
			continue;
		}
		if (!spEntity->isKindOf(Mx2dCustomAnnotation::desc()))
		{
			continue;
		}
		ids.append(entId);
		
	}
	MXAPP.CallMain([=]() {
		 emit Mx2dSignalTransfer::getInstance().signalEeaseAnnotations(MxUtils::gCurrentTab, ids);
		});
}

void MxCADCommand::Mx_GetDrawingLength()
{
	// prompt to select start point
    SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select start point"));
    MrxDbgUiPrPoint getStartPoint(QCoreApplication::translate("MxCADCommand", "Select start point").toStdString().c_str());
    MrxDbgUiPrBase::Status retStatus = getStartPoint.go();
    HIDE_PROMPT_MESSAGE;
    if (retStatus != MrxDbgUiPrBase::kOk)
    {
        return;
    }
    McGePoint3d startPoint = getStartPoint.value();
    SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select end point"));
    MrxDbgUiPrPoint getEndPoint(QCoreApplication::translate("MxCADCommand", "Select end point").toStdString().c_str());
    retStatus = getEndPoint.go();
    HIDE_PROMPT_MESSAGE;
    if (retStatus != MrxDbgUiPrBase::kOk)
    {
        return;
    }
    McGePoint3d endPoint = getEndPoint.value();
	double length = startPoint.distanceTo(endPoint);
    MXAPP.CallMain([=]() {
		 emit Mx2dSignalTransfer::getInstance().signalDrawingLength(MxUtils::gCurrentTab, length);
		});

}

void MxCADCommand::Mx_TestThread()
{
	// Continuous line drawing loop (repeat until user cancels)
	while (true)
	{
		// Prompt user to select line start point
		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select start point"));
		MrxDbgUiPrPoint getStartPoint(QCoreApplication::translate("MxCADCommand", "Select start point").toStdString().c_str());
		MrxDbgUiPrBase::Status retStatus = getStartPoint.go();

		// Exit command if user cancels or fails to select start point
		if (retStatus != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}
		McGePoint3d startPoint = getStartPoint.value();

		// Prompt user to select line end point
		SHOW_PROMPT_MESSAGE(QCoreApplication::translate("MxCADCommand", "Select end point"));
		MrxDbgUiPrPoint getEndPoint(QCoreApplication::translate("MxCADCommand", "Select end point").toStdString().c_str());
		// Set base point to start point for dynamic rubber band preview
		getEndPoint.setUserDraw([=](McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw) {
			worldDrawLine(startPoint, pData, pWorldDraw);
			});
		retStatus = getEndPoint.go();

		// Exit command if user cancels or fails to select end point
		if (retStatus != MrxDbgUiPrBase::kOk)
		{
			HIDE_PROMPT_MESSAGE;
			return;
		}
		McGePoint3d endPoint = getEndPoint.value();

		// Hide command prompt after selecting both points
		HIDE_PROMPT_MESSAGE;

		// Draw the line in main thread (ensure UI thread safety)
		MXAPP.CallMain([=]() {
			auto* pAnno = new Mx2dCustomLine(startPoint, endPoint, 1.0);
			McDbObjectId id = Mx2d::addEntityToAnnotationLayerAndClose(pAnno);
			emit Mx2dSignalTransfer::getInstance().signalAddAnnotation(MxUtils::gCurrentTab, id);
			});
	}
}

