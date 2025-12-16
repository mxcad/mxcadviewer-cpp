/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QApplication>
#include <QPainter>
#include <QKeyEvent>
#include <QScreen>

class MxMenu : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a custom menu widget.
     * @param parent The parent widget.
     */
    explicit MxMenu(QWidget* parent = nullptr);

    /**
     * @brief Adds a new item to the menu.
     * @param text The text to display for the item.
     */
    void addItem(const QString& text);

    /**
     * @brief Shows the menu positioned relative to a parent widget.
     * @param parent The widget to anchor the menu to.
     */
    void showAt(QWidget* parent);

protected:
    /**
     * @brief Overridden to provide custom painting for rounded corners.
     */
    void paintEvent(QPaintEvent*) override;

    /**
     * @brief Overridden to handle keyboard events, like closing on Escape.
     */
    void keyPressEvent(QKeyEvent* e) override;

private:
    /**
     * @brief Sets up the initial UI layout for the menu.
     */
    void setupUI();

signals:
    /**
     * @brief Emitted when a menu item is clicked.
     * @param text The text of the selected item.
     */
    void itemSelected(const QString& text);
};