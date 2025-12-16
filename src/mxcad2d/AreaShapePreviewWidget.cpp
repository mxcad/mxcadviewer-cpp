/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "AreaShapePreviewWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPainterPathStroker>
#include <QtMath>
#include <QDebug>

AreaShapePreviewWidget::AreaShapePreviewWidget(QWidget* parent)
	: QWidget(parent)
	, m_scale(1.0)
	, m_panOffset(0, 0)
	, m_isPanning(false)
{
	setAutoFillBackground(true);
	QPalette pal = palette();
	pal.setColor(QPalette::Window, QColor(0x2d, 0x31, 0x3b));
	setPalette(pal);

	setMinimumSize(400, 400);
}

void AreaShapePreviewWidget::setPolylineData(const Mx2d::PLVertexList& vertices)
{
	processPolylineData(vertices);
	fitToView();
	update();
}

void AreaShapePreviewWidget::processPolylineData(const Mx2d::PLVertexList& vertices)
{
	m_segments.clear();
	if (vertices.size() <= 2) {
		return;
	}

	for (int i = 0; i < vertices.size(); ++i)
	{
		const Mx2d::PLVertex& startVertex = vertices[i];
		// last vertex to first
		const Mx2d::PLVertex& endVertex = vertices[(i + 1) % vertices.size()];

		const QPointF p1(startVertex.pt.x, -startVertex.pt.y);
		const double bulge = startVertex.bulge;
		const QPointF p2(endVertex.pt.x, -endVertex.pt.y);

		QPainterPath segmentPath;
		qreal segmentLength = 0;
		QPointF labelPos;
		segmentPath.moveTo(p1);

		if (qFuzzyCompare(bulge, 0.0)) // line segment
		{
			segmentPath.lineTo(p2);
			segmentLength = QLineF(p1, p2).length();
			labelPos = (p1 + p2) / 2.0;
		}
		else // arc segment
		{
			double dDX = startVertex.pt.distanceTo(endVertex.pt) / 2.0;
			double dDY = qFabs(bulge) * dDX;
			double dR = ((dDX * dDX) + (dDY * dDY)) / (2.0 * dDY);

			McGeVector3d dir = McGeVector3d(endVertex.pt - startVertex.pt).rotateBy(M_PI / 2.0, McGeVector3d::kZAxis).normal();
			if (bulge > 0) // 
			{
				dir = -dir;
			}
			McGePoint3d midCoordPt((startVertex.pt.x + endVertex.pt.x) / 2.0, (startVertex.pt.y + endVertex.pt.y) / 2.0, 0);
			McGePoint3d center = midCoordPt + dir * (dDY - dR);
			McGeVector3d vecStart(startVertex.pt - center);
			McGeVector3d vecEnd(endVertex.pt - center);
			McGeVector3d vecArcMid = vecStart;


			double sweepLength = vecStart.angleTo(vecEnd);
			if (dDY > dR) // >PI arc
			{
				sweepLength = M_PI * 2.0 - sweepLength;
			}

			McGeVector3d qtVecStart = vecStart;
			double arcMidRot = sweepLength / 2.0;
			if (bulge < 0) // clockwise
			{
				qtVecStart = vecEnd;
				arcMidRot = -arcMidRot;
			}
			vecArcMid.rotateBy(arcMidRot, McGeVector3d::kZAxis);
			McGePoint3d arcMidPt = center + vecArcMid;
			double startAngle = qtVecStart.angleTo(McGeVector3d::kXAxis);
			if (qtVecStart.crossProduct(McGeVector3d::kXAxis).z > 0)
			{
				startAngle = M_PI * 2.0 - startAngle;
			}
			// reverse y 
			center = { center.x, -center.y , 0.0 };

			QRectF boundingRect(center.x - dR, center.y - dR, 2 * dR, 2 * dR);
			if (bulge < 0)
			{
				segmentPath.moveTo(p2);
			}
			segmentLength = qAbs(dR * sweepLength);
			startAngle = startAngle * 180 / M_PI;
			sweepLength = sweepLength * 180 / M_PI;
			segmentPath.arcTo(boundingRect, startAngle, sweepLength);


			labelPos = QPointF(arcMidPt.x, -arcMidPt.y);
		}

		m_segments.append({ i + 1, segmentPath, segmentLength, true, labelPos, 0.0 });
	}
}

void AreaShapePreviewWidget::fitToView()
{
	if (m_segments.isEmpty()) return;

	QPainterPath fullPath;
	for (const auto& segment : m_segments) {
		fullPath.addPath(segment.path);
	}
	QRectF bounds = fullPath.boundingRect();

	if (bounds.isEmpty() || bounds.width() == 0 || bounds.height() == 0) return;

	qreal scaleX = width() / bounds.width();
	qreal scaleY = height() / bounds.height();
	m_scale = qMin(scaleX, scaleY) * 0.9;

	QPointF center = bounds.center();
	m_panOffset = QPointF(width() / 2.0, height() / 2.0) - m_scale * center;

	update();
}

qreal AreaShapePreviewWidget::getTotalSelectedLength() const
{
	qreal totalLength = 0.0;
	for (const auto& segment : m_segments) {
		if (segment.selected) {
			totalLength += segment.length;
		}
	}
	return totalLength;
}

void AreaShapePreviewWidget::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event);
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	painter.translate(m_panOffset);
	painter.scale(m_scale, m_scale);

	for (const auto& segment : m_segments) {
		QColor color = segment.selected ? Qt::red : Qt::gray;
		painter.setPen(QPen(color, 1.5 / m_scale));
		painter.drawPath(segment.path);

		painter.setPen(Qt::red);
		QFont font("Arial", 12);
		font.setPointSizeF(12 / m_scale);
		painter.setFont(font);


		QVector2D dir = QVector2D(segment.labelPosition - segment.path.pointAtPercent(0.5)).normalized();
		QPointF textOffset = QPointF(dir.x(), dir.y()) * (10 / m_scale);
		painter.drawText(segment.labelPosition + textOffset, QString::number(segment.id));
	}
}

void AreaShapePreviewWidget::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton || event->button() == Qt::MiddleButton) {
		m_isPanning = true;
		m_lastMousePos = event->pos();
		setCursor(Qt::ClosedHandCursor);
	}

	if (event->button() == Qt::LeftButton) {
		bool changed = false;
		QPointF worldPos = mapToWorld(event->pos());

		for (auto& segment : m_segments) {
			QPainterPathStroker stroker;
			stroker.setWidth(15 / m_scale);
			QPainterPath strokePath = stroker.createStroke(segment.path);

			if (strokePath.contains(worldPos)) {
				segment.selected = !segment.selected;
				changed = true;
				m_isPanning = false;
				setCursor(Qt::ArrowCursor);
				break;
			}
		}

		if (changed) {
			update();
			emit selectionChanged();
		}
	}
	QWidget::mousePressEvent(event);
}

void AreaShapePreviewWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (m_isPanning) {
		QPointF delta = event->pos() - m_lastMousePos;
		m_panOffset += delta;
		m_lastMousePos = event->pos();
		update();
	}
	QWidget::mouseMoveEvent(event);
}

void AreaShapePreviewWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if ((event->button() == Qt::LeftButton || event->button() == Qt::MiddleButton) && m_isPanning) {
		m_isPanning = false;
		setCursor(Qt::ArrowCursor);
	}
	QWidget::mouseReleaseEvent(event);
}

void AreaShapePreviewWidget::wheelEvent(QWheelEvent* event)
{
	qreal zoomFactor = 1.15;
	qreal oldScale = m_scale;

	if (event->angleDelta().y() > 0) {
		m_scale *= zoomFactor;
	}
	else {
		m_scale /= zoomFactor;
	}

	QPointF mousePos = event->position();
	m_panOffset = mousePos - (mousePos - m_panOffset) * (m_scale / oldScale);

	update();
}

void AreaShapePreviewWidget::resizeEvent(QResizeEvent* event)
{
	fitToView();
	QWidget::resizeEvent(event);
}

QPointF AreaShapePreviewWidget::mapToWorld(const QPointF& widgetPoint) const {
	return (widgetPoint - m_panOffset) / m_scale;
}

QPointF AreaShapePreviewWidget::mapFromWorld(const QPointF& worldPoint) const {
	return (worldPoint * m_scale) + m_panOffset;
}