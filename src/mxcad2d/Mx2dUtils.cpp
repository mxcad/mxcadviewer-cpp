/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dUtils.h"
#include <algorithm>
#include <QDebug>
#include "Mx2dGuiDocument.h"
#include "MxLogger.h"
#include "Mx2dCustomAnnotation.h"
namespace {

	bool isSameLineText(McDbText* text, McDbText* other) {

		McDbExtents textExt, otherExt;
		text->getGeomExtents(textExt, false);
		other->getGeomExtents(otherExt, false);
		if (otherExt.minPoint().y > textExt.maxPoint().y || otherExt.maxPoint().y < textExt.minPoint().y)
			return false;
		return true;
	}

	QList<McDbText*> findSameLineText(McDbText* text, QList<McDbText*> others) {
		QList<McDbText*> res;
		for (auto & other : others)
		{
			if(text == other) continue;
			if (isSameLineText(text, other))
			{
				res.append(other);
			}
		}
		res.append(text);
		return res;
	}

	

}

namespace Mx2d {
	QJsonArray plVertexListToJsonArray(const PLVertexList& vertices)
	{
        QJsonArray res;
        for (auto & vertex : vertices)
		{
			QJsonArray v;
			v.append(vertex.pt.x);
			v.append(vertex.pt.y);
			v.append(vertex.bulge);
			res.append(v);
		}
		return res;
	}
	PLVertexList jsonArrayToPLVertexList(const QJsonArray& jsonArray)
	{
        PLVertexList res;
        for (auto & json : jsonArray)
		{
			PLVertex vertex;
			vertex.pt.x = json[0].toDouble();
			vertex.pt.y = json[1].toDouble();
			vertex.bulge = json[2].toDouble();
			res.append(vertex);
		}
        return res;
	}
	QJsonArray point3dToJsonArray2d(const McGePoint3d& pt)
	{
        return QJsonArray() << pt.x << pt.y;
	}
	QJsonArray plVertexToJsonArray3d(const PLVertex& vertex)
	{
        return QJsonArray() << point3dToJsonArray2d(vertex.pt) << vertex.bulge;
	}
	McGePoint3d jsonArray2dToPoint3d(const QJsonArray& jsonArray)
	{
		return McGePoint3d(jsonArray[0].toDouble(), jsonArray[1].toDouble(), 0);
	}

	PLVertex jsonArray3dToPLVertex(const QJsonArray& jsonArray)
	{
		return { jsonArray2dToPoint3d(jsonArray) ,jsonArray[2].toDouble() };
	}
	Vector2d vectorProjection(const Vector2d& a, const Vector2d& b)
	{
		double dotProduct = a.x * b.x + a.y * b.y;

		double bMagnitudeSq = b.x * b.x + b.y * b.y;

		if (bMagnitudeSq < 1e-9) {
			return Vector2d(0, 0);
		}

		double scale = dotProduct / bMagnitudeSq;
		return Vector2d(scale * b.x, scale * b.y);
	}
	QList<TextLine> Mx2d::getTextContent(McDbObjectId id)
	{
		McDbEntityPointer spEntity(id, McDb::kForRead);
		if (spEntity.openStatus() != Mcad::eOk) return {};

		if (spEntity->isA() == McDbText::desc())
		{
			McDbText* pText = McDbText::cast(spEntity.object());
			QString str = QString::fromLocal8Bit(pText->textString());
			McDbExtents ext;
			pText->getGeomExtents(ext, false);
			return { { ext.minPoint(), str } }; 
		}
		else if (spEntity->isA() == McDbMText::desc())
		{
			McDbVoidPtrArray arr;
			Mcad::ErrorStatus state = spEntity->explode(arr);
			if (state != Mcad::eOk) return {};

			QList<McDbText*> notProcessedTexts;
			for (int i = 0; i < arr.length(); ++i)
			{
				McDbText* pText = static_cast<McDbText*>(arr[i]);
				notProcessedTexts.append(pText);
			}
			QList<TextLine> resStr;
			while (!notProcessedTexts.isEmpty())
			{
				QList<McDbText*> sameLineTempList;
				QList<McDbText*> notSameLineTempList;

				McDbText* pFirstText = notProcessedTexts[0];

				sameLineTempList.append(pFirstText);

				for (int i = 1; i < notProcessedTexts.length(); ++i)
				{
					McDbText* pCurText = notProcessedTexts[i];
					if (isSameLineText(pFirstText, pCurText))
					{
						sameLineTempList.append(pCurText);
					}
					else 
					{
						notSameLineTempList.append(pCurText);
					}
				}

				TextLine line = conbineOneLineText(sameLineTempList);
				resStr.append(line);
				notProcessedTexts = notSameLineTempList;
			}

			// sort
			std::sort(resStr.begin(), resStr.end(), [](TextLine a, TextLine b) {
				return a.pos.y > b.pos.y;
				});
			
			for (int i = 0; i < arr.length(); ++i)
			{
				delete arr[i];
			}
			arr.clear();
			return resStr;
		}
		else if (spEntity->isA() == McDbBlockReference::desc())
		{
			McDbBlockReference* blockRef = McDbBlockReference::cast(spEntity.object());
			return findAllTextsInBlockRef(blockRef);
		}

		return {};
	}
	TextLine conbineOneLineText(QList<McDbText*> texts)
	{
		std::sort(texts.begin(), texts.end(), [](McDbText* a, McDbText* b) {
			McDbExtents extA, extB;
			a->getGeomExtents(extA, false);
			b->getGeomExtents(extB, false);
			return extA.minPoint().x < extB.minPoint().x;
			});

		
		McDbExtents firstSubStrExt;
		texts[0]->getGeomExtents(firstSubStrExt, false);
		McGePoint3d pos = firstSubStrExt.minPoint();
		QString str;
		for (McDbText * pText : texts)
		{
			QString subStr = QString::fromLocal8Bit(pText->textString());
			str += subStr;
		}
		TextLine textLine;
		textLine.pos = pos;
		textLine.text = str;

		return textLine;
	}
	double crossProduct(const Point2D& p0, const Point2D& p1, const Point2D& p2)
	{
		return (p1.x - p0.x) * (p2.y - p0.y) - (p1.y - p0.y) * (p2.x - p0.x);
	}
	bool isPointOnSegment(const Point2D& p, const Point2D& a, const Point2D& b)
	{
		// Check if p's coordinates are within the bounding box of a and b
		if (qMin(a.x, b.x) - 1e-8 <= p.x && p.x <= qMax(a.x, b.x) + 1e-8 &&
			qMin(a.y, b.y) - 1e-8 <= p.y && p.y <= qMax(a.y, b.y) + 1e-8)
		{
			// Cross product near zero means collinear (allow minor precision error)
			return qAbs(crossProduct(a, b, p)) < 1e-8;
		}
		return false;
	}
	bool isSegmentsIntersect(const Point2D& a, const Point2D& b, const Point2D& c, const Point2D& d)
	{
		double cp1 = crossProduct(a, b, c);
		double cp2 = crossProduct(a, b, d);
		double cp3 = crossProduct(c, d, a);
		double cp4 = crossProduct(c, d, b);

		// Straddle test: segments straddle each other (cross products have opposite signs)
		// Epsilon (1e-8) avoids false positives from floating-point precision issues
		if (((cp1 > 1e-8 && cp2 < -1e-8) || (cp1 < -1e-8 && cp2 > 1e-8)) &&
			((cp3 > 1e-8 && cp4 < -1e-8) || (cp3 < -1e-8 && cp4 > 1e-8)))
		{
			return true;
		}

		// Handle edge cases where endpoints lie on the other segment (collinear intersection)
		if (isPointOnSegment(c, a, b)) return true;
		if (isPointOnSegment(d, a, b)) return true;
		if (isPointOnSegment(a, c, d)) return true;
		if (isPointOnSegment(b, c, d)) return true;

		return false;
	}
	bool isPolygonSelfIntersecting(const Point2DList& polygon)
	{
		// A polygon needs at least 4 vertices to be self-intersecting (triangle can't self-intersect)
		if (polygon.size() < 4)
		{
			return false;
		}

		int n = polygon.size();
		// Iterate through each edge of the polygon (i -> i+1)
		for (int i = 0; i < n; ++i)
		{
			Point2D a = polygon[i];
			Point2D b = polygon[(i + 1) % n]; // Last vertex connects back to first (closed polygon)

			// Iterate through other edges to check intersection (avoid adjacent/duplicate checks)
			for (int j = i + 2; j < n; ++j)
			{
				// Skip the last edge when i=0 (avoids checking adjacent edges: n-1 -> 0 and 0 -> 1)
				if (i == 0 && j == n - 1)
				{
					continue;
				}

				Point2D c = polygon[j];
				Point2D d = polygon[(j + 1) % n];

				// Check if current edge (a-b) intersects with edge (c-d)
				if (isSegmentsIntersect(a, b, c, d))
				{
					return true; // Self-intersection found, return immediately
				}
			}
		}

		// No self-intersections found in any edge pairs
		return false;
	}
	void recursiveExplodeBlock(McDbBlockReference* block, McDbVoidPtrArray& entitySet)
	{
		McDbVoidPtrArray _entitySet;
		block->explode(_entitySet);
		for (int i = 0; i < _entitySet.length(); i++)
		{
			McDbEntity* pEntity = (McDbEntity*)_entitySet[i];
			if (pEntity->isA() == McDbBlockReference::desc())
			{
				McDbBlockReference* pBlockRef = McDbBlockReference::cast(pEntity);
				std::unique_ptr<McDbBlockReference> spBlockRef(pBlockRef);
				recursiveExplodeBlock(spBlockRef.get(), _entitySet);
			}
			else
			{
				entitySet.append((void*)pEntity);
			}
		}
	}
	bool isIntersect(McGePoint3d& line1_st, McGePoint3d& line1_end, McGePoint3d& line2_st, McGePoint3d& line2_end)
	{
		double D1x = line1_end.x - line1_st.x;
		double D1y = line1_end.y - line1_st.y;
		double D2x = line2_end.x - line2_st.x;
		double D2y = line2_end.y - line2_st.y;
		double O12x = line2_st.x - line1_st.x;
		double O12y = line2_st.y - line1_st.y;
		double D1 = D2y * O12x - D2x * O12y;
		double D2 = D1y * O12x - D1x * O12y;
		double D = D1x * D2y - D1y * D2x;
		if (D == 0)
			return false;
		double t1 = D1 / D;
		if (t1 < 0 || t1>1)
			return false;
		double t2 = D2 / D;
		if (t2 < 0 || t2>1)
			return false;
		return true;
	}
	bool isInRect(McGePoint3d& line_st, McGePoint3d& line_end, double rect_left, double rect_right, double rect_bottom, double rect_top)
	{
		if (line_st.x >= rect_left && line_st.x <= rect_right && line_st.y >= rect_bottom && line_st.y <= rect_top && line_end.x >= rect_left && line_end.x <= rect_right && line_end.y >= rect_bottom && line_end.y <= rect_top)
			return true;
		return false;
	}
	bool IsEqual(const double& d1, const double& d2, const double& dTol)
	{
		return std::abs(d1 - d2) <= dTol;
	}
	bool cmpVLine(McDbLine* line1, McDbLine* line2)
	{
		McGePoint3d line1BottomPoint = line1->startPoint().y < line1->endPoint().y ? line1->startPoint() : line1->endPoint();
		McGePoint3d line2BottomPoint = line2->startPoint().y < line2->endPoint().y ? line2->startPoint() : line2->endPoint();
		return line1BottomPoint.y < line2BottomPoint.y;
	}
	bool cmpHLine(McDbLine* line1, McDbLine* line2)
	{
		McGePoint3d line1LeftPoint = line1->startPoint().x < line1->endPoint().x ? line1->startPoint() : line1->endPoint();
		McGePoint3d line2LeftPoint = line2->startPoint().x < line2->endPoint().x ? line2->startPoint() : line2->endPoint();
		return line1LeftPoint.x < line2LeftPoint.x;
	}
	double getViewToDocScaleRatio()
	{
		double viewLong = 100.0;
		double docLong = MxDrawApp::ViewLongToDoc(viewLong);
		return viewLong / docLong;
	}

	bool getArcSegmentAtPoint(const McGePoint3d& pt, McGePoint3d& arcStart, McGePoint3d& arcEnd, double& bulge)
	{
		McDbObjectId entId = MrxDbgUtils::findEntAtPoint(pt);
		if (entId.isNull())
		{
			return false;
		}
		McDbEntityPointer spEnt(entId, McDb::kForRead);
		if (spEnt.openStatus() != Mcad::eOk) return false;
		if (spEnt->isA() == McDbArc::desc())
		{
			McDbArc* pArc = McDbArc::cast(spEnt.object());
			pArc->getStartPoint(arcStart);
			pArc->getEndPoint(arcEnd);
			double arcLen = 0.0;
			pArc->getDistAtPoint(arcEnd, arcLen);
			//const double kEpsilon = arcLen / 10;
			if (pt.distanceTo(arcStart) <= MxDblEpsilon || pt.distanceTo(arcEnd) <= MxDblEpsilon)
			{
				return false;
			}

			double startParam = 0.0;
			double endParam = 0.0;
			pArc->getParamAtPoint(arcStart, startParam);
			pArc->getParamAtPoint(arcEnd, endParam);
			double midParam = (startParam + endParam) / 2.0;
			McGePoint3d midPt;
			pArc->getPointAtParam(midParam, midPt);
			McGePoint3d coordMidPt((arcStart.x + arcEnd.x) / 2.0, (arcStart.y + arcEnd.y) / 2.0, 0);
			double dDY = midPt.distanceTo(coordMidPt);
			double dDX = arcStart.distanceTo(arcEnd) / 2.0;
			bulge = dDY / dDX; // arc, bulge always positive
			return true;
		}
		else if (spEnt->isA() == McDbPolyline::desc())
		{
            McDbPolyline* pPoly = McDbPolyline::cast(spEnt.object());
			int index = indexArcBefore(pt, pPoly);
			if (index == -1) {
				return false;
			}

			McGePoint3d startPt, endPt; 
			pPoly->getPointAt(index, startPt);

			if (index == pPoly->numVerts() - 1 && pPoly->isClosed())
			{
				pPoly->getStartPoint(endPt);
			}
			else
			{
				pPoly->getPointAt(index + 1, endPt);
			}
			if (pt.distanceTo(startPt) < MxDblEpsilon || pt.distanceTo(endPt) < MxDblEpsilon)
			{
				return false;
			}

			arcStart = startPt;
			arcEnd = endPt;
			pPoly->getBulgeAt(index, bulge);

			return true;
			
		}
		return false;
	}

	// Determine if a vector is between two other vectors (inclusive of boundaries)
	bool isBetweenTwoVecs(const McGeVector3d& vec1, const McGeVector3d& vec2, const McGeVector3d& vec)
	{
		const double EPS = 1e-9;
		// Exclude zero vectors (zero vectors have no direction meaning)
		if (vec1.isZeroLength(EPS) || vec2.isZeroLength(EPS) || vec.isZeroLength(EPS))
			return false;

		// Calculate normalized versions of vectors (avoid length influence, focus on direction only)
		McGeVector3d u = vec1.normal();
		McGeVector3d v = vec2.normal();
		McGeVector3d w = vec.normal();

		// Calculate key cross products (take Z component to determine direction relationship in XY plane)
		// Cross product Z component > 0: counterclockwise; < 0: clockwise; ≈ 0: collinear
		double cross_uv = u.crossProduct(v).z;  // Rotation direction from u to v
		double cross_uw = u.crossProduct(w).z;  // Rotation direction from u to w
		double cross_vw = v.crossProduct(w).z;  // Rotation direction from v to w

		// Case 1: vec1 and vec2 are collinear (angle is 0 or 180 degrees)
		if (fabs(cross_uv) < EPS)
		{
			// If two vectors are in the same direction (dot product > 0), then w must be in the same direction (dot products both > 0)
			if (u.dotProduct(v) > EPS)
			{
				return (u.dotProduct(w) > EPS) && (v.dotProduct(w) > EPS);
			}
			// If two vectors are opposite (180 degree angle), no middle vector exists
			else
			{
				return false;
			}
		}

		// Case 2: vec1 and vec2 are not collinear (there is an angle between them)
		// Core logic: w must be within the minimum angle between u and v, meaning it has the same rotation direction as u and v
		// 1. If u to v is counterclockwise (cross_uv > 0), then u to w and v to w must also be counterclockwise or collinear
		if (cross_uv > EPS)
		{
			return (cross_uw >= -EPS) && (cross_vw <= EPS);
		}
		// 2. If u to v is clockwise (cross_uv < 0), then u to w and v to w must also be clockwise or collinear
		else
		{
			return (cross_uw <= EPS) && (cross_vw >= -EPS);
		}
	}

	int indexArcBefore(const McGePoint3d& pt, McDbPolyline* pPoly)
	{
		if (!pPoly) return -1;

		double ptParam = 0.0;
		Mcad::ErrorStatus es = pPoly->getParamAtPoint(pt, ptParam);
		if (es != Mcad::eOk) return -1;
        for (unsigned int i = 0; i < pPoly->numVerts(); i++)
        {
			McGePoint3d curPt;
			pPoly->getPointAt(i, curPt);
			double curPtParam = 0.0;
			pPoly->getParamAtPoint(curPt, curPtParam);
			if (curPtParam > ptParam)
			{
                double prevBulge = 0.0;
                pPoly->getBulgeAt(i - 1, prevBulge);
				if (!qFuzzyCompare(prevBulge, 0.0))
				{
					return i - 1;
				}
				else
				{
					return -1;
				}
			}
			if (i == pPoly->numVerts() - 1 && pPoly->isClosed())
			{
				double bulge = 0.0;
                pPoly->getBulgeAt(i, bulge);
                if (!qFuzzyCompare(bulge, 0.0))
				{
					return i;
				}
				else
				{
					return -1;
				}
			}
        }
		return -1;
	}
	int indexLineBefore(const McGePoint3d& pt, McDbPolyline* pPoly)
	{
		if (!pPoly) return -1;

		double ptParam = 0.0;
		Mcad::ErrorStatus es = pPoly->getParamAtPoint(pt, ptParam);
		if (es != Mcad::eOk) return -1;
		for (unsigned int i = 0; i < pPoly->numVerts(); i++)
		{
			McGePoint3d curPt;
			pPoly->getPointAt(i, curPt);
			double curPtParam = 0.0;
			pPoly->getParamAtPoint(curPt, curPtParam);
			if (curPtParam > ptParam)
			{
				double prevBulge = 0.0;
				pPoly->getBulgeAt(i - 1, prevBulge);
				if (!qFuzzyCompare(prevBulge, 0.0))
				{
					return -1;
				}
				else
				{
					return i - 1;
				}
			}
			if (i == pPoly->numVerts() - 1 && pPoly->isClosed())
			{
				double bulge = 0.0;
				pPoly->getBulgeAt(i, bulge);
				if (!qFuzzyCompare(bulge, 0.0))
				{
					return -1;
				}
				else
				{
					return i;
				}
			}
		}
		return -1;
	}

	void calcArc(const McGePoint3d& ptStart, const McGePoint3d& ptEnd, double bulge, McGePoint3d& center, double& radius, double& angle)
	{
		if (ptStart.distanceTo(ptEnd) < MxDblEpsilon)
		{
			center = ptStart;
            radius = 0;
            angle = 0;
			return;
		}

		double dDX = ptStart.distanceTo(ptEnd) / 2.0;
		double dDY = fabs(bulge) * dDX;
		double dR = ((dDX * dDX) + (dDY * dDY)) / (2.0 * dDY);

		radius = dR;

		McGeVector3d dir = McGeVector3d(ptEnd - ptStart).rotateBy(M_PI / 2.0, McGeVector3d::kZAxis).normal();
		if (bulge > 0)
		{
			dir = -dir;
		}
		McGePoint3d midCoordPt((ptStart.x + ptEnd.x) / 2.0, (ptStart.y + ptEnd.y) / 2.0, 0);
		McGePoint3d center_ = midCoordPt + dir * (dDY - dR);

		center = center_;

		McGeVector3d vecStart(ptStart - center_);
		McGeVector3d vecEnd(ptEnd - center_);
		McGeVector3d vecArcMid = vecStart;


		double sweepLength = vecStart.angleTo(vecEnd);
		if (dDY > dR)
		{
			sweepLength = M_PI * 2.0 - sweepLength;
		}
		angle = sweepLength;
	}

	void calcArc2(const McGePoint3d& ptStart, const McGePoint3d& ptEnd, double bulge, McGePoint3d& center, double& radius, double& angle, McGePoint3d& ptMid)
	{
		calcArc(ptStart, ptEnd, bulge, center, radius, angle);
		double arcMidRot = angle / 2.0;
		McGeVector3d vecArcMid(ptStart - center);
		if (bulge < 0.0)
		{
			arcMidRot = -arcMidRot;
		}
		vecArcMid.rotateBy(arcMidRot, McGeVector3d::kZAxis);
		ptMid = center + vecArcMid;
	}

	double getPolylineLength(McDbPolyline* pPoly)
	{
		if (pPoly != nullptr) {
			double perimeter = 0.0;

            McGePoint3d endPt;
			pPoly->getEndPoint(endPt);
			double endBulge = 0.0;
            pPoly->getBulgeAt(pPoly->numVerts() - 1, endBulge);

			double endParam = 0.0;
            pPoly->getEndParam(endParam);
			pPoly->getDistAtParam(endParam, perimeter);

			McGePoint3d startPt;
            pPoly->getStartPoint(startPt);

			if (Mx2d::IsEqual(endBulge, 0.0))
			{
				perimeter += startPt.distanceTo(endPt);
			}
			else
			{
				McGePoint3d center;
				double radius, angle;
				Mx2d::calcArc(endPt, startPt, endBulge, center, radius, angle);
				perimeter += radius * angle;
			}


			return perimeter;
		}
		return 0.0;
	}


	bool getPolygonOffsetPoints(McDbPolyline* pPoly, McGePoint3d& innerOffsetPt, McGePoint3d& outerOffsetPt)
	{
		if (pPoly == nullptr || !pPoly->isClosed())
		{
			return false;
		}

		bool findLineSegement = false;
		int lineSegIndex = -1;
		for (unsigned int i = 0; i < pPoly->numVerts(); ++i) {
			double bulge = 0.0;
            pPoly->getBulgeAt(i, bulge);
			if (Mx2d::IsEqual(bulge, 0.0))
			{
				findLineSegement = true;
				lineSegIndex = i;
				break;
			}
		}
		McGePoint3d firstPt, secondPt, midPt;
		if (findLineSegement)
		{
			if (lineSegIndex == pPoly->numVerts() - 1)
			{
				pPoly->getPointAt(lineSegIndex, firstPt);
                pPoly->getPointAt(0, secondPt);
			}
			else
			{
				pPoly->getPointAt(lineSegIndex, firstPt);
				pPoly->getPointAt(lineSegIndex + 1, secondPt);
			}
			midPt = McGePoint3d((firstPt.x + secondPt.x) / 2.0, (firstPt.y + secondPt.y) / 2.0, 0);
		}
		else
		{
			pPoly->getPointAt(0, firstPt);
			pPoly->getPointAt(1, secondPt);
			double bulge = 0.0;
			pPoly->getBulgeAt(0, bulge);
			McGePoint3d center;
			double radius = 0.0;
			double angle = 0.0;
			Mx2d::calcArc(firstPt, secondPt, bulge, center, radius, angle);
			double arcMidRot = angle / 2.0;
			McGeVector3d vecArcMid(firstPt - center);
			if (bulge < 0.0) // 顺时针
			{
				arcMidRot = -arcMidRot;
			}
			vecArcMid.rotateBy(arcMidRot, McGeVector3d::kZAxis);
			midPt = center + vecArcMid;
		}


		McGeVector3d dir = McGeVector3d(secondPt - firstPt).normal();
		McGeVector3d offsetDir1 = dir;
		McGeVector3d offsetDir2 = dir;
		offsetDir1.rotateBy(M_PI / 2.0, McGeVector3d::kZAxis);
		offsetDir2.rotateBy(-M_PI / 2.0, McGeVector3d::kZAxis);

		McGePoint3d innerPt, outerPt;
		bool success = false;

		McGePoint3d offsetPt1 = midPt + offsetDir1 * MxDblEpsilon;
		McGePoint3d offsetPt2 = midPt + offsetDir2 * MxDblEpsilon;
		McDbVoidPtrArray offsetCurves1;
		McDbVoidPtrArray offsetCurves2;
		Mcad::ErrorStatus es1 = pPoly->getOffsetCurvesEx(MxDblEpsilon, offsetCurves1, offsetPt1);
		Mcad::ErrorStatus es2 = pPoly->getOffsetCurvesEx(MxDblEpsilon, offsetCurves2, offsetPt2);
		if (es1 == Mcad::eOk && es2 == Mcad::eOk && offsetCurves1.length() == 1 && offsetCurves2.length() == 1) {
			McDbPolyline* pOffsetCurve1 = static_cast<McDbPolyline*>(offsetCurves1[0]);
			McDbPolyline* pOffsetCurve2 = static_cast<McDbPolyline*>(offsetCurves2[0]);
			if (pOffsetCurve1 && pOffsetCurve2)
			{
				double endParam1 = 0.0, endParam2 = 0.0;
				pOffsetCurve1->getEndParam(endParam1);
				pOffsetCurve2->getEndParam(endParam2);
				double length1 = 0.0, length2 = 0.0;
				pOffsetCurve1->getDistAtParam(endParam1, length1);
				pOffsetCurve2->getDistAtParam(endParam2, length2);
				if (length1 > length2)
				{
					outerPt = offsetPt1;
					innerPt = offsetPt2;
				}
				else
				{
					outerPt = offsetPt2;
					innerPt = offsetPt1;
				}
				if (length1 != length2)
				{
					success = true;
				}
			}
			delete pOffsetCurve1;
			delete pOffsetCurve2;
		}

		if (success)
		{
			innerOffsetPt = innerPt;
            outerOffsetPt = outerPt;
            return true;
		}

		return false;
	}

	bool isClockwise(const McGePoint3dArray& pts)
	{
		double sumCross = 0.0;
		for (int i = 0; i < pts.length(); i++)
		{
			McGePoint3d A = pts[i];
			McGePoint3d B = pts[(i + 1) % pts.length()];
			McGePoint3d C = pts[(i + 2) % pts.length()];
			double x1 = A.x, y1 = A.y;
			double x2 = B.x, y2 = B.y;
			double x3 = C.x, y3 = C.y;
            sumCross += (x2 - x1) * (y3 - y1) - (x3 - x1) * (y2 - y1);
		}
		
		if (sumCross < 0.0)
		{
			return true;
		}
		return false;
	}

	bool isClockwise(McDbPolyline* pPoly)
	{
		double sumCross = 0.0;
		int size = pPoly->numVerts();
		for (int i = 0; i < size; i++)
		{
			McGePoint3d A, B, C;
			pPoly->getPointAt(i, A);
			pPoly->getPointAt((i + 1) % size, B);
			pPoly->getPointAt((i + 2) % size, C);
			double x1 = A.x, y1 = A.y;
			double x2 = B.x, y2 = B.y;
			double x3 = C.x, y3 = C.y;
			sumCross += (x2 - x1) * (y3 - y1) - (x3 - x1) * (y2 - y1);
		}

		if (sumCross < 0.0)
		{
			return true;
		}
		return false;
	}

	McDbObjectId addEntityToAnnotationLayerAndClose(McDbEntity* pEnt)
	{
		if (pEnt)
		{
			McCmColor newColor; newColor.setRGB(230, 81, 0);
			pEnt->setColor(newColor);
			McDbObjectId id, curSpaceId;
			curSpaceId = Mx::mcdbCurDwg()->currentSpaceId();
			QString layoutName = Mx2d::getBlockTableRecordLayoutName(curSpaceId);
			McDbBlockTableRecordPointer spCurSpace(curSpaceId, McDb::kForWrite);
			if (spCurSpace.openStatus() == Mcad::eOk) {
				spCurSpace->appendAcDbEntity(id, pEnt);
				if (pEnt->isKindOf(Mx2dCustomAnnotation::desc()))
				{
                    Mx2dCustomAnnotation* pAnnotation = Mx2dCustomAnnotation::cast(pEnt);
					pAnnotation->setLayout(layoutName);
				}
				pEnt->close();
				McDbEntityPointer spEntity(id, McDb::kForWrite);
				spEntity->setLayer("MxCADAnnotationLayer");
				return id;
			}
			else {
				delete pEnt;
			}
		}
        return McDbObjectId::kNull;
	}


	void execCmd2d(QWidget* tab, QString cmdStr, struct resbuf* param)
	{
		Mx2dGuiDocument* pCadGuiDoc = qobject_cast<Mx2dGuiDocument*>(tab);
		if (!pCadGuiDoc)
		{
            LOG_ERROR(QString("tab is invalid!"));
			return;
		}
		if (!pCadGuiDoc->executeCommand(cmdStr, param))
		{
            LOG_ERROR(QString("failed to execute %1!").arg(cmdStr));
		}
	}

	QString getEntityLayoutName(McDbObjectId entId)
	{
		McDbObjectPointer<McDbEntity> spEnt(entId, McDb::kForRead);
		if (spEnt.openStatus() != Mcad::eOk)
			return {};

		McDbObjectId ownerId = spEnt->ownerId();

		McDbDatabase* pDatabase = spEnt->database();
		McDbDictionary* pDict = nullptr;
		if (pDatabase->getLayoutDictionary(pDict, McDb::kForRead) != Mcad::eOk)
			return {};

		McDbDictionaryIterator* pIter = pDict->newIterator();
		MXASSERT(pIter != nullptr);
		QString qsLayoutName;
		for (; !pIter->done(); pIter->next())
		{
			McDbObjectPointer<McDbLayout> spLayout(pIter->objectId(), McDb::kForRead);
			if (spLayout.openStatus() != Mcad::eOk)
				continue;

			if (spLayout->getBlockTableRecordId() == ownerId)
			{
				MxString sLayoutName;
				spLayout->getLayoutName(sLayoutName);
				qsLayoutName = QString::fromLocal8Bit(sLayoutName.c_str());
				break;
			}

		}
		pDict->close();
		delete pIter;

		return qsLayoutName;
	}

	QString getBlockTableRecordLayoutName(McDbObjectId btrId)
	{
		McDbBlockTableRecordPointer spBTR(btrId, McDb::kForRead);
		if (spBTR.openStatus() != Mcad::eOk)
			return {};
		McDbDatabase* pDatabase = spBTR->database();
		McDbDictionary* pDict = nullptr;
		if (pDatabase->getLayoutDictionary(pDict, McDb::kForRead) != Mcad::eOk)
			return {};

		McDbDictionaryIterator* pIter = pDict->newIterator();
		MXASSERT(pIter != nullptr);
		QString qsLayoutName;
		for (; !pIter->done(); pIter->next())
		{
			McDbObjectPointer<McDbLayout> spLayout(pIter->objectId(), McDb::kForRead);
			if (spLayout.openStatus() != Mcad::eOk)
				continue;

			if (spLayout->getBlockTableRecordId() == btrId)
			{
				MxString sLayoutName;
				spLayout->getLayoutName(sLayoutName);
				qsLayoutName = QString::fromLocal8Bit(sLayoutName.c_str());
				break;
			}

		}
		pDict->close();
		delete pIter;

		return qsLayoutName;
	}

	Extents getPolygonGeomExtents(const McGePoint3dArray& pts)
	{
		McDbPolyline pline;
		for (int i = 0; i < pts.length(); i++)
		{
			pline.addVertexAt(pts[i]);
		}
		pline.setClosed(true);
		McDbExtents ext;
		pline.getGeomExtents(ext, false);
		Extents ext2d{ ext.minPoint().x,ext.minPoint().y,ext.maxPoint().x,ext.maxPoint().y };
		return ext2d;
	}

	int isPointInPolygon(const Point2D& pt, const Point2DList& pts)
	{
		int vertexCount = pts.size();
		// A valid polygon requires at least 3 vertices; return 0 (outside) if not met
		if (vertexCount < 3) return 0;

		int windingNumber = 0;

		for (int i = 0; i < vertexCount; ++i) {
			const Point2D& a = pts[i];
			const Point2D& b = pts[(i + 1) % vertexCount];

			if (isPointOnSegment(pt, a, b)) {
				return -1;  // Point is on boundary
			}

			double dx1 = a.x - pt.x;
			double dy1 = a.y - pt.y;
			double dx2 = b.x - pt.x;
			double dy2 = b.y - pt.y;

			double cross = dx1 * dy2 - dy1 * dx2;

			double dot = dx1 * dx2 + dy1 * dy2;

			if (qAbs(cross) < 1e-8) {
				if (dot < -1e-8) {
					if (qMin(a.x, b.x) - 1e-8 <= pt.x && pt.x <= qMax(a.x, b.x) + 1e-8 &&
						qMin(a.y, b.y) - 1e-8 <= pt.y && pt.y <= qMax(a.y, b.y) + 1e-8) {
						return -1;  // Point is on boundary extension (treated as on polygon)
					}
				}
				continue;
			}
			bool isCrossing = (a.y <= pt.y + 1e-8 && pt.y < b.y + 1e-8) ||
				(b.y <= pt.y + 1e-8 && pt.y < a.y + 1e-8);

			if (isCrossing) {
				double xIntersect = ((pt.y - a.y) * (b.x - a.x)) / (b.y - a.y) + a.x;

				if (pt.x < xIntersect - 1e-8) {
					if (b.y > a.y + 1e-8)
						windingNumber++;
					else
						windingNumber--;
				}
			}
		}

		// Determine final position: non-zero winding number means inside, zero means outside
		return (windingNumber != 0) ? 1 : 0;
	}

	
	McDbExtents getGeomExtents(const McDbObjectId& entId, bool roughCalculation)
	{
		McDbEntityPointer spEnt(entId, McDb::kForRead);
		if (spEnt.openStatus() != Mcad::eOk) return { };
		McDbExtents ext;
		spEnt->getGeomExtents(ext, roughCalculation);
		return ext;
	}
	
	McDbExtents getGeomExtents(const McDbEntity* pEnt, bool roughCalculation)
	{
		McDbExtents ext;
		pEnt->getGeomExtents(ext, roughCalculation);
		return ext;
	}

	QList<TextLine> getMTextContent(McDbMText* pMText)
	{
		McDbVoidPtrArray arr;
		pMText->explode(arr);
		QList<McDbText*> notProcessedTexts;
		for (int i = 0; i < arr.length(); ++i)
		{
			McDbText* pText = static_cast<McDbText*>(arr[i]);
			notProcessedTexts.append(pText);
		}
		QList<TextLine> resStr;
		while (!notProcessedTexts.isEmpty())
		{
			QList<McDbText*> sameLineTempList;
			QList<McDbText*> notSameLineTempList;

			McDbText* pFirstText = notProcessedTexts[0];

			sameLineTempList.append(pFirstText);

			for (int i = 1; i < notProcessedTexts.length(); ++i)
			{
				McDbText* pCurText = notProcessedTexts[i];
				if (isSameLineText(pFirstText, pCurText))
				{
					sameLineTempList.append(pCurText);
				}
				else
				{
					notSameLineTempList.append(pCurText);
				}
			}

			TextLine line = conbineOneLineText(sameLineTempList);
			resStr.append(line);
			notProcessedTexts = notSameLineTempList;
		}

		// sort
		std::sort(resStr.begin(), resStr.end(), [](TextLine a, TextLine b) {
			return a.pos.y > b.pos.y;
			});

		for (int i = 0; i < arr.length(); ++i)
		{
			delete arr[i];
		}
		arr.clear();
		return resStr;

	}
	QList<TextLine> findAllTextsInBlockRef(const McDbBlockReference* blockRef)
	{
		if (!blockRef) return {};
		McDbVoidPtrArray arr;
		blockRef->explode(arr);
		QList<TextLine> res;
		for (int i = 0; i < arr.length(); i++)
		{
			McDbEntity* pEnt = static_cast<McDbEntity*>(arr[i]);
			if (pEnt->isA() == McDbText::desc())
			{
				McDbText* pText = McDbText::cast(pEnt);
				QString str = QString::fromLocal8Bit(pText->textString());
				McGePoint3d pos = getGeomExtents(pText, false).minPoint();
				TextLine line = { pos, str };
				res.append(line);
			}
			else if (pEnt->isA() == McDbMText::desc())
			{
				McDbMText* pMText = McDbMText::cast(pEnt);
				auto texts = getMTextContent(pMText);
				res.append(texts);
			}
			delete pEnt;
		}

		return res;
	}
	QList<TextLine> findAllTextsInBlockRef(const McDbObjectId& blockRefId)
	{
		McDbEntityPointer spEnt(blockRefId, McDb::kForRead);
		if (spEnt.openStatus() != Mcad::eOk) return {};
		if (spEnt->isA() != McDbBlockReference::desc()) return {};
		McDbBlockReference* blockRef = McDbBlockReference::cast(spEnt.object());

		return findAllTextsInBlockRef(blockRefId);

	}

}
