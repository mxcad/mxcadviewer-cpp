/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once


#include <QWidget>
#include <QList>
#include <QRect>
#include <QPoint>
#include <utility>

class QMouseEvent;
class QPaintEvent;

struct SelectableRect
{
	QRect rect;
	bool isSelected = false;
};

class Mx2dFrameRegionWidget : public QWidget
{
	Q_OBJECT

public:
	explicit Mx2dFrameRegionWidget(QWidget* parent = nullptr);

public slots:
	void startSelectionMode();
	void clearAll();

protected:
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void paintEvent(QPaintEvent* event) override;

private:
	enum class HoverLocation {
		None,
		Inside,
		Top, Bottom, Left, Right,
		TopLeft, TopRight, BottomLeft, BottomRight
	};

	std::pair<int, HoverLocation> findHoveredRect(const QPoint& point);
	void updateCursorShape(HoverLocation location);
	void resizeRect(const QPoint& mousePos);
	void moveRect(const QPoint& mousePos);
	void drawResizeHandles(QPainter& painter, const QRect& rect);

	// drawing state
	bool m_isDrawingMode;
	bool m_waitingForSecondClick;
	QPoint m_firstPoint;

	// interaction state
	bool m_isResizing;
	bool m_isMoving;
	int m_interactionRectIndex;
	HoverLocation m_resizeHandle;
	QPoint m_dragStartPoint;

	// hover state
	int m_hoveredRectIndex;
	HoverLocation m_hoverLocation;


	QList<SelectableRect> m_rectangles;
};
