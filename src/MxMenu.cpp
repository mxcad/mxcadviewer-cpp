/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "MxMenu.h"

MxMenu::MxMenu(QWidget* parent)
// Initialize as a popup widget with no window frame or shadow.
    : QWidget(parent, Qt::Popup | Qt::NoDropShadowWindowHint | Qt::FramelessWindowHint)
{
    setupUI();

    // Apply a custom stylesheet for the menu's appearance.
    setStyleSheet(R"(
        MxMenu {
            background: rgba(39, 40, 34, 0.5);
            padding: 5px 0;
        }
        QPushButton {
            color: white;
            background: transparent;
            border: none;
            padding: 12px 24px;
            min-width: 80px;
            text-align: left;
            font-size: 20px;
        }
        QPushButton:hover {
            background: rgba(39, 40, 34, 0.8);
        }
    )");

    // Enable translucent background to allow for custom shapes and semi-transparency.
    setAttribute(Qt::WA_TranslucentBackground);
}

void MxMenu::addItem(const QString& text)
{
    // Create a new button for the menu item.
    QPushButton* btn = new QPushButton(text, this);

    // When the button is clicked, emit the itemSelected signal and hide the menu.
    connect(btn, &QPushButton::clicked, this, [this, text]() {
        emit itemSelected(text);
        hide();
        });

    // Add the button to the layout.
    layout()->addWidget(btn);
}

void MxMenu::showAt(QWidget* parent)
{
    // Calculate the initial display position below the parent widget.
    QPoint globalPos = parent->mapToGlobal(QPoint(0, parent->height()));
    QRect screenRect = QGuiApplication::screenAt(globalPos)->availableGeometry();

    // Automatically adjust the display direction if it goes off the bottom of the screen.
    if (globalPos.y() + height() > screenRect.bottom()) {
        globalPos = parent->mapToGlobal(QPoint(0, -height()));
    }

    move(globalPos);
    adjustSize(); // Adjust widget size to fit its content.
    raise();      // Ensure the menu is on top of other windows.
    show();
    setFocus();   // Ensure the menu receives keyboard events.
}

void MxMenu::paintEvent(QPaintEvent* /*event*/)
{
    // Custom painting for rounded corners.
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(palette().window()); // Use the window brush color for the background.
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect().adjusted(0, 0, -1, -1), 5, 5);
}

void MxMenu::keyPressEvent(QKeyEvent* e)
{
    // Basic keyboard navigation support.
    if (e->key() == Qt::Key_Escape) {
        hide();
    }
}

void MxMenu::setupUI()
{
    // Set up the vertical layout for the menu items.
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(2, 5, 2, 5);
    layout->setSpacing(0);
}