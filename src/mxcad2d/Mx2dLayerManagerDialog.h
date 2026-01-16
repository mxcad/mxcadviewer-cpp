/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QDialog>
#include <QList>
#include <QMap>
#include "Mx2dUtils.h"

// Forward declarations
QT_BEGIN_NAMESPACE
class QTableWidget;
class QPushButton;
class QLabel;
class QKeyEvent;
class Mx2dGuiDocument;
QT_END_NAMESPACE

/**
 * @class Mx2dLayerManagerDialog
 * @brief A non-modal dialog for managing layers of 2D CAD drawings.
 *
 * This dialog displays a list of layers for the active drawing, allowing the user
 * to toggle their visibility. It supports multiple open documents and can restore
 * the initial layer state for each.
 */
class Mx2dLayerManagerDialog : public QDialog
{
    Q_OBJECT

public:
    Mx2dLayerManagerDialog(Mx2dGuiDocument* guiDoc, QWidget* parent = nullptr);
    void updateLayerTable(const Mx2d::LayerInfo& info);

protected:
    // We keep keyPressEvent to prevent closing with the Escape key.
    void keyPressEvent(QKeyEvent* event) override;

private:
    void setupUi();
    void setButtonToolTips();
    void connectSignals();
    

    void setSomeLayersOffStatus(const Mx2d::LayerInfo& info);
    void setAllLayersOffStatus(bool isOff);
    void restoreInitialLayerState();

private slots:
    void onSwitchCellClicked(int row, int column);

private:
    // --- UI Member Widgets ---
    QTableWidget* m_layerTable;
    QPushButton* m_btnRestoreInit;
    QPushButton* m_btnOpenAll;
    QPushButton* m_btnCloseAll;
    QPushButton* m_btnCloseSelected;
    QPushButton* m_btnKeepSelected;
    QLabel* m_statusLabel;

    Mx2dGuiDocument* m_guiDoc;
private:
    Mx2d::LayerInfo m_originLayerInfo;
    Mx2d::LayerInfo m_currentLayerInfo;
private:
	Mx2d::LayerInfo getAllLayersInfo();
};