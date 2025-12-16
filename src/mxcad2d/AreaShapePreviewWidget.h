/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QWidget>
#include <QVector>
#include <QPainterPath>
#include "Mx2dUtils.h"

struct EdgeSegment
{
	int id;
	QPainterPath path;
	qreal length;
	bool selected;
	QPointF labelPosition;
	double height;
};

class AreaShapePreviewWidget : public QWidget
{
	Q_OBJECT

public:
	explicit AreaShapePreviewWidget(QWidget* parent = nullptr);

	void setPolylineData(const Mx2d::PLVertexList& vertices);

	qreal getTotalSelectedLength() const;
	const QVector<EdgeSegment>& getSegments() const { return m_segments; }
public:
	void fitToView();
signals:
	void selectionChanged();

protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;

private:
	void processPolylineData(const Mx2d::PLVertexList& vertices);

	QPointF mapToWorld(const QPointF& widgetPoint) const;
	QPointF mapFromWorld(const QPointF& worldPoint) const;

	QVector<EdgeSegment> m_segments;

	qreal m_scale;
	QPointF m_panOffset;

	bool m_isPanning;
	QPointF m_lastMousePos;
};