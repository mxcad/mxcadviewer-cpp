/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include "MxCADInclude.h"
#include <QMap>
#include <QList>
#include <QString>
#include "Mx2dUtils.h"

class Mx2dCustomLinearDim;
class Mx2dCustomContinuousMeasurement;
class Mx2dCustomRadiusDim;

class MxCADCommand
{
public:
	static void Mx_TestThread();
	static void Mx_Test();

	// Full drawing search
	static void Mx_TextSearch();
	// Search within a rectangular area
	static void Mx_TextSearchRect();
	// Search within a polygonal area
	static void Mx_TextSearchPolygon();

	static void Mx_GetRectArea();
	static void Mx_GetPolyArea();

	// Draw a line
	static void Mx_DrawLine();

	// Move the camera center and zoom to a specified point
	static void Mx_MoveViewCenterTo();

	// Zoom to extents
	static void Mx_ZoomAll();

	// Zoom to a window
	static void Mx_ZoomWin();

	// Get all layers
	static void Mx_GetAllLayer();

	// Set the on/off status of some layers
	static void Mx_SetSomeLayersOffStatus();

	// Turn off selected layers
	static void Mx_SetSelectedLayersOff();

	// Isolate selected layers (turn off all others)
	static void Mx_SetNotSelectedLayersOff();

	// Extract text from a rectangular area
	static void Mx_ExtractTextRect();

	// Extract text from a polygonal area
	static void Mx_ExtractTextPoly();

	// Extract table
	static void Mx_ExtractTable();

	// Draw numbered text
	static void Mx_DrawNumberedText();

	// Draw single-line text
	static void Mx_DrawText();

	// Draw multi-line text
	static void Mx_DrawMText();

	// Draw a rectangular annotation
	static void Mx_DrawRectMark();

	// Draw an elliptical annotation
	static void Mx_DrawEllipseMark();

	// Draw a rectangular cloud annotation
	static void Mx_DrawRectCloudMark();

	// Draw a leader annotation
	static void Mx_DrawLeaderMark();

	// Draw a polygonal area annotation
	static void Mx_DrawPolyAreaMark();

	// Draw a polygonal area annotation (including arcs)
	static void Mx_DrawArcPolyAreaMark();

	// Draw a rectangular area annotation
	static void Mx_DrawRectAreaMark();

	// Draw a coordinate annotation
	static void Mx_DrawCartesianCoordMark();

	// Draw an aligned dimension annotation
	static void Mx_DrawAlignedDimMark();

	// Draw a linear dimension annotation
	static void Mx_DrawLinearDimMark();

	// Draw a continuous measurement annotation
	static void Mx_DrawContinuousMeasurementMark();

	// Show segment lengths
	static void Mx_ShowSegmentLengths();

	// Draw a radius dimension annotation
	static void Mx_DrawRadiusDimMark();

	// Draw an arc length dimension annotation
	static void Mx_DrawArcLengthDimMark();

	// Draw a circle measurement annotation
	static void Mx_DrawCircleMeasurementMark();

	// Update the display
	static void Mx_UpdateDisplay();

	// Draw a point-to-line distance annotation
	static void Mx_DrawDistPointToLineMark();

	// Draw an angle measurement annotation
	static void Mx_DrawAngleMeasurementMark();

	// Calculate side area
	static void Mx_CalculateSiderArea();

	// Draw a hatch area annotation
	//static void Mx_DrawHatchAreaMark();

	// Draw a hatch area annotation (alternative method)
	static void Mx_DrawHatchArea2Mark();

	// Select text to modify
	static void Mx_SelectTextToModify();

	// Modify text
	static void Mx_ModifyText();

	// Move text
	static void Mx_MoveText();

	// Copy text
	static void Mx_CopyText();


	// Select an area object to offset
	static void Mx_SelectAreaToOffset();

	// Offset an area object
	static void Mx_DoOffsetArea();

	// Batch measurement
	static void Mx_BatchMeasure();

	// Move annotation
	static void Mx_MoveAnnotation();

	// Copy annotation
	static void Mx_CopyAnnotation();

	// Erase an annotation object
	static void Mx_EraseAnnotation();

	static void Mx_GetDrawingLength();

	// Register CAD command functions
	static void RegisterCommand();

private:
	static void textFilter(Mx2d::TextInfoList& result, const McDbObjectIdArray& ids, const QString& containStr = QString(), bool isFull = false);
	static void worldDrawLine(const McGePoint3d& startPoint, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw);
	static void worldDrawRect(const McGePoint3d& corner1, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw);

	static void worldDrawPolygon(const McGePoint3dArray& pts, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw);
	static void worldDrawPolygon(const Mx2d::PLVertexList& pts, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw);

	static void worldDrawEllipse(const McGePoint3d& corner1, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw);
	static void worldDrawRectCloud(const McGePoint3d& corner1, double textHeight, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw);

	static void worldDrawText(const QString& str, double textHeight, McDbObjectId textStyle, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw);
	static void worldDrawMText(const QString& str, double textHeight, McDbObjectId textStyle, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw);

	static void worldDrawLeader(const McGePoint3d& startPt, const QString& text, double textHeight, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw);
	static void worldDrawPolyArea(const McGePoint3dArray& pts, double textHeight, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw);
	static void worldDrawRectArea(const McGePoint3d& corner1, const McGePoint3d& corner2, double textHeight, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw);

	static void worldDrawCartesianCoord(const McGePoint3d& insertPt, double textHeight, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw);

	static void worldDrawAlignedDim(const McGePoint3d& p1, const McGePoint3d& p2, double textHeight, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw);
	static void worldDrawLinearDim(Mx2dCustomLinearDim* pLinearDim, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw);

	static void worldDrawContinuousMeasurement(const Mx2d::PLVertexList& pts, double textHeight, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw);
	static void worldDrawSegmentLengths(Mx2dCustomContinuousMeasurement* pContinuousMeasurement, double textHeight, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw);

	static void worldDrawRadiusDim(Mx2dCustomRadiusDim* pRadiusDim, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw);
	static void worldDrawArcLengthDim(const McGePoint3d& startPt, const McGePoint3d& midPt, const McGePoint3d& endPt, double textHeight, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw);

	static void worldDrawCircleMeasurement(const McGePoint3d& centerPt, double radius, double textHeight, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw);

	static void worldDrawAngleMeasurement(const McGePoint3d& l1s, const McGePoint3d& l1e, const McGePoint3d& l2s, const McGePoint3d& l2e, double textHeight, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw);

	static void worldDrawArcPolyArea(const Mx2d::PLVertexList& pts, double textHeight, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw);

	static void worldDrawHatchArea2(const Mx2d::HatchPLList& polyArray, double textHeight, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw);

	static void worldDrawBatchMeasure(const Mx2d::CurveShapeList& curveShapes, double textHeight, McEdGetPointWorldDrawData* pData, McGiWorldDraw* pWorldDraw, bool isDynamicDrawing = false);

	static void drawRectByCorner(const McGePoint3d& corner1, const McGePoint3d& corner2, MxColor color, bool isReserve = false);

	// =======================================================

	// Calculate the cross product (b - a) × (c - a)
	static double cross(const McGePoint3d& a, const McGePoint3d& b, const McGePoint3d& c);

	// Check if point c is on the line segment ab (including endpoints)
	static bool isPointOnSegment(const McGePoint3d& a, const McGePoint3d& b, const McGePoint3d& c);

	// Check if two line segments ab and cd intersect (excluding vertex contact of adjacent edges)
	static bool segmentsIntersect(const McGePoint3d& a, const McGePoint3d& b, const McGePoint3d& c, const McGePoint3d& d);

	// Check if a polygon is self-intersecting
	static bool isPolygonSelfIntersecting(const McGePoint3dArray& polygon);

	// Get the minimum positive bounding box of a polygon
	static McDbExtents getPolygonGeomExtents(const McGePoint3dArray& pts);

	/**
	 * @brief Check if a point is inside a polygon (winding number algorithm).
	 * @return
	 *  1 - Point is inside the polygon
	 *  0 - Point is outside the polygon
	 * -1 - Point is on the boundary of the polygon
	 */
	static int isPointInPolygon(const McGePoint3d& pt, const McGePoint3dArray& pts);

	static int isEntityInPolygon(const McDbObjectId& id, const McGePoint3dArray& pts);

	static void getTextEntitiesInPolygon(Mx2d::TextInfoList& result, const McDbObjectIdArray& ids, const McGePoint3dArray& pts, const QString& containStr = QString(), bool isFull = false);

	// Get [single-line/multi-line] text objects within a polygonal area
	static McDbObjectIdArray getTextInPolygon(const McDbObjectIdArray& ids, const McGePoint3dArray& pts);

	static void deepTextInBlockRef(Mx2d::TextInfoList& result, const McDbBlockReference& blockRef, const McDbObjectId& curId, const QString& containStr, bool isFull);

};