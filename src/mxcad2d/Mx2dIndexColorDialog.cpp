/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dIndexColorDialog.h"

#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QMouseEvent>
#include <functional>

// -------------------------------------------------------------------------
// Internal Custom Widget for Color Blocks
// (Does not use Q_OBJECT to avoid MOC compilation requirements in .cpp files)
// -------------------------------------------------------------------------
class ColorBlockWidget : public QWidget {
public:
    using ClickCallback = std::function<void(int)>;

    ColorBlockWidget(const QColor& color, int index, ClickCallback callback, QWidget* parent = nullptr)
        : QWidget(parent), m_color(color), m_index(index), m_selected(false), m_callback(callback) 
    {
        // Set fixed size to match the provided image proportions
        setFixedSize(60, 30);
        // Change cursor to pointing hand when hovering over the color block
        setCursor(Qt::PointingHandCursor);
    }

    void setSelected(bool selected) {
        if (m_selected != selected) {
            m_selected = selected;
            update(); // Trigger repaint
        }
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        Q_UNUSED(event);
        QPainter painter(this);
        QRect r = rect();

        // 1. Fill the base color
        painter.fillRect(r, m_color);

        // 2. Draw selection border or normal border
        if (m_selected) {
            // Selected state: Dark outer border + White inner gap (matches image perfectly)
            painter.setPen(QPen(QColor(100, 100, 100), 1)); // Dark gray outer
            painter.drawRect(r.adjusted(0, 0, -1, -1));

            painter.setPen(QPen(Qt::white, 1)); // White inner gap
            painter.drawRect(r.adjusted(1, 1, -2, -2));
            painter.drawRect(r.adjusted(2, 2, -3, -3));
        } else {
            // Unselected state: Thin light gray border
            painter.setPen(QPen(QColor(200, 200, 200), 1));
            painter.drawRect(r.adjusted(0, 0, -1, -1));
        }
    }

    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton && m_callback) {
            m_callback(m_index);
        }
        QWidget::mousePressEvent(event);
    }

private:
    QColor m_color;
    int m_index;
    bool m_selected;
    ClickCallback m_callback;
};


// -------------------------------------------------------------------------
// Mx2dIndexColorDialog Implementation
// -------------------------------------------------------------------------

const QVector<QColor>& Mx2dIndexColorDialog::getDefaultColors()
{
	static QVector<QColor> colors = {
		QColor(183, 27, 11),   // 0: Dark Red
		QColor(121, 199, 57),  // 1: Light Green
		QColor(230, 81, 0),    // 2: Orange
		QColor(255, 255, 255), // 3: White
		QColor(80, 127, 185),  // 4: Steel Blue
		QColor(189, 11, 117),  // 5: Magenta
		QColor(255, 255, 0),   // 6: Yellow
		QColor(12, 70, 197),    // 7: Blue
		QColor(235, 154, 41),  // 8: Goldenrod
		QColor(255, 192, 203), // 9: Pink
		QColor(0, 255, 255),   // 10: Cyan
		QColor(255, 52, 179),  // 11: Deep Pink
		QColor(224, 102, 255), // 12: Violet
		QColor(255, 222, 173), // 13: Wheat/Peach
		QColor(255, 48, 48),   // 14: Red
		QColor(34, 139, 34),   // 15: Green
		QColor(254, 67, 101),   // 16: Pinkish Red
		QColor(173, 137, 118), // 17: Light Brown
		QColor(35, 235, 185),  // 18: Turquoise
		QColor(153, 204, 255)  // 19: Light Blue
	};
    return colors;
}

int Mx2dIndexColorDialog::getClosestColorIndex(const QColor& color)
{
	const QVector<QColor>& defaultColors = getDefaultColors();
	if (defaultColors.isEmpty()) {
		return -1;
	}

	int closestIndex = 0;
	qint64 minDistance = std::numeric_limits<qint64>::max();

	for (int i = 0; i < defaultColors.size(); ++i) {
		const QColor& defaultColor = defaultColors.at(i);

		qint64 deltaR = color.red() - defaultColor.red();
		qint64 deltaG = color.green() - defaultColor.green();
		qint64 deltaB = color.blue() - defaultColor.blue();
		qint64 distance = deltaR * deltaR + deltaG * deltaG + deltaB * deltaB;

		if (distance < minDistance) {
			minDistance = distance;
			closestIndex = i;
		}
	}
	return closestIndex;
}

Mx2dIndexColorDialog::Mx2dIndexColorDialog(QWidget *parent)
    : QDialog(parent), m_selectedIndex(-1)
{
    // Set dialog flags: no question mark, fixed size behavior
    Qt::WindowFlags flags = Qt::Dialog;
    flags |= Qt::CustomizeWindowHint;
    flags |= Qt::WindowTitleHint;
    flags |= Qt::WindowCloseButtonHint;
    setWindowFlags(flags);

    setWindowTitle(tr("Select Color"));
    
  
    initUI();

    // Ensure the dialog size cannot be changed by the user
    adjustSize();
    setFixedSize(size());
}



void Mx2dIndexColorDialog::initUI()
{
    // Main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // Header label
    QLabel* lblTip = new QLabel(tr("Please click one of the color blocks below to select a color"));
    lblTip->setStyleSheet("color: #333333; font-size: 13px;");
    mainLayout->addWidget(lblTip);

    // Grid layout for color blocks
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setContentsMargins(10, 10, 10, 10);
    gridLayout->setHorizontalSpacing(30); // Horizontal gap between blocks
    gridLayout->setVerticalSpacing(20);   // Vertical gap between blocks

    // Create and add color blocks
    int row = 0;
    int col = 0;
    for (int i = 0; i < getDefaultColors().size(); ++i) {
        // Pass a lambda callback to handle clicks inside the custom widget
        auto callback = [this](int index) { this->onColorBlockClicked(index); };
        
        ColorBlockWidget* block = new ColorBlockWidget(getDefaultColors()[i], i, callback, this);
        gridLayout->addWidget(block, row, col);
        m_blocks.append(block);

        // Advance to next column/row (4 columns total)
        col++;
        if (col >= 4) {
            col = 0;
            row++;
        }
    }
    mainLayout->addLayout(gridLayout);

    // Separator space
    mainLayout->addSpacing(10);

    // Buttons layout
    QHBoxLayout* btnLayout = new QHBoxLayout();
    m_btnOk = new QPushButton(tr("OK"));
    m_btnCancel = new QPushButton(tr("Cancel"));

    // Ensure buttons have uniform width matching the provided UI
    m_btnOk->setFixedWidth(90);
    m_btnCancel->setFixedWidth(90);
    
    // Optional: Make OK button look slightly emphasized
    m_btnOk->setStyleSheet("QPushButton { border: 1px solid #005bb5; background-color: white; padding: 5px; color: #005bb5; }"
                           "QPushButton:hover { background-color: #f0f8ff; }");
    m_btnCancel->setStyleSheet("QPushButton { border: 1px solid #d0d0d0; background-color: white; padding: 5px; }"
                               "QPushButton:hover { background-color: #f5f5f5; }");

    btnLayout->addStretch();
    btnLayout->addWidget(m_btnOk);
    btnLayout->addWidget(m_btnCancel);
    mainLayout->addLayout(btnLayout);

    // Connect standard dialog buttons
    connect(m_btnOk, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    
    // Default: disable OK button until a color is selected
    m_btnOk->setEnabled(false);
}

void Mx2dIndexColorDialog::onColorBlockClicked(int index)
{
    setSelectedIndex(index);
}

// -------------------------------------------------------------------------
// Public Interface API
// -------------------------------------------------------------------------

void Mx2dIndexColorDialog::setSelectedIndex(int index)
{
    if (index < 0 || index >= getDefaultColors().size()) return;

    m_selectedIndex = index;

    // Update the visual selection state of all blocks
    for (int i = 0; i < m_blocks.size(); ++i) {
        m_blocks[i]->setSelected(i == index);
    }

    // Enable the OK button once a valid selection is made
    m_btnOk->setEnabled(true);
}

int Mx2dIndexColorDialog::getSelectedIndex() const
{
    return m_selectedIndex;
}

QColor Mx2dIndexColorDialog::getSelectedColor() const
{
    if (m_selectedIndex >= 0 && m_selectedIndex < getDefaultColors().size()) {
        return getDefaultColors()[m_selectedIndex];
    }
    return QColor(); // Return invalid color if nothing is selected
}