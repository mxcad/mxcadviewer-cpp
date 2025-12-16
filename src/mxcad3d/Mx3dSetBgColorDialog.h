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

QT_BEGIN_NAMESPACE
class QWidget;
class QComboBox;
class QTabWidget;
QT_END_NAMESPACE

/**
 * @class Mx3dSetBgColorDialog
 * @brief A dialog for setting the background color of the 3D view.
 *
 * This dialog allows the user to choose between a solid background color
 * and a gradient background. It loads and saves the user's preferences.
 */
class Mx3dSetBgColorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit Mx3dSetBgColorDialog(QWidget* parent = nullptr);
    ~Mx3dSetBgColorDialog() override;

private:
    void setupUi();
    QWidget* createSolidColorTab();
    QWidget* createGradientColorTab();
    
    void loadInitialConfig();
    void connectSignals();
    void updateColorPreview(QWidget* previewWidget, const QColor& color);

private slots:
    void onSelectSolidBgColor();
    void onSelectGradientBgColor1();
    void onSelectGradientBgColor2();
    void onSetGradientMode(int index);
    void onRestoreSolidBgDefault();
    void onRestoreGradientBgDefault();
    void onTabChanged(int index);

private:
    QTabWidget* m_tabWidget;
    QWidget*    m_solidColorPreview;
    QWidget*    m_gradientColor1Preview;
    QWidget*    m_gradientColor2Preview;
    QComboBox*  m_gradientModeComboBox;

    QColor m_solidBgColor;
    QColor m_gradientBgColor1;
    QColor m_gradientBgColor2;
    int    m_gradientMode;
};