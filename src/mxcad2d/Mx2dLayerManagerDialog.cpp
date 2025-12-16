/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dLayerManagerDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QPushButton>
#include <QHeaderView>
#include <QLabel>
#include <QPainter>
#include <QStatusBar>
#include <QIcon>
#include <QKeyEvent>
#include <QDebug>
#include "Mx2dSignalTransfer.h"
#include "MxUtils.h"

Mx2dLayerManagerDialog::Mx2dLayerManagerDialog(QWidget* parent)
    : QDialog(parent)
{
    // Set window flags for a standard, non-resizable dialog with a close button
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);

    setupUi();
    setButtonToolTips();
    connectSignals();

    setWindowTitle(tr("Layer Manager"));
    adjustSize();
}

void Mx2dLayerManagerDialog::keyPressEvent(QKeyEvent* event)
{
    // Prevent the dialog from closing when the Escape key is pressed
    if (event->key() == Qt::Key_Escape) {
        event->ignore();
    }
    else {
        QDialog::keyPressEvent(event);
    }
}

void Mx2dLayerManagerDialog::setupUi()
{
    // --- Create Buttons ---
    m_btnRestoreInit = new QPushButton(QIcon(":/resources/images2d/2d_layerRestore.svg"), tr("Restore Initial"));
    m_btnOpenAll = new QPushButton(QIcon(":/resources/images2d/2d_layerAllOn.svg"), tr("All Layers On"));
    m_btnCloseAll = new QPushButton(QIcon(":/resources/images2d/2d_layerAllOff.svg"), tr("All Layers Off"));
    m_btnCloseSelected = new QPushButton(QIcon(":/resources/images2d/2d_layerSelectedOff.svg"), tr("Turn Off Selected Layer"));
    m_btnKeepSelected = new QPushButton(QIcon(":/resources/images2d/2d_layerOnlySelectedOn.svg"), tr("Isolate Selected Layer"));

    // --- Create Table ---
    m_layerTable = new QTableWidget(this);
    m_layerTable->setColumnCount(3);
    m_layerTable->setHorizontalHeaderLabels({ tr("On/Off"), tr("Color"), tr("Name") });
    m_layerTable->verticalHeader()->setVisible(false);
    m_layerTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_layerTable->setSelectionMode(QAbstractItemView::NoSelection);
    m_layerTable->horizontalHeader()->setStretchLastSection(true);
    m_layerTable->setColumnWidth(0, 80);
    m_layerTable->setColumnWidth(1, 80);

    // --- Create Layouts ---
    auto* btnLayout = new QHBoxLayout;
    btnLayout->addWidget(m_btnRestoreInit);
    btnLayout->addSpacing(10);
    btnLayout->addWidget(m_btnOpenAll);
    btnLayout->addWidget(m_btnCloseAll);
    btnLayout->addSpacing(10);
    btnLayout->addWidget(m_btnCloseSelected);
    btnLayout->addWidget(m_btnKeepSelected);
    btnLayout->addStretch();

    // --- Create Status Bar ---
    auto* statusBar = new QStatusBar(this);
    m_statusLabel = new QLabel(tr("Click buttons or the lightbulb icon in the list to toggle layers."), this);
    statusBar->addWidget(m_statusLabel);
    statusBar->setStyleSheet("QStatusBar { border: 0; }"); // Remove border for a cleaner look

    // --- Main Layout ---
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(btnLayout);
    mainLayout->addWidget(m_layerTable);
    mainLayout->addWidget(statusBar);
    mainLayout->setContentsMargins(10, 10, 10, 10);
}

void Mx2dLayerManagerDialog::setButtonToolTips()
{
    // Use QToolTip for hover-tips, which is simpler than event filters.
    m_btnRestoreInit->setToolTip(tr("Restore layer visibility to the state when the drawing was first opened."));
    m_btnOpenAll->setToolTip(tr("Turn on all layers in the drawing."));
    m_btnCloseAll->setToolTip(tr("Turn off all layers in the drawing."));
    m_btnCloseSelected->setToolTip(tr("Select one or more entities to turn off their layers."));
    m_btnKeepSelected->setToolTip(tr("Select one or more entities to keep their layers on and turn off all others."));
}

void Mx2dLayerManagerDialog::connectSignals()
{
    // Connect to global signals for layer information updates
    connect(&Mx2dSignalTransfer::getInstance(), &Mx2dSignalTransfer::signalLayerInformation, this, &Mx2dLayerManagerDialog::onLayerInformationReceived);
    connect(&Mx2dSignalTransfer::getInstance(), &Mx2dSignalTransfer::signalLayersOffStatusSetted, this, [](QWidget* tab) {
        // Refresh layer information after a batch operation
        Mx2d::execCmd2d(tab, "Mx_GetAllLayer");
        });

    // Connect button clicks to CAD commands
    connect(m_btnRestoreInit, &QPushButton::clicked, this, [this]() {
        restoreInitialLayerState(MxUtils::gCurrentTab);
        });
    connect(m_btnOpenAll, &QPushButton::clicked, this, [this]() {
        setAllLayersOffStatus(MxUtils::gCurrentTab, false);
        });
    connect(m_btnCloseAll, &QPushButton::clicked, this, [this]() {
        setAllLayersOffStatus(MxUtils::gCurrentTab, true);
        });
    connect(m_btnCloseSelected, &QPushButton::clicked, this, []() {
        Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_SetSelectedLayersOff");
        });
    connect(m_btnKeepSelected, &QPushButton::clicked, this, []() {
        Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_SetNotSelectedLayersOff");
        });

    // Connect table cell click to toggle layer visibility
    connect(m_layerTable, &QTableWidget::cellClicked, this, &Mx2dLayerManagerDialog::onLayerVisibilityToggled);
}

void Mx2dLayerManagerDialog::updateLayerTable(const Mx2d::Mx2dLayerInfo& info)
{
    // Disconnect signal to prevent feedback loops while updating
    disconnect(m_layerTable, &QTableWidget::cellClicked, this, &Mx2dLayerManagerDialog::onLayerVisibilityToggled);

    m_layerTable->setRowCount(0); // Clear table contents
    m_layerTable->setRowCount(info.datas.size());

    const QIcon visibleIcon(":/resources/images2d/2d_layerSwitchOn.svg");
    const QIcon hiddenIcon(":/resources/images2d/2d_layerSwitchOff.svg");

    for (int row = 0; row < info.datas.size(); ++row) {
        const auto& layerData = info.datas[row];

        // Column 0: On/Off Icon
        auto* iconLabel = new QLabel();
        iconLabel->setPixmap(layerData.isOff ? hiddenIcon.pixmap(20, 20) : visibleIcon.pixmap(20, 20));
        iconLabel->setAlignment(Qt::AlignCenter);
        m_layerTable->setCellWidget(row, 0, iconLabel);

        // Column 1: Color Swatch
        auto* colorLabel = new QLabel();
        QPixmap colorPixmap(20, 20);
        colorPixmap.fill(QColor(layerData.color.red(), layerData.color.green(), layerData.color.blue()));
        QPainter painter(&colorPixmap);
        painter.setPen(QColor(136, 136, 136));
        painter.drawRect(0, 0, colorPixmap.width() - 1, colorPixmap.height() - 1);
        colorLabel->setPixmap(colorPixmap);
        colorLabel->setAlignment(Qt::AlignCenter);
        m_layerTable->setCellWidget(row, 1, colorLabel);

        // Column 2: Layer Name
        auto* nameItem = new QTableWidgetItem(layerData.name);
        m_layerTable->setItem(row, 2, nameItem);

        m_layerTable->setRowHeight(row, 30);
    }

    // Reconnect the signal
    connect(m_layerTable, &QTableWidget::cellClicked, this, &Mx2dLayerManagerDialog::onLayerVisibilityToggled);
}

resbuf* Mx2dLayerManagerDialog::makeLayersStatusResbuf(const QList<McDbObjectId>& ids, const QList<bool>& offStatus)
{
    if (ids.isEmpty() || ids.length() != offStatus.length())
        return nullptr;

    resbuf* head = Mx::mcutBuildList(RTId, ids[0].asOldId(), 0);
    resbuf* current = head;

    resbuf* statusNode = Mx::mcutBuildList(RTSHORT, offStatus[0] ? 1 : 0, 0);
    current->rbnext = statusNode;
    current = statusNode;

    for (int i = 1; i < ids.length(); ++i) {
        resbuf* idNode = Mx::mcutBuildList(RTId, ids[i].asOldId(), 0);
        current->rbnext = idNode;
        current = idNode;

        statusNode = Mx::mcutBuildList(RTSHORT, offStatus[i] ? 1 : 0, 0);
        current->rbnext = statusNode;
        current = statusNode;
    }
    return head;
}

void Mx2dLayerManagerDialog::restoreInitialLayerState(QWidget* tab)
{
    if (!m_originLayerInfoMap.contains(tab)) return;

    const auto& initialInfo = m_originLayerInfoMap[tab];
    QList<McDbObjectId> ids;
    QList<bool> offStatus;
    for (const auto& layer : initialInfo.datas) {
        ids.append(layer.id);
        offStatus.append(layer.isOff);
    }
    setSomeLayersOffStatus(tab, ids, offStatus);
}

void Mx2dLayerManagerDialog::setSomeLayersOffStatus(QWidget* tab, const QList<McDbObjectId>& ids, const QList<bool>& offStatus)
{
    MrxDbgRbList spParam = makeLayersStatusResbuf(ids, offStatus);
    Mx2d::execCmd2d(tab, "Mx_SetSomeLayersOffStatus", spParam.orphanData());
}

void Mx2dLayerManagerDialog::setAllLayersOffStatus(QWidget* tab, bool isOff)
{
    if (!m_currentLayerInfoMap.contains(tab)) return;

    const auto& currentInfo = m_currentLayerInfoMap[tab]; // Use stored info for complete list
    QList<McDbObjectId> ids;
    QList<bool> offStatus;
    for (const auto& layer : currentInfo.datas) {
        ids.append(layer.id);
        offStatus.append(isOff);
    }
    setSomeLayersOffStatus(tab, ids, offStatus);
}

void Mx2dLayerManagerDialog::onLayerInformationReceived(const Mx2d::Mx2dLayerInfo& info)
{
    // If this is the first time we see this tab's layer info, store it as the "original" state.
    if (!m_originLayerInfoMap.contains(info.pTab)) {
        m_originLayerInfoMap[info.pTab] = info;
    }
    m_currentLayerInfoMap[info.pTab] = info;
    // Update the UI with the latest layer information.
    updateLayerTable(info);
}

void Mx2dLayerManagerDialog::onLayerVisibilityToggled(int row, int column)
{
    if (column != 0) return;
    QWidget* currentTab = MxUtils::gCurrentTab;
    if (!currentTab || !m_currentLayerInfoMap.contains(currentTab)) return;

    // Get the current layer info for the active tab
    const auto& currentLayers = m_currentLayerInfoMap[currentTab];
    if (row < 0 || row >= currentLayers.datas.size()) return;

    // Toggle the state of the clicked layer
    const auto& layerToToggle = currentLayers.datas[row];
    QList<McDbObjectId> ids = { layerToToggle.id };
    QList<bool> offStatus = { !layerToToggle.isOff }; // Invert the current status

    setSomeLayersOffStatus(currentTab, ids, offStatus);
}