/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once


#include "MxCADInclude.h"
#include <QList>
#include <QString>
#include "CurveShape.h"
#include <QJsonArray>

#define MxDblEpsilon 1e-10

namespace Mx2d {

	typedef struct {
		McGePoint3d pt;
		double bulge;
	} PLVertex;

	using PLVertexList = QList<PLVertex>;

	QJsonArray plVertexListToJsonArray(const PLVertexList& vertices);
	PLVertexList jsonArrayToPLVertexList(const QJsonArray& jsonArray);

	QJsonArray point3dToJsonArray2d(const McGePoint3d& pt);
	QJsonArray plVertexToJsonArray3d(const PLVertex& vertex);

	McGePoint3d jsonArray2dToPoint3d(const QJsonArray& jsonArray);
	PLVertex jsonArray3dToPLVertex(const QJsonArray& jsonArray);

	typedef struct {
		PLVertexList vertices;
		double length;
		double area;
	} PLVertexListWithMetrics;

	typedef struct {
		PLVertexListWithMetrics inner;
		PLVertexListWithMetrics outer;
	} HatchRegion;

	using HatchRegionList = QList<HatchRegion>;

	typedef struct {
		PLVertexList vertices;
		double length;
		double area;
		bool isExternal;
	} PLVertexListWithMetricsAndExtFlag;
	
	using HatchPLList = QList<PLVertexListWithMetricsAndExtFlag>;


	using CurveShapeList = QList<std::shared_ptr<Mx2d::CurveShape>>;

	struct Vector2d {
		double x;
		double y;
		Vector2d(double x_ = 0, double y_ = 0) : x(x_), y(y_) {}
	};

	Vector2d vectorProjection(const Vector2d& a, const Vector2d& b);

	struct TextLine
	{
		McGePoint3d pos; // left bottom point
		QString text;
	};

	QList<TextLine> getTextContent(McDbObjectId id);
	
	TextLine conbineOneLineText(QList<McDbText*> texts);

	McDbExtents getGeomExtents(const McDbObjectId& entId, bool roughCalculation = true);
	McDbExtents getGeomExtents(const McDbEntity* pEnt, bool roughCalculation = true);

	QList<TextLine> getMTextContent(McDbMText* pMText);


	QList<TextLine> findAllTextsInBlockRef(const McDbBlockReference* blockRef);
	QList<TextLine> findAllTextsInBlockRef(const McDbObjectId& blockRefId);


	
	struct Point2D
	{
		double x;
		double y;
		Point2D() {
            x = 0;
            y = 0;
		}
		Point2D(double x_, double y_) {
			x = x_;
			y = y_;
		}
	};
	using Point2DList = QList<Point2D>;

	// Calculate cross product of vectors (p1-p0) × (p2-p0)
	// This helps determine the relative position of point p2 with respect to line p0->p1
	double crossProduct(const Point2D& p0, const Point2D& p1, const Point2D& p2);
	// Check if point p lies on line segment ab (collinear and within bounding box)
	// Uses epsilon (1e-8) to handle floating-point precision errors
	bool isPointOnSegment(const Point2D& p, const Point2D& a, const Point2D& b);
	// Check if two line segments ab and cd intersect
	// Implements the "straddle test" for segment intersection with edge case handling
	bool isSegmentsIntersect(const Point2D& a, const Point2D& b, const Point2D& c, const Point2D& d);
	// Core function: Check if a closed polygon (Point2DList) is self-intersecting
	// A self-intersecting polygon is also known as a complex/non-simple polygon
	bool isPolygonSelfIntersecting(const Point2DList& polygon);
	using Rect2D = QPair<Point2D, Point2D>;

	struct Extents
	{
		double minX;
		double minY;
		double maxX;
		double maxY;
		Extents() {
			minX = 0;
			minY = 0;
			maxX = 0;
			maxY = 0;
		}
		Extents(double _minX, double _minY, double _maxX, double _maxY) {
			minX = _minX;
			minY = _minY;
			maxX = _maxX;
			maxY = _maxY;
		}

		double centerX() {
			return (minX + maxX) / 2;
		}

		double centerY() {
			return (minY + maxY) / 2;
		}

		Point2D center() {
			return {centerX(), centerY()};
		}

		Point2D topLeft() {
			return { qMin(minX, maxX), qMax(minY,maxY) };
		}
		Point2D topRight() {
			return { qMax(minX, maxX), qMax(minY, maxY) };
		}
        Point2D bottomLeft() {
			return { qMin(minX, maxX), qMin(minY, maxY) };
		}
        Point2D bottomRight() {
			return { qMax(minX, maxX), qMin(minY, maxY) };
		}

		McGePoint3d center3d() {
			return { centerX(), centerY(), 0 };
		}

		McGePoint3d topLeft3d() {
			return { qMin(minX, maxX), qMax(minY,maxY), 0 };
		}
		McGePoint3d topRight3d() {
			return { qMax(minX, maxX), qMax(minY, maxY), 0 };
		}
		McGePoint3d bottomLeft3d() {
			return { qMin(minX, maxX), qMin(minY, maxY), 0 };
		}
		McGePoint3d bottomRight3d() {
			return { qMax(minX, maxX), qMin(minY, maxY), 0 };
		}
	};
	using TextInfo = QPair<QString, Mx2d::Extents>;
	using TextInfoList = QList<TextInfo>;

	struct Mx2dLayerInfoDetail {
		McDbObjectId id;
		bool isOff;
		QString name;
		McCmColor color;
	};

	struct Mx2dLayerInfo {
		QList<Mx2dLayerInfoDetail> datas;
		QWidget* pTab;
	};

	struct LayerInfoDetail {
		McDbObjectId id;
		bool isOff;
		QString name;
		McCmColor color;
	};

	using LayerInfo = QList<LayerInfoDetail> ;

	void recursiveExplodeBlock(McDbBlockReference* block, McDbVoidPtrArray& entitySet);

	bool isIntersect(McGePoint3d& line1_st, McGePoint3d& line1_end, McGePoint3d& line2_st, McGePoint3d& line2_end);
	bool isInRect(McGePoint3d& line_st, McGePoint3d& line_end, double rect_left, double rect_right, double rect_bottom, double rect_top);

	bool IsEqual(const double& d1, const double& d2, const double& dTol = MxDblEpsilon);

	bool cmpVLine(McDbLine* line1, McDbLine* line2);
	bool cmpHLine(McDbLine* line1, McDbLine* line2);



	struct ContinueRange
	{
		double start;
		double end;
		ContinueRange(double _start, double _end) :start(_start), end(_end) {}

		bool operator==(struct ContinueRange const& that) const {
			if (start - 0.01 <= that.start && end + 0.01 >= that.end)
				return true;
			return false;
		}
	};
	struct CellRect
	{
		double left;
		double right;
		double bottom;
		double top;
		int columnStart;		
		int columnEnd;			
		int rowStart;			
		int rowEnd;				
		QString texts;			

		CellRect(double _left, double _right, double _bottom, double _top) :left(_left), right(_right), bottom(_bottom), top(_top) {}

		bool operator==(struct CellRect const& that) const {
			if (left == that.left && right == that.right && bottom == that.bottom && top == that.top)
				return true;
			return false;
		}
		inline bool beLongTo(struct CellRect const& that) {
			if (this->left >= that.left && this->right <= that.right && this->bottom >= that.bottom && this->top <= that.top)
				return true;
			return false;
		}
	};

	double getViewToDocScaleRatio();

	bool getArcSegmentAtPoint(const McGePoint3d& pt, McGePoint3d& arcStart, McGePoint3d& arcEnd, double& bulge);

	bool isBetweenTwoVecs(const McGeVector3d& vec1, const McGeVector3d& vec2, const McGeVector3d& vec);

    int indexArcBefore(const McGePoint3d& pt, McDbPolyline* pPoly);

    int indexLineBefore(const McGePoint3d& pt, McDbPolyline* pPoly);

    void calcArc(const McGePoint3d& ptStart, const McGePoint3d& ptEnd, double bulge, McGePoint3d& center, double& radius, double& angle);

	void calcArc2(const McGePoint3d& ptStart, const McGePoint3d& ptEnd, double bulge, McGePoint3d& center, double& radius, double& angle, McGePoint3d& ptMid);
	
	double getPolylineLength(McDbPolyline* pPoly);

	bool getPolygonOffsetPoints(McDbPolyline* pPoly, McGePoint3d& innerOffsetPt, McGePoint3d& outerOffsetPt);

    bool isClockwise(const McGePoint3dArray& pts);
    bool isClockwise(McDbPolyline* pPoly);

	McDbObjectId addEntityToAnnotationLayerAndClose(McDbEntity* pEnt);

	void execCmd2d(QWidget* tab, QString cmdStr, struct resbuf* param = nullptr);

	QString getEntityLayoutName(McDbObjectId entId);

	QString getBlockTableRecordLayoutName(McDbObjectId btrId);

	Extents getPolygonGeomExtents(const McGePoint3dArray& pts);

	int isPointInPolygon(const Point2D& pt, const Point2DList& pts);

} // namespace Mx2d