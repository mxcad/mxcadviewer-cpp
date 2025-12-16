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
    explicit Mx2dLayerManagerDialog(QWidget* parent = nullptr);

protected:
    // We keep keyPressEvent to prevent closing with the Escape key.
    void keyPressEvent(QKeyEvent* event) override;

private:
    void setupUi();
    void setButtonToolTips();
    void connectSignals();
    void updateLayerTable(const Mx2d::Mx2dLayerInfo& info);

    // Original logic for interacting with the CAD engine
    static resbuf* makeLayersStatusResbuf(const QList<McDbObjectId>& ids, const QList<bool>& offStatus);
    void setSomeLayersOffStatus(QWidget* tab, const QList<McDbObjectId>& ids, const QList<bool>& offStatus);
    void setAllLayersOffStatus(QWidget* tab, bool isOff);
    void restoreInitialLayerState(QWidget* tab);

private slots:
    // Slot to receive layer information from the CAD document
    void onLayerInformationReceived(const Mx2d::Mx2dLayerInfo& info);
    // Slot to handle a layer's visibility toggle from the table
    void onLayerVisibilityToggled(int row, int column);

private:
    // --- UI Member Widgets ---
    QTableWidget* m_layerTable;
    QPushButton* m_btnRestoreInit;
    QPushButton* m_btnOpenAll;
    QPushButton* m_btnCloseAll;
    QPushButton* m_btnCloseSelected;
    QPushButton* m_btnKeepSelected;
    QLabel* m_statusLabel;

    // Stores the initial layer information for each document tab to allow for restoration.
    QMap<QWidget*, Mx2d::Mx2dLayerInfo> m_originLayerInfoMap;
    // Stores the current layer information for each document tab.
    QMap<QWidget*, Mx2d::Mx2dLayerInfo> m_currentLayerInfoMap;
};