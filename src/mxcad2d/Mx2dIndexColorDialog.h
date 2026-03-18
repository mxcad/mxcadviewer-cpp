/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QDialog>
#include <QColor>
#include <QVector>

class QPushButton;

// Forward declaration of the internal widget class
class ColorBlockWidget; 

class Mx2dIndexColorDialog : public QDialog
{
    Q_OBJECT

public:
    static const QVector<QColor>& getDefaultColors();

    static int getClosestColorIndex(const QColor& color);

    explicit Mx2dIndexColorDialog(QWidget *parent = nullptr);
    ~Mx2dIndexColorDialog() override = default;

    // Interface to set the current selected color block by index (0 to 19)
    void setSelectedIndex(int index);

    // Interface to get the currently selected index
    int getSelectedIndex() const;

    // Interface to get the QColor object of the currently selected block
    QColor getSelectedColor() const;

private:
    void initUI();
    // Internal callback triggered when a color block is clicked
    void onColorBlockClicked(int index);

private:
    int m_selectedIndex;
    QVector<QColor> m_colors;
    QVector<ColorBlockWidget*> m_blocks;

    QPushButton* m_btnOk;
    QPushButton* m_btnCancel;
};