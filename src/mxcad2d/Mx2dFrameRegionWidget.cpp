/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dFrameRegionWidget.h"
#include <QPainter>
#include <QCursor>
#include <QMouseEvent>

constexpr int HANDLE_SIZE = 8; // Control handle size

Mx2dFrameRegionWidget::Mx2dFrameRegionWidget(QWidget* parent)
	: QWidget(parent),
	m_isDrawingMode(false),
	m_waitingForSecondClick(false),
	m_isResizing(false),
	m_isMoving(false),
	m_interactionRectIndex(-1),
	m_resizeHandle(HoverLocation::None),
	m_hoveredRectIndex(-1),
	m_hoverLocation(HoverLocation::None)
{
	setAutoFillBackground(true);
	QPalette pal = palette();
	pal.setColor(QPalette::Window, Qt::white);
	setPalette(pal);

	setMinimumSize(600, 400);
	setMouseTracking(true);
}

// Enter drawing mode
void Mx2dFrameRegionWidget::startSelectionMode()
{
	m_isDrawingMode = true;
	m_waitingForSecondClick = false; // Initial state, haven't clicked first point yet
	// Reset other mode states
	m_isResizing = false;
	m_isMoving = false;
	setCursor(Qt::CrossCursor);
	update();
}

// Clear all rectangles
void Mx2dFrameRegionWidget::clearAll()
{
	m_rectangles.clear();
	m_hoveredRectIndex = -1;
	m_hoverLocation = HoverLocation::None;
	// Reset all states
	m_isDrawingMode = false;
	m_waitingForSecondClick = false;
	m_isResizing = false;
	m_isMoving = false;
	setCursor(Qt::ArrowCursor);
	update();
}

// Handle "start" action for all modes
void Mx2dFrameRegionWidget::mousePressEvent(QMouseEvent* event)
{
	if (event->button() != Qt::LeftButton) return;

	if (m_isDrawingMode) {
		// --- Drawing mode logic ---
		if (!m_waitingForSecondClick) {
			m_firstPoint = event->pos();
			m_waitingForSecondClick = true;
		}
		else {
			QRect newRect = QRect(m_firstPoint, event->pos()).normalized();
			if (newRect.isValid()) {
				m_rectangles.append({ newRect, true });
			}
			m_isDrawingMode = false;
			m_waitingForSecondClick = false;
			setCursor(Qt::ArrowCursor);
		}
	}
	else {
		// --- Interaction mode logic ---
		auto hoverInfo = findHoveredRect(event->pos());
		int clickedIndex = hoverInfo.first;

		if (clickedIndex != -1) {
			// --- Case A: Click occurred on a rectangle ---

			// Step 1: Ensure the clicked rectangle is selected
			if (!m_rectangles[clickedIndex].isSelected) {
				// If it's not currently selected, make it the only selected object
				for (int i = 0; i < m_rectangles.size(); ++i) {
					m_rectangles[i].isSelected = (i == clickedIndex);
				}
			}
			// If it's already selected, keep its selected state (leaving room for multi-select functionality)

			// Step 2: Prepare to enter move or resize state based on click position
			m_interactionRectIndex = clickedIndex;
			m_dragStartPoint = event->pos();

			if (hoverInfo.second == HoverLocation::Inside) {
				m_isMoving = true; // Prepare to move
			}
			else if (hoverInfo.second != HoverLocation::None) {
				m_isResizing = true; // Prepare to resize
				m_resizeHandle = hoverInfo.second;
			}

		}
		else {
			// --- Case B: Click occurred on blank area ---
			// Deselect all rectangles
			for (auto& sRect : m_rectangles) {
				sRect.isSelected = false;
			}
		}
	}
	update(); // Trigger repaint to reflect state changes
}

// Mouse move event: Handle "dragging" process for all modes
void Mx2dFrameRegionWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (m_isMoving) {
		// --- Currently moving rectangle ---
		moveRect(event->pos());
	}
	else if (m_isResizing) {
		// --- Currently resizing ---
		resizeRect(event->pos());
	}
	else if (m_isDrawingMode && m_waitingForSecondClick) {
		// --- Currently drawing, real-time preview ---
		update(); // Just need to trigger repaint, paintEvent will handle preview
	}
	else {
		// --- Default interaction mode, detect hovering ---
		auto hoverInfo = findHoveredRect(event->pos());
		if (hoverInfo.first != m_hoveredRectIndex || hoverInfo.second != m_hoverLocation) {
			m_hoveredRectIndex = hoverInfo.first;
			m_hoverLocation = hoverInfo.second;
			updateCursorShape(m_hoverLocation);
			update();
		}
	}
}

// Mouse release event: Handle "end" action for all modes
void Mx2dFrameRegionWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() != Qt::LeftButton) return;

	if (m_isMoving) {
		// --- End moving ---
		m_isMoving = false;
		m_interactionRectIndex = -1;
	}
	else if (m_isResizing) {
		// --- End resizing ---
		m_isResizing = false;
		// Ensure rectangle dimensions are valid
		if (m_interactionRectIndex != -1) {
			m_rectangles[m_interactionRectIndex].rect = m_rectangles[m_interactionRectIndex].rect.normalized();
		}
		m_interactionRectIndex = -1;
	}
	// After mouse release, re-detect hover position and cursor shape
	mouseMoveEvent(event);
}

// Paint event
void Mx2dFrameRegionWidget::paintEvent(QPaintEvent* event)
{
	QWidget::paintEvent(event);
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	// 1. Draw all existing rectangles
	for (int i = 0; i < m_rectangles.size(); ++i) {
		const auto& sRect = m_rectangles[i];
		QColor fillColor(0, 150, 0, 50);
		QPen pen(QColor("#222222"), 1);

		if (sRect.isSelected) {
			fillColor = QColor(0, 80, 200, 100);
			pen.setColor(QColor("#0050C8"));
			pen.setWidth(2);
		}

		// Highlight on hover in non-interactive state
		if (i == m_hoveredRectIndex && !m_isMoving && !m_isResizing) {
			pen.setColor(QColor("#00AEEF"));
			pen.setWidth(2);
		}

		painter.setPen(pen);
		painter.setBrush(fillColor);
		painter.drawRect(sRect.rect);

		if (sRect.isSelected) {
			drawResizeHandles(painter, sRect.rect);
		}
	}

	// 2. If currently drawing, draw preview rectangle
	if (m_isDrawingMode && m_waitingForSecondClick) {
		QPen previewPen(QColor("#222222"), 1, Qt::DashLine);
		painter.setPen(previewPen);
		painter.setBrush(Qt::NoBrush); // No fill during preview
		painter.drawRect(QRect(m_firstPoint, this->mapFromGlobal(QCursor::pos())).normalized());
	}
}

// --- Helper functions ---

// Find rectangle under mouse and its specific location
std::pair<int, Mx2dFrameRegionWidget::HoverLocation> Mx2dFrameRegionWidget::findHoveredRect(const QPoint& point)
{
	int insideIndex = -1; // Used to record the topmost "inside" hover

	// Traverse from top rectangle backwards (handle overlapping cases)
	for (int i = m_rectangles.size() - 1; i >= 0; --i) {
		const auto& sRect = m_rectangles[i];
		const QRect& r = sRect.rect;

		// Only detect handles for selected rectangles
		if (sRect.isSelected) {
			QRect topLeft(r.topLeft(), QSize(HANDLE_SIZE, HANDLE_SIZE));
			QRect topRight(r.topRight() - QPoint(HANDLE_SIZE - 1, 0), QSize(HANDLE_SIZE, HANDLE_SIZE));
			QRect bottomLeft(r.bottomLeft() - QPoint(0, HANDLE_SIZE - 1), QSize(HANDLE_SIZE, HANDLE_SIZE));
			QRect bottomRight(r.bottomRight() - QPoint(HANDLE_SIZE - 1, HANDLE_SIZE - 1), QSize(HANDLE_SIZE, HANDLE_SIZE));
			QRect topEdge(r.left() + HANDLE_SIZE, r.top(), r.width() - 2 * HANDLE_SIZE, HANDLE_SIZE);
			QRect bottomEdge(r.left() + HANDLE_SIZE, r.bottom() - HANDLE_SIZE + 1, r.width() - 2 * HANDLE_SIZE, HANDLE_SIZE);
			QRect leftEdge(r.left(), r.top() + HANDLE_SIZE, HANDLE_SIZE, r.height() - 2 * HANDLE_SIZE);
			QRect rightEdge(r.right() - HANDLE_SIZE + 1, r.top() + HANDLE_SIZE, HANDLE_SIZE, r.height() - 2 * HANDLE_SIZE);

			// If mouse is on any handle, return immediately (handles have highest priority)
			if (topLeft.contains(point)) return { i, HoverLocation::TopLeft };
			if (topRight.contains(point)) return { i, HoverLocation::TopRight };
			if (bottomLeft.contains(point)) return { i, HoverLocation::BottomLeft };
			if (bottomRight.contains(point)) return { i, HoverLocation::BottomRight };
			if (topEdge.contains(point)) return { i, HoverLocation::Top };
			if (bottomEdge.contains(point)) return { i, HoverLocation::Bottom };
			if (leftEdge.contains(point)) return { i, HoverLocation::Left };
			if (rightEdge.contains(point)) return { i, HoverLocation::Right };
		}

		// Check if mouse is inside regardless of selection
		if (r.contains(point)) {
			// Don't return immediately, there might be other rectangle handles on top
			// Record this as the topmost "inside" hit
			if (insideIndex == -1) {
				insideIndex = i;
			}
		}
	}

	// After loop, if found an "inside" hit and no higher priority handle hit, return it
	if (insideIndex != -1) {
		return { insideIndex, HoverLocation::Inside };
	}

	// If nothing found
	return { -1, HoverLocation::None };
}

// Update cursor shape
void Mx2dFrameRegionWidget::updateCursorShape(HoverLocation location) {
	switch (location) {
	case HoverLocation::Inside:
		setCursor(Qt::SizeAllCursor); // Cross move cursor
		break;
	case HoverLocation::TopLeft: case HoverLocation::BottomRight:
		setCursor(Qt::SizeFDiagCursor); break;
	case HoverLocation::TopRight: case HoverLocation::BottomLeft:
		setCursor(Qt::SizeBDiagCursor); break;
	case HoverLocation::Top: case HoverLocation::Bottom:
		setCursor(Qt::SizeVerCursor); break;
	case HoverLocation::Left: case HoverLocation::Right:
		setCursor(Qt::SizeHorCursor); break;
	default:
		setCursor(Qt::ArrowCursor); break;
	}
}

// Move rectangle
void Mx2dFrameRegionWidget::moveRect(const QPoint& mousePos) {
	if (m_interactionRectIndex == -1) return;
	QPoint delta = mousePos - m_dragStartPoint; // Calculate displacement
	m_rectangles[m_interactionRectIndex].rect.translate(delta);
	m_dragStartPoint = mousePos; // Update drag start point for next move calculation
	update();
}

// Resize rectangle
void Mx2dFrameRegionWidget::resizeRect(const QPoint& mousePos) {
	if (m_interactionRectIndex == -1) return;
	QRect& r = m_rectangles[m_interactionRectIndex].rect;
	switch (m_resizeHandle) {
	case HoverLocation::TopLeft:     r.setTopLeft(mousePos); break;
	case HoverLocation::TopRight:    r.setTopRight(mousePos); break;
	case HoverLocation::BottomLeft:  r.setBottomLeft(mousePos); break;
	case HoverLocation::BottomRight: r.setBottomRight(mousePos); break;
	case HoverLocation::Top:         r.setTop(mousePos.y()); break;
	case HoverLocation::Bottom:      r.setBottom(mousePos.y()); break;
	case HoverLocation::Left:        r.setLeft(mousePos.x()); break;
	case HoverLocation::Right:       r.setRight(mousePos.x()); break;
	default: break;
	}
	update();
}

// Draw control handles
void Mx2dFrameRegionWidget::drawResizeHandles(QPainter& painter, const QRect& rect) {
	painter.setPen(Qt::black);
	painter.setBrush(Qt::white);
	QPoint tl = rect.topLeft();
	QPoint tr = rect.topRight();
	QPoint bl = rect.bottomLeft();
	QPoint br = rect.bottomRight();
	QPoint tc(rect.center().x(), rect.top());
	QPoint bc(rect.center().x(), rect.bottom());
	QPoint cl(rect.left(), rect.center().y());
	QPoint cr(rect.right(), rect.center().y());
	QPoint points[] = { tl, tr, bl, br, tc, bc, cl, cr };
	for (const auto& p : points) {
		QRect handleRect(p.x() - HANDLE_SIZE / 2, p.y() - HANDLE_SIZE / 2, HANDLE_SIZE, HANDLE_SIZE);
		painter.drawRect(handleRect);
	}
}