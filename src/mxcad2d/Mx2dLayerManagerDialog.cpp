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
#include "Mx2dGuiDocument.h"
#include "MxSignalTransfer.h"
#include <QCoreApplication>


Mx2dLayerManagerDialog::Mx2dLayerManagerDialog(Mx2dGuiDocument* guiDoc, QWidget* parent)
    : QDialog(parent), m_guiDoc(guiDoc)
{
	// Set window flags for a standard, non-resizable dialog with a close button
	setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);

	setupUi();
	setButtonToolTips();
	connectSignals();

	setWindowTitle(tr("Layer Manager"));

    m_originLayerInfo = getAllLayersInfo();
    m_currentLayerInfo = m_originLayerInfo;
    updateLayerTable(m_currentLayerInfo);
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
    connect(m_btnRestoreInit, &QPushButton::clicked, this, [this]() {
        restoreInitialLayerState();
        });
    connect(m_btnOpenAll, &QPushButton::clicked, this, [this]() {
        setAllLayersOffStatus(false);
        });
    connect(m_btnCloseAll, &QPushButton::clicked, this, [this]() {
        setAllLayersOffStatus(true);
        });
    connect(m_btnCloseSelected, &QPushButton::clicked, this, [this]() {
        MxUtils::doAction([this]() { m_guiDoc->executeCommand("Mx_SetSelectedLayersOff"); });
        });
    connect(m_btnKeepSelected, &QPushButton::clicked, this, [this]() {
        MxUtils::doAction([this]() { m_guiDoc->executeCommand("Mx_SetNotSelectedLayersOff"); });
        });

    // Connect table cell click to toggle layer visibility
    connect(m_layerTable, &QTableWidget::cellClicked, this, &Mx2dLayerManagerDialog::onSwitchCellClicked);
    connect(&Mx2dSignalTransfer::getInstance(), &Mx2dSignalTransfer::signalLayersOffStatusSetted, this, [this](QWidget* curDoc) {
        if (curDoc != m_guiDoc)
            return;
		MxDrawApp::UpdateLayerDisplayStatus(m_guiDoc->cadView().GetOcxHandle());
		MxDrawApp::UpdateDisplay(m_guiDoc->cadView().GetOcxHandle());
        m_currentLayerInfo = getAllLayersInfo();
        updateLayerTable(m_currentLayerInfo);
        });
}


void Mx2dLayerManagerDialog::updateLayerTable(const Mx2d::LayerInfo& info)
{
	// Disconnect signal to prevent feedback loops while updating
	//disconnect(m_layerTable, &QTableWidget::cellClicked, this, &Mx2dLayerManagerDialog::onSwitchCellClicked);

	m_layerTable->setRowCount(0); // Clear table contents
	m_layerTable->setRowCount(info.size());

	const QIcon visibleIcon(":/resources/images2d/2d_layerSwitchOn.svg");
	const QIcon hiddenIcon(":/resources/images2d/2d_layerSwitchOff.svg");

	for (int row = 0; row < info.size(); ++row) {
		const auto& layerData = info[row];

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
	//connect(m_layerTable, &QTableWidget::cellClicked, this, &Mx2dLayerManagerDialog::onSwitchCellClicked);
}

void Mx2dLayerManagerDialog::restoreInitialLayerState()
{
	setSomeLayersOffStatus(m_originLayerInfo);
    m_currentLayerInfo = m_originLayerInfo;
    updateLayerTable(m_originLayerInfo);
}

void Mx2dLayerManagerDialog::setSomeLayersOffStatus(const Mx2d::LayerInfo& info)
{
	//MrxDbgRbList spParam = makeLayersStatusResbuf(info);
    //m_guiDoc->executeCommand("Mx_SetSomeLayersOffStatus", spParam.orphanData());


	for (const auto & ly : info)
	{
		McDbObjectId id = ly.id;
		bool status = ly.isOff;
		if (!id.isValid()) continue;
		McDbLayerTableRecordPointer spLayerRecord(id, McDb::kForWrite);
		if (spLayerRecord.openStatus() != Mcad::eOk) 
            continue;

		spLayerRecord->setIsOff(status);
	}
	MxDrawApp::UpdateLayerDisplayStatus(m_guiDoc->cadView().GetOcxHandle());
    MxDrawApp::UpdateDisplay(m_guiDoc->cadView().GetOcxHandle());

    for (const auto& layerNew : info)
    {
        for (auto& layer : m_currentLayerInfo)
        {
            if (layer.id == layerNew.id)
            {
                layer.isOff = layerNew.isOff;
                break;
            }
        }
    }
    updateLayerTable(m_currentLayerInfo);
}

void Mx2dLayerManagerDialog::setAllLayersOffStatus(bool isOff)
{
    Mx2d::LayerInfo layerInfo = m_originLayerInfo;
    for (auto & ly : layerInfo)
    {
        ly.isOff = isOff;
    }
	setSomeLayersOffStatus(layerInfo);
}

void Mx2dLayerManagerDialog::onSwitchCellClicked(int row, int column)
{
    if (column != 0) 
        return;
    if (row < 0 || row >= m_currentLayerInfo.length())
        return;
	Mx2d::LayerInfoDetail& ly = m_currentLayerInfo[row];
    ly.isOff = !ly.isOff;
	Mx2d::LayerInfo layerInfo;
	layerInfo.append(ly);
	setSomeLayersOffStatus(layerInfo);
    updateLayerTable(m_currentLayerInfo);
}

Mx2d::LayerInfo Mx2dLayerManagerDialog::getAllLayersInfo()
{
	McDbLayerTablePointer spLayerTable(Mx::mcdbCurDwg(), McDb::kForRead);
    if (spLayerTable.openStatus() != Mcad::eOk)
    {
        return {};
    }

	Mx2d::LayerInfo layerInfo;

	McDbLayerTableIterator* pLayerIter = nullptr;
    Mcad::ErrorStatus status = spLayerTable->newIterator(pLayerIter);
    if (status != Mcad::eOk)
    {
        return {};
    }
	if (pLayerIter)
	{
		for (pLayerIter->start(); !pLayerIter->done(); pLayerIter->step())
		{
			McDbLayerTableRecord* pRecord = nullptr;
			pLayerIter->getRecord(pRecord, McDb::kForRead);
			if (pRecord)
			{
				MxString sName;
				pRecord->getName(sName);
				QString qsName = QString::fromLocal8Bit(sName.c_str());
				// Skip internal annotation layer
				if (qsName == "MxCADAnnotationLayer") {
					pRecord->close();
					continue;
				}

				layerInfo.append({ pRecord->objectId(), pRecord->isOff(), qsName, pRecord->color() });
				pRecord->close();
			}
		}

		delete pLayerIter;
        pLayerIter = nullptr;
	}
	return layerInfo;
}
