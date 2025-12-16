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


	struct Mx2dExtents
	{
		double m_minX;
		double m_minY;
		double m_maxX;
		double m_maxY;
		Mx2dExtents(double minX, double minY, double maxX, double maxY) {
			m_minX = minX;
			m_minY = minY;
			m_maxX = maxX;
			m_maxY = maxY;
		}

		double centerX() {
			return (m_minX + m_maxX) / 2;
		}

		double centerY() {
			return (m_minY + m_maxY) / 2;
		}
	};

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

} // namespace Mx2d