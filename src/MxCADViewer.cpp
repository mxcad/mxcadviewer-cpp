/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "MxCADViewer.h"

#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QMenuBar>
#include <QToolBar>
#include <QTabWidget>
#include <QTabBar>
#include <QFileInfo>
#include <QMessageBox>
#include <QMenu>
#include <QMimeData>
#include <QToolButton>
#include <QProcess>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>


#include "MxRecentFileWidget.h"
#include "MxProgressDialog.h"
#include "MxRecentFileManager.h"

#ifdef MX_BUILD_3D
#include "Mx3dSetBgColorDialog.h"
#endif
#include "Mx2dLayerManagerDialog.h"
#include "Mx2dTextSearchDialog.h"
#include "Mx2dDimCategoryManagerDialog.h"
#include "Mx2dCompareSelDrawingDialog.h"
#include "Mx2dAnnotationSettingDialog.h"
#include "Mx2dMeasurementDialog.h"
#include "Mx2dMeasurementStatDialog.h"
#include "Mx2dShapeRecognitionDialog.h"
#include "Mx2dExtractTextDialog.h"
#include "Mx2dTableSaveDialog.h"
#include "Mx2dPrintHintDialog.h"
#include "Mx2dShapeListDialog.h"
#include "Mx2dShortcutSettingsDialog.h"
#include "Mx2dExportPdfDialog.h"
#include "Mx2dInsertSingleLineTextDialog.h"
#include "Mx2dMultiLineInputDialog.h"
#include "Mx2dLeaderTextInputDialog.h"
#include "Mx2dNumberedTextDialog.h"
#include "Mx2dPdfToDwgDialog.h"
#include "Mx2dSplitExportDialog.h"
#include "Mx2dBatchExportPdfDialog.h"
#include "MxCADInclude.h"
#include "Mx2dCalculateSideAreaDialog.h"
#include "Mx2dModifySingleLineTextDialog.h"
#include "Mx2dModifyMultiLineTextDialog.h"
#include "Mx2dAreaOffsetDialog.h"
#include "MxUtils.h"
#include "Mx2dSignalTransfer.h"
#include "Mx2dGuiDocument.h"
#ifdef MX_BUILD_3D
#include "Mx3dGuiDocument.h"
#endif
#include "MxSignalTransfer.h"
#include <thread>
#include "MxAboutMeDialog.h"

#include "MxLanguageManager.h"
#include "Mx3dProgressIndicator.h"

MxCADViewer::MxCADViewer(QWidget* parent)
	: QMainWindow(parent)
{
	setupUi();
	populateActionLists();
	connectSignalSlots();

	updateToolBar(m_tabWidget->currentIndex());
}

MxCADViewer::~MxCADViewer()
{
}

void MxCADViewer::setupUi()
{
	setWindowTitle(MxUtils::getAppVersion());
	setWindowIcon(QIcon(":/resources/images/mxlogo.svg"));
	setAcceptDrops(true);
	resize(1500, 1200);
	setMinimumSize(1200, 700);

	// --- Create UI Components ---
	createActions();
	createToolBars();
	createCentralWidget();

	// --- Setup Recent Files Tab ---
	m_recentFileWidget = new MxRecentFileWidget(m_tabWidget);
	int idx = m_tabWidget->addTab(m_recentFileWidget, tr("Recent Files"));
	m_tabWidget->setCurrentIndex(idx);

	// --- Progress Dialog ---
	m_progressDialog = new MxProgressDialog(this);
}

void MxCADViewer::createActions()
{
	m_openAction = new QAction(QIcon(":/resources/images/open.svg"), tr("Open"), this);
	m_openAction->setToolTip(tr("Open Model"));
	m_openAction->setShortcut(QKeySequence::Open);

	m_recentFileAction = new QAction(QIcon(":/resources/images/recent.svg"), tr("Recent"), this);
	m_recentFileAction->setToolTip(tr("Recent file history"));

	m_settingmenuAction = new QAction(QIcon(":/resources/images3d/setting.svg"), tr("Setting"), this);
	m_settingmenuAction->setToolTip(tr("Setting"));

	m_aboutAction = new QAction(QIcon(":/resources/images/about.svg"), tr("About"), this);
	m_aboutAction->setToolTip(tr("About CAD Dream Viewer"));

	m_settingLanguageAction = new QAction(QIcon(":/resources/images/language.svg"), tr("Language"), this);
    m_settingLanguageAction->setToolTip(tr("Setting Language"));

	m_englishAction = new QAction(tr("English"), this);
	m_englishAction->setToolTip(tr("English"));
	m_chineseAction = new QAction(tr("Chinese"), this);
	m_chineseAction->setToolTip(tr("Chinese"));

	auto languageMenu = new QMenu(this);
    languageMenu->addAction(m_englishAction);
    languageMenu->addAction(m_chineseAction);
    m_settingLanguageAction->setMenu(languageMenu);

	m_closeAction = new QAction(QIcon(":/resources/images/close.svg"), tr("Exit"), this);
	m_closeAction->setToolTip(tr("Exit Application"));
}

void MxCADViewer::createToolBars()
{
	m_mainToolBar = new QToolBar(tr("Main Toolbar"), this);
	m_mainToolBar->setFloatable(false);
	m_mainToolBar->setMovable(false);
	m_mainToolBar->setIconSize(QSize(40, 40));
	m_mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	addToolBar(Qt::TopToolBarArea, m_mainToolBar);
}

void MxCADViewer::createCentralWidget()
{
	m_tabWidget = new QTabWidget(this);
	m_tabWidget->setTabsClosable(true);
	m_tabWidget->setStyleSheet("QTabWidget { border: none; }");
	m_tabWidget->tabBar()->setContextMenuPolicy(Qt::CustomContextMenu);
	setCentralWidget(m_tabWidget);
}

void MxCADViewer::populateActionLists()
{
	addActionsToUserToolBar();
	addActionsToRecentFileToolBar();
	addActionsTo3dViewToolBar();
	addActionsTo2dViewToolBar();
}

void MxCADViewer::addActionsToUserToolBar()
{
	m_tabUserActions.append(m_openAction);
	m_tabUserActions.append(m_recentFileAction);
	m_tabUserActions.append(createSeparator());
	m_tabUserActions.append(m_aboutAction);
	m_tabUserActions.append(createSeparator());
	m_tabUserActions.append(m_settingLanguageAction);
	m_tabUserActions.append(createSeparator());
	m_tabUserActions.append(m_closeAction);
}

void MxCADViewer::addActionsToRecentFileToolBar()
{
	m_tabRecentFileActions.append(m_openAction);
	m_tabRecentFileActions.append(m_recentFileAction);
	
	m_tabRecentFileActions.append(createSeparator());
	m_tabRecentFileActions.append(m_aboutAction);
	m_tabRecentFileActions.append(createSeparator());
	m_tabRecentFileActions.append(m_settingLanguageAction);
	m_tabRecentFileActions.append(createSeparator());
	m_tabRecentFileActions.append(m_closeAction);
}

void MxCADViewer::addActionsTo3dViewToolBar()
{
	m_tab3dViewActions.append(m_openAction);
	m_tab3dViewActions.append(m_recentFileAction);
	m_tab3dViewActions.append(createSeparator());
	auto* changeBgColorAction = new QAction(tr("Change Background Color"), this);
	connect(changeBgColorAction, &QAction::triggered, this, &MxCADViewer::onChangeBgColor);

	auto* p3dVipMenu = new QMenu(this);
	p3dVipMenu->addAction(changeBgColorAction);
	m_settingmenuAction->setMenu(p3dVipMenu);

	m_tab3dViewActions.append(m_settingmenuAction);
	m_tab3dViewActions.append(createSeparator());
	m_tab3dViewActions.append(m_aboutAction);
	m_tab3dViewActions.append(createSeparator());
	m_tab3dViewActions.append(m_settingLanguageAction);
	m_tab3dViewActions.append(createSeparator());
	m_tab3dViewActions.append(m_closeAction);
}

void MxCADViewer::addActionsTo2dViewToolBar()
{
	// --- Standard Actions ---
	m_tab2dViewActions.append(m_openAction);
	m_tab2dViewActions.append(m_recentFileAction);
	m_tab2dViewActions.append(createSeparator());
	m_tab2dViewActions.append(m_aboutAction);
	m_tab2dViewActions.append(createSeparator());
	m_tab2dViewActions.append(m_settingLanguageAction);
	m_tab2dViewActions.append(createSeparator());
	m_tab2dViewActions.append(m_closeAction);
	m_tab2dViewActions.append(createSeparator());

	// --- Window Menu ---
	auto* pWinMenu = new QMenu(this);
	auto* pActZoomAll = new QAction(QIcon(":/resources/images2d/2d_zoomAll.svg"), tr("Zoom All"), this);
	auto* pActZoomWin = new QAction(QIcon(":/resources/images2d/2d_zoomWin.svg"), tr("Zoom Window"), this);
	pWinMenu->addAction(pActZoomAll);
	pWinMenu->addAction(pActZoomWin);
	connect(pActZoomAll, &QAction::triggered, this, []() { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_ZoomAll"); });
	connect(pActZoomWin, &QAction::triggered, this, []() { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_ZoomWin"); });
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_window.svg", tr("Window"), MxPageType::PAGE_2D, [this](MxPageType type) { onDoNothing(type); }, pWinMenu);

	// --- Basic 2D Actions ---
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_layer.svg", tr("Layers"), MxPageType::PAGE_2D, [this](MxPageType type) { onLayerManager(type); });
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_undo.svg", tr("Undo"), MxPageType::PAGE_2D, [this](MxPageType type) { emit Mx2dSignalTransfer::getInstance().signalUndo(MxUtils::gCurrentTab); });
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_redo.svg", tr("Redo"), MxPageType::PAGE_2D, [this](MxPageType type) { emit Mx2dSignalTransfer::getInstance().signalRedo(MxUtils::gCurrentTab); });

	// --- VIP Menu (fully populated) ---
	auto* pVipMenu = new QMenu(this);
	auto* pActExtractText = new QAction(tr("Extract Text"), this);
	connect(pActExtractText, &QAction::triggered, this, [this]() { if (m_extractTextDialog && m_extractTextDialog->isVisible()) m_extractTextDialog->accept(); Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_ExtractTextRect"); });
	auto* pActExtractTable = new QAction(tr("Extract Table"), this);
	connect(pActExtractTable, &QAction::triggered, this, [this]() { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_ExtractTable"); });

	auto* pActArcLength = new QAction(tr("Arc Length"), this);
	connect(pActArcLength, &QAction::triggered, this, [this]() { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_DrawArcLengthDimMark"); });

	auto* pActPt2LineDist = new QAction(tr("Point to Line Distance"), this);
    connect(pActPt2LineDist, &QAction::triggered, this, [this]() { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_DrawDistPointToLineMark"); });

	auto* pActContinuousMeasurement = new QAction(tr("Continuous Measurement"), this);
    connect(pActContinuousMeasurement, &QAction::triggered, this, [this]() { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_DrawContinuousMeasurementMark"); });

	auto* pActBatchMeasurement = new QAction(tr("Batch Measurement"), this);
    connect(pActBatchMeasurement, &QAction::triggered, this, [this]() { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_BatchMeasure"); });

	auto* pActViewSegmentLength = new QAction(tr("View Segment Length"), this);
    connect(pActViewSegmentLength, &QAction::triggered, this, [this]() { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_ShowSegmentLengths"); });

	auto* pActAreaWithArc = new QAction(tr("Area with Arc"), this);
    connect(pActAreaWithArc, &QAction::triggered, this, [this]() { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_DrawArcPolyAreaMark"); });

	auto* pActMeasureFillArea = new QAction(tr("Measure Fill Area"), this);
    connect(pActMeasureFillArea, &QAction::triggered, this, [this]() { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_DrawHatchArea2Mark"); });

	auto* pActCalculateSideArea = new QAction(tr("Calculate Side Area"), this);
    connect(pActCalculateSideArea, &QAction::triggered, this, [this]() { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_CalculateSiderArea"); });

	auto* pActAreaOffset = new QAction(tr("Area Offset"), this);
    connect(pActAreaOffset, &QAction::triggered, this, [this]() { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_SelectAreaToOffset"); });

	auto* pActMeasureCircle = new QAction(tr("Measure Circle"), this);
    connect(pActMeasureCircle, &QAction::triggered, this, [this]() { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_DrawCircleMeasurementMark"); });

	auto* pActMeasureAngle = new QAction(tr("Measure Angle"), this);
    connect(pActMeasureAngle, &QAction::triggered, this, [this]() { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_DrawAngleMeasurementMark"); });


	auto* pActCADSplitExport = new QAction(tr("Split and Export CAD"), this);
	connect(pActCADSplitExport, &QAction::triggered, this, [this]() { onCADSplitExport(MxPageType::PAGE_2D); });
	auto* pActPDFToCAD = new QAction(tr("PDF to CAD"), this);
	connect(pActPDFToCAD, &QAction::triggered, this, [this]() { onPdfToDwg(MxPageType::PAGE_2D); });
	auto* pActExportPDF = new QAction(tr("Export PDF"), this);
	connect(pActExportPDF, &QAction::triggered, this, [this]() { onExportPDF(MxPageType::PAGE_2D); });
	auto* pActBatchExportPDF = new QAction(tr("Batch Export PDF"), this);
	connect(pActBatchExportPDF, &QAction::triggered, this, [this]() { onBatchExportPDF(MxPageType::PAGE_2D); });
	auto* pActShortcutSettings = new QAction(tr("Shortcut Settings"), this);
	connect(pActShortcutSettings, &QAction::triggered, this, [this]() { onShortcutSettings(MxPageType::PAGE_2D); });
	pVipMenu->addAction(pActExtractText);
	pVipMenu->addAction(pActExtractTable);
	pVipMenu->addAction(pActArcLength);
	pVipMenu->addAction(pActPt2LineDist);
	pVipMenu->addAction(pActContinuousMeasurement);
	pVipMenu->addAction(pActBatchMeasurement);
	pVipMenu->addAction(pActViewSegmentLength);
#ifdef MX_DEVELOPING
	pVipMenu->addAction(new QAction(tr("Modify Single Dimension"), this));
	pVipMenu->addAction(new QAction(tr("External Reference Manager"), this));
	pVipMenu->addAction(new QAction(tr("CAD Gray Display"), this));
	pVipMenu->addAction(new QAction(tr("T-Arch Convert (to T3)"), this));
	pVipMenu->addAction(new QAction(tr("CAD Version Conversion"), this));
	pVipMenu->addAction(pActCADSplitExport);
	pVipMenu->addAction(new QAction(tr("Layout to Model"), this));
	pVipMenu->addAction(pActPDFToCAD);
	pVipMenu->addAction(pActExportPDF);
	pVipMenu->addAction(pActBatchExportPDF);
	pVipMenu->addAction(pActShortcutSettings);
#endif
	pVipMenu->addAction(pActAreaWithArc);
	pVipMenu->addAction(pActMeasureFillArea);
	pVipMenu->addAction(pActCalculateSideArea);
	pVipMenu->addAction(pActAreaOffset);
	pVipMenu->addAction(pActMeasureCircle);
	pVipMenu->addAction(pActMeasureAngle);
	
	
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_vip.svg", tr("VIP"), MxPageType::PAGE_2D, [this](MxPageType type) { onDoNothing(type); }, pVipMenu);

	// --- Main 2D Tools ---
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_measure.svg", tr("Measure"), MxPageType::PAGE_2D, [this](MxPageType type) { onMeasurement(type); });
#ifdef MX_DEVELOPING
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_measureStatistics.svg", tr("Statistics"), MxPageType::PAGE_2D, [this](MxPageType type) { onMeasurementStat(type); });
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_compare.svg", tr("Compare"), MxPageType::PAGE_2D, [this](MxPageType type) { onDrawingCompare(type); });
	// --- Shape Recognition Menu ---
	auto* pShapeRecognitionMenu = new QMenu(this);
	auto* pActShapeRecognition = new QAction(QIcon(":/resources/images2d/2d_graphicRecognition.svg"), tr("Recognize Shapes"), this);
	connect(pActShapeRecognition, &QAction::triggered, this, [this]() { onShapeRecognition(MxPageType::PAGE_2D); });
	auto* pActRecognitedList = new QAction(tr("View Recognized Shapes"), this);
	connect(pActRecognitedList, &QAction::triggered, this, [this]() { onLookRecognitedShapeList(MxPageType::PAGE_2D); });
	auto* pActShapeRecogDemo = new QAction(tr("Recognition Demo"), this);
	pShapeRecognitionMenu->addAction(pActShapeRecognition);
	pShapeRecognitionMenu->addAction(pActRecognitedList);
	pShapeRecognitionMenu->addAction(pActShapeRecogDemo);
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_graphicRecognition.svg", tr("Recognition"), MxPageType::PAGE_2D, [this](MxPageType type) { onDoNothing(type); }, pShapeRecognitionMenu);
#endif
	

	// --- Text Menu ---
	auto* pTextMenu = new QMenu(this);
	auto* pActSingleLineText = new QAction(tr("Single Line Text"), this);
	connect(pActSingleLineText, &QAction::triggered, this, [this]() { onInsertSingleLineText(MxPageType::PAGE_2D); });
	auto* pActMultLinesText = new QAction(tr("Multi Line Text"), this);
	connect(pActMultLinesText, &QAction::triggered, this, [this]() { onMultiLineInput(MxPageType::PAGE_2D); });
	auto* pActNumberText = new QAction(tr("Numbered Text"), this);
	connect(pActNumberText, &QAction::triggered, this, [this]() { onNumberedText(MxPageType::PAGE_2D); });
	auto* pActModText = new QAction(tr("Modify Text"), this);
	connect(pActModText, &QAction::triggered, this, [this]() { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_SelectTextToModify"); });
	auto* pActMoveText = new QAction(tr("Move Text"), this);
	connect(pActMoveText, &QAction::triggered, this, [this](bool) { onMoveText(MxPageType::PAGE_2D); });
	auto* pActCloneText = new QAction(tr("Copy Text"), this);
	connect(pActCloneText, &QAction::triggered, this, [this](bool) { onCloneText(MxPageType::PAGE_2D); });
	pTextMenu->addAction(pActSingleLineText); pTextMenu->addAction(pActMultLinesText); pTextMenu->addAction(pActNumberText);
	pTextMenu->addAction(pActModText); pTextMenu->addAction(pActMoveText); pTextMenu->addAction(pActCloneText);
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_text.svg", tr("Text"), MxPageType::PAGE_2D, [this](MxPageType type) { onDoNothing(type); }, pTextMenu);

	// --- Draw Line ---
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_line.svg", tr("Draw Line"), MxPageType::PAGE_2D, [](MxPageType) { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_DrawLine"); });

	// --- Shapes Menu ---
	auto* pShapeMenu = new QMenu(this);
	auto* pActRectangle = new QAction(QIcon(":/resources/images2d/2d_dimRectangle.svg"), tr("Rectangle"), this);
	connect(pActRectangle, &QAction::triggered, this, []() { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_DrawRectMark"); });
	auto* pActEllipse = new QAction(QIcon(":/resources/images2d/2d_dimEllipse.svg"), tr("Ellipse"), this);
	connect(pActEllipse, &QAction::triggered, this, []() { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_DrawEllipseMark"); });
	auto* pActCloudLine = new QAction(QIcon(":/resources/images2d/2d_dimCloud.svg"), tr("Cloud"), this);
	connect(pActCloudLine, &QAction::triggered, this, []() { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_DrawRectCloudMark"); });
	auto* pActLeadLine = new QAction(QIcon(":/resources/images2d/2d_dimLeader.svg"), tr("Leader"), this);
	connect(pActLeadLine, &QAction::triggered, this, [this]() { if (!m_leaderTextInputDialog) { m_leaderTextInputDialog = new Mx2dLeaderTextInputDialog(this); } m_leaderTextInputDialog->show(); });
	pShapeMenu->addAction(pActRectangle); pShapeMenu->addAction(pActEllipse); pShapeMenu->addAction(pActCloudLine); pShapeMenu->addAction(pActLeadLine);
#ifdef MX_DEVELOPING
	pShapeMenu->addAction(new QAction(QIcon(":/resources/images2d/2d_dimPic.svg"), tr("Picture"), this));
	pShapeMenu->addAction(new QAction(QIcon(":/resources/images2d/2d_dimFreeDraw.svg"), tr("Freehand"), this));
#endif // MX_DEVELOPING
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_shapes.svg", tr("Shapes"), MxPageType::PAGE_2D, [this](MxPageType type) { onDoNothing(type); }, pShapeMenu);

	// --- Delete Menu ---
	auto* pDeleteMenu = new QMenu(this);
	auto* pActDelDim = new QAction(tr("Delete Annotation"), this);
	connect(pActDelDim, &QAction::triggered, this, []() { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_EraseAnnotation"); });
	auto* pActDelAllDim = new QAction(tr("Delete All Annotations"), this);
	connect(pActDelAllDim, &QAction::triggered, this, [this]() {
		if (QMessageBox::question(this, tr("Confirm"), tr("Are you sure you want to delete all annotations?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
			Mx2dSignalTransfer::getInstance().signalEraseAllAnnotations(MxUtils::gCurrentTab);
		}
		});
	pDeleteMenu->addAction(pActDelDim); pDeleteMenu->addAction(pActDelAllDim);
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_delete.svg", tr("Delete"), MxPageType::PAGE_2D, [this](MxPageType type) { onDoNothing(type); }, pDeleteMenu);

	// --- Hide/Show Annotation ---
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_hideDim.svg", tr("Hide Annotations"), MxPageType::PAGE_2D, [this](MxPageType) {
		static bool isShow = false;
		//MrxDbgRbList spParam = Mx::mcutBuildList(RTSHORT, isShow ? 1 : 0, 0);
		//Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_ShowHideAnnotation", spParam.orphanData());
		emit Mx2dSignalTransfer::getInstance().signalShowAllAnnotations(MxUtils::gCurrentTab, isShow);
		isShow = !isShow;
		});

	// --- Import/Export Menu ---
	auto* pImportExportMenu = new QMenu(this);
	pImportExportMenu->addAction(new QAction(tr("Import Drawing and Annotations"), this));
	pImportExportMenu->addAction(new QAction(tr("Export Drawing and Annotations"), this));
	pImportExportMenu->addAction(pActExportPDF);       // Reuse existing action
	pImportExportMenu->addAction(pActBatchExportPDF);  // Reuse existing action
#ifdef MX_DEVELOPING
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_inputOutput.svg", tr("Import/Export"), MxPageType::PAGE_2D, [this](MxPageType type) { onDoNothing(type); }, pImportExportMenu);

	// --- More Tools ---
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_dimSetting.svg", tr("Settings"), MxPageType::PAGE_2D, [this](MxPageType type) { onDimSetting(type); });
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_proportion.svg", tr("Scale"), MxPageType::PAGE_2D, [](MxPageType) { /* TODO: Implement scale logic */ });
#endif // MX_DEVELOPING
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_textSearch.svg", tr("Find Text"), MxPageType::PAGE_2D, [this](MxPageType type) { onTextSearch(type); });

#ifdef MX_DEVELOPING
	// --- Screen Rotation Menu ---
	auto* pScreenRotMenu = new QMenu(this);
	pScreenRotMenu->addAction(new QAction(tr(u8"Rotate Clockwise 90°"), this));
	pScreenRotMenu->addAction(new QAction(tr(u8"Rotate Counter-Clockwise 90°"), this));
	pScreenRotMenu->addAction(new QAction(tr("Custom Rotation"), this));
	pScreenRotMenu->addSeparator();
	pScreenRotMenu->addAction(new QAction(tr("Restore Initial View"), this));
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_screenRotate.svg", tr("Rotate View"), MxPageType::PAGE_2D, [this](MxPageType type) { onDoNothing(type); }, pScreenRotMenu);

	// --- Final Tools ---
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_print.svg", tr("Print"), MxPageType::PAGE_2D, [this](MxPageType type) { onPrint(type); });
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_dimClassify.svg", tr("Annotation Categories"), MxPageType::PAGE_2D, [this](MxPageType type) { onDimCategoryManager(type); });
#endif // MX_DEVELOPING
}

void MxCADViewer::dragEnterEvent(QDragEnterEvent* event)
{
	// Check if the dragged content contains local file URLs.
	for (const QUrl& url : event->mimeData()->urls()) {
		if (url.isLocalFile()) {
			QFileInfo fileInfo(url.toLocalFile());
			if (fileInfo.isFile()) { // Ensure it's a file, not a directory
				event->acceptProposedAction();
				return;
			}
		}
	}
}

void MxCADViewer::dropEvent(QDropEvent* event)
{
	const QMimeData* mimeData = event->mimeData();
	if (mimeData->hasUrls()) {
		// Process only the first valid file URL.
		for (const QUrl& url : mimeData->urls()) {
			QString filePath = url.toLocalFile();
			if (!filePath.isEmpty()) {
				onOpenFile(filePath);
				return;
			}
		}
	}
}

void MxCADViewer::connectSignalSlots()
{
	connect(&MxSignalTransfer::getInstance(), &MxSignalTransfer::signalOpenFile, this, &MxCADViewer::onOpenFile);

	// --- Signals for 2D specific functionality ---
	connect(&Mx2dSignalTransfer::getInstance(), &Mx2dSignalTransfer::signalExtractTextFinished, this, [this](QWidget* tab, const QStringList& textList) {
		Q_UNUSED(tab); onExtractText(MxPageType::PAGE_2D); if (m_extractTextDialog) m_extractTextDialog->setTexts(textList);
		});
	connect(&Mx2dSignalTransfer::getInstance(), &Mx2dSignalTransfer::signalAreaMarkInformation, this, [this](QWidget* tab, const Mx2d::PLVertexList& arr) {
		Q_UNUSED(tab); /*onCalculateSideArea(MxPageType::PAGE_2D);*/ if (!m_calculateSideAreaDialog) { m_calculateSideAreaDialog = new Mx2dCalculateSideAreaDialog(this); } m_calculateSideAreaDialog->setPolylineData(arr); m_calculateSideAreaDialog->exec();
		});
	connect(&Mx2dSignalTransfer::getInstance(), &Mx2dSignalTransfer::signalTextModify, this, [this](QWidget* tab, const QString& oldStr, McDbObjectId id) {
		Q_UNUSED(tab); if (!m_modifySingleLineTextDialog) m_modifySingleLineTextDialog = new Mx2dModifySingleLineTextDialog(this);
		m_modifySingleLineTextDialog->setTextId(id); m_modifySingleLineTextDialog->setTextContent(oldStr); m_modifySingleLineTextDialog->exec();
		});
	connect(&Mx2dSignalTransfer::getInstance(), &Mx2dSignalTransfer::signalMTextModify, this, [this](QWidget* tab, const QString& oldStr, McDbObjectId id) {
		Q_UNUSED(tab); if (!m_modifyMultiLineTextDialog) m_modifyMultiLineTextDialog = new Mx2dModifyMultiLineTextDialog(this);
		m_modifyMultiLineTextDialog->setTextId(id); m_modifyMultiLineTextDialog->setTextContent(oldStr); m_modifyMultiLineTextDialog->exec();
		});
	connect(&Mx2dSignalTransfer::getInstance(), &Mx2dSignalTransfer::signalAreaMarkId, this, [this](QWidget* tab, McDbObjectId id) {
		Q_UNUSED(tab); if (!m_areaOffsetDialog) m_areaOffsetDialog = new Mx2dAreaOffsetDialog(this);
		m_areaOffsetDialog->setAreaMarkId(id); m_areaOffsetDialog->show();
		});

	// --- UI Component Signals ---
	connect(m_openAction, &QAction::triggered, this, &MxCADViewer::onOpen);
	connect(m_recentFileAction, &QAction::triggered, this, &MxCADViewer::onRecentFile);
	connect(m_closeAction, &QAction::triggered, this, &QWidget::close);
	connect(m_aboutAction, &QAction::triggered, this, &MxCADViewer::onAbout);
	connect(m_englishAction, &QAction::triggered, this, &MxCADViewer::onEnglish);
	connect(m_chineseAction, &QAction::triggered, this, &MxCADViewer::onChinese);
	connect(m_tabWidget, &QTabWidget::tabCloseRequested, this, &MxCADViewer::onTabCloseRequested);
	connect(m_tabWidget, &QTabWidget::currentChanged, this, &MxCADViewer::onTabChanged);
	connect(m_tabWidget, &QTabWidget::currentChanged, this, &MxCADViewer::updateToolBar);

	connect(m_tabWidget->tabBar(), &QTabBar::customContextMenuRequested, this, [this](const QPoint& pos) {
		int tabIndex = m_tabWidget->tabBar()->tabAt(pos);
		if (tabIndex != -1) {
			QWidget* w = m_tabWidget->widget(tabIndex);
			QString filePath = m_mapFileOpened.key(w, "");
			if (!filePath.isEmpty()) {
				QMenu menu;
				QAction* openDirAction = menu.addAction(tr("Open Containing Folder"));
				connect(openDirAction, &QAction::triggered, [filePath]() { openFolderAndSelectFile(filePath); });
				menu.exec(m_tabWidget->tabBar()->mapToGlobal(pos));
			}
		}
		});
}

void MxCADViewer::onOpen()
{
	QString filePath = QFileDialog::getOpenFileName(this, tr("Select File"), "",
		tr("All Files (*.*);;"
			"CAD Files (*.dwg *.dxf *.mxweb);;"

#ifdef MX_BUILD_3D
			"STEP Files (*.step *.stp);;" "IGES Files (*.iges *.igs);;" "STL Files (*.stl);;"
			"VRML Files (*.wrl *.wrz *.vrml);;" "BREP Files (*.brep *.rle *.occ);;"
			"glTF Files (*.gltf *.glb);;" "OBJ Files (*.obj)"
#endif // MX_BUILD3D

		
		));

	if (!filePath.isEmpty()) { onOpenFile(filePath); }
}

void MxCADViewer::onRecentFile()
{
	m_tabWidget->insertTab(0, m_recentFileWidget, tr("Recent Files"));
	m_tabWidget->setCurrentIndex(0);
}


void MxCADViewer::onAbout()
{
	MxAboutMeDialog aboutDialog(this);
	aboutDialog.exec();
}

void MxCADViewer::onEnglish()
{
	MxLanguageManager::getInstance().setLanguage("en_US");
	// prompt: restart the application to apply the changes
    QMessageBox::information(this, tr("Restart"), tr("Please restart the application to apply the changes."));
}

void MxCADViewer::onChinese()
{
    MxLanguageManager::getInstance().setLanguage("zh_CN");
	QMessageBox::information(this, tr("Restart"), tr("Please restart the application to apply the changes."));
}

void MxCADViewer::onTabChanged(int index)
{
	MxDrawApp::StopAllCommand();
	emit MxSignalTransfer::getInstance().signalHidePromptMessage(MxUtils::gCurrentTab);
	QWidget* widget = m_tabWidget->widget(index);
	MxUtils::gCurrentTab = widget;
	if (!widget) return;
	if (auto* pMxCAD = qobject_cast<Mx2dGuiDocument*>(widget)) {
		pMxCAD->triggerUpdate();
		pMxCAD->makeCurrent();
	}
}

void MxCADViewer::onTabCloseRequested(int index)
{
	QWidget* widget = m_tabWidget->widget(index);
	if (!widget) return;

	// For special tabs, just remove them from view
	if (widget == m_recentFileWidget) {
		m_tabWidget->removeTab(index);
	}
	// For document tabs, remove the tab and handle cleanup
	else if (
#ifdef MX_BUILD_3D
		qobject_cast<Mx3dGuiDocument*>(widget) ||
#endif
		qobject_cast<Mx2dGuiDocument*>(widget)) {
		m_tabWidget->removeTab(index);
		QString key = m_mapFileOpened.key(widget);
		if (!key.isEmpty()) { m_mapFileOpened.remove(key); }
		widget->deleteLater();
	}
}

void MxCADViewer::onAddOpenedFile(QString path, QWidget* guidoc)
{
	m_mapFileOpened.insert(path, guidoc);
	qDebug() << "Opened file: " << path;
}

void MxCADViewer::onOpenFile(const QString& filePath)
{
	QFileInfo fileInfo(filePath);
	if (!fileInfo.exists()) {
		QMessageBox::warning(this, tr("Open Failed"), tr("File does not exist!"));
		MxRecentFileManager::instance()->removeFile(filePath);
		return;
	}
	if (m_mapFileOpened.contains(filePath)) {
		m_tabWidget->setCurrentWidget(m_mapFileOpened.value(filePath));
		return;
	}
	QString suffix = fileInfo.suffix().toLower();
	if (suffix == "dwg" || suffix == "dxf" || suffix == "mxweb") {
		newCADDocument(filePath);
	}
	else
	{
		
#ifdef MX_BUILD_3D
		newDocument(filePath);
#else
		QMessageBox::warning(this, tr("Open Failed"), tr("Unsupported file format!"));
#endif
	}
}

void MxCADViewer::updateToolBar(int index)
{
	QWidget* widget = m_tabWidget->widget(index);
	m_mainToolBar->clear();

	const QList<QAction*>* actions_to_add = &m_tabUserActions; // Default

	
#ifdef MX_BUILD_3D
	if (qobject_cast<Mx3dGuiDocument*>(widget)) {
		actions_to_add = &m_tab3dViewActions;
	}
	else
#endif
		if (qobject_cast<Mx2dGuiDocument*>(widget)) {
		actions_to_add = &m_tab2dViewActions;
	}
	else if (qobject_cast<MxRecentFileWidget*>(widget)) {
		actions_to_add = &m_tabRecentFileActions;
	}

	for (QAction* action : *actions_to_add) {
		m_mainToolBar->addAction(action);
		setToolButtonPopupMode(action);
	}
}

void MxCADViewer::setToolButtonPopupMode(QAction* action)
{
	if (!action || !action->menu()) return;

	if (QWidget* widget = m_mainToolBar->widgetForAction(action)) {
		if (auto* toolButton = qobject_cast<QToolButton*>(widget)) {
			toolButton->setPopupMode(QToolButton::InstantPopup);
			toolButton->setStyleSheet("QToolButton::menu-indicator { image: none; }");
		}
	}
}

void MxCADViewer::createAndAddAction(QList<QAction*>& group, const QString& iconPath, const QString& text, MxPageType pageType, const FunctionCallBack& callback, QMenu* menu)
{
	auto* action = new QAction(QIcon(iconPath), text, this);
	if (menu) { action->setMenu(menu); }
	connect(action, &QAction::triggered, this, [this, pageType, callback]() { callback(pageType); });
	group.append(action);
}

QAction* MxCADViewer::createSeparator()
{
	auto* sep = new QAction(this);
	sep->setSeparator(true);
	return sep;
}

void MxCADViewer::newCADDocument(const QString& path)
{
	auto* guiDoc = new Mx2dGuiDocument(m_tabWidget);
	connect(guiDoc, &Mx2dGuiDocument::startReadFile, this, [this, path]() {
		m_progressDialog->start();
		m_progressDialog->setProgressPercent(0);
		m_progressDialog->setProgressDetail(tr("Reading: ") + path);
});
	connect(guiDoc, &Mx2dGuiDocument::fileRead, this, [this, guiDoc, path](bool success) {
		if (!success) 
		{
			m_progressDialog->stop();
			guiDoc->deleteLater();
			QMessageBox::warning(this, tr("Failed"), QString(tr("Failed to read file:%1")).arg(path));
		}
		});	
	connect(guiDoc, &Mx2dGuiDocument::startRender, this, [this]() {
		m_progressDialog->setProgressPercent(100);
		m_progressDialog->setProgressDetail(tr("Rendering..."));
		});
	connect(guiDoc, &Mx2dGuiDocument::fileReady, this, [this, guiDoc, path]() {
		m_progressDialog->stop();
		QFileInfo fileInfo(path);
		int index = m_tabWidget->addTab(guiDoc, fileInfo.fileName());
		m_tabWidget->setCurrentIndex(index);
		onAddOpenedFile(path, guiDoc);
		});
	connect(guiDoc, &Mx2dGuiDocument::runConverterFailed, this, [this, guiDoc]() {
		m_progressDialog->stop();
		guiDoc->deleteLater();
		QMessageBox::warning(this, tr("Failed"), tr("Failed to run mxconvert!"));
		});
	connect(guiDoc, &Mx2dGuiDocument::convertFailed, this, [this, guiDoc](const QString& error) {
		m_progressDialog->stop();
		guiDoc->deleteLater();
		QMessageBox::warning(this, tr("Failed"), QString(tr("Failed to convert! error:%1")).arg(error));
		});
	guiDoc->openFile(path);
}

void MxCADViewer::newDocument(const QString& path)
{

#ifdef MX_BUILD_3D
	auto* guiDoc = new Mx3dGuiDocument(m_tabWidget);
	connect(guiDoc, &Mx3dGuiDocument::startReadFile, this, [this](const QString& filePath) {
		m_progressDialog->start();
		m_progressDialog->setProgressPercent(0);
		m_progressDialog->setProgressDetail(tr("Reading: ") + filePath);
		});
	connect(guiDoc, &Mx3dGuiDocument::startRender, this, [this]() {
		m_progressDialog->setProgressPercent(100);
		m_progressDialog->setProgressDetail(tr("Rendering..."));
		});
	connect(guiDoc, &Mx3dGuiDocument::stopProgress, this, [this]() {
		m_progressDialog->stop();
		});
	connect(guiDoc, &Mx3dGuiDocument::fileReady, this, [this, guiDoc, path](bool success) {
		if (success)
		{
			QFileInfo fileInfo(path);
			int index = m_tabWidget->addTab(guiDoc, fileInfo.fileName());
			m_tabWidget->setCurrentIndex(index);
			onAddOpenedFile(path, guiDoc);
		}
		else
		{
			guiDoc->deleteLater();
		}
		});
	connect(guiDoc->getProgressIndicator(), &Mx3dProgressIndicator::progressDetailChanged, this, [this](const QString& detail) {
		m_progressDialog->setProgressDetail(detail);
		});
	connect(guiDoc->getProgressIndicator(), &Mx3dProgressIndicator::progressPercentChanged, this, [this](int percent) {
		m_progressDialog->setProgressPercent(percent);
		});
	connect(m_progressDialog, &MxProgressDialog::abort, guiDoc->getProgressIndicator(), &Mx3dProgressIndicator::onAborted);
	guiDoc->openFile(path);
#endif
}

void MxCADViewer::openFolderAndSelectFile(const QString& filePath)
{
	QFileInfo fileInfo(filePath);
	if (!fileInfo.exists()) return;

#ifdef Q_OS_WIN
	QProcess::startDetached("explorer.exe", { "/select,", QDir::toNativeSeparators(filePath) });
#elif defined(Q_OS_MAC)
	QProcess::startDetached("open", { "-R", fileInfo.absoluteFilePath() });
#else
	if (!QProcess::startDetached("nautilus", { "--select", fileInfo.absoluteFilePath() }) &&
		!QProcess::startDetached("dolphin", { "--select", fileInfo.absoluteFilePath() })) {
		QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absolutePath()));
	}
#endif
}

// Macro to simplify lazy initialization and showing of dialogs
#define LAZY_INIT_DIALOG(dialog_member, DialogClass, modal_or_nonmodal) \
    if (!dialog_member) { dialog_member = new DialogClass(this); } \
    if constexpr (std::is_same_v<decltype(modal_or_nonmodal), std::true_type>) { dialog_member->exec(); } \
    else { if (!dialog_member->isVisible()) dialog_member->show(); else dialog_member->activateWindow(); }



void MxCADViewer::onCloseGuiDocument(QWidget* widget)
{
	int index = m_tabWidget->indexOf(widget);
	if (index != -1) { onTabCloseRequested(index); }
}

void MxCADViewer::onSetWindowSubTitle(const QString& subTitle)
{
	setWindowTitle(MxUtils::getAppVersion() + subTitle);
}

void MxCADViewer::onChangeBgColor() { 
#ifdef MX_BUILD_3D
	LAZY_INIT_DIALOG(m_setBgColorDialog, Mx3dSetBgColorDialog, std::false_type{});
#endif
	
}
void MxCADViewer::onDoNothing(MxPageType) {}
void MxCADViewer::onDoTest(MxPageType) {

}

// --- 2D Dialog Slots Implementation ---
void MxCADViewer::onLayerManager(MxPageType pageType) { if (pageType != MxPageType::PAGE_2D) return; LAZY_INIT_DIALOG(m_layerManagerDialog, Mx2dLayerManagerDialog, std::false_type{}); Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_GetAllLayer"); }
void MxCADViewer::onTextSearch(MxPageType pageType) { if (pageType != MxPageType::PAGE_2D) return; LAZY_INIT_DIALOG(m_textSearchDialog, Mx2dTextSearchDialog, std::false_type{}); }
void MxCADViewer::onDimCategoryManager(MxPageType pageType) { if (pageType != MxPageType::PAGE_2D) return; LAZY_INIT_DIALOG(m_dimCategoryManagerDialog, Mx2dDimCategoryManagerDialog, std::true_type{}); }
void MxCADViewer::onDrawingCompare(MxPageType pageType) { if (pageType != MxPageType::PAGE_2D) return; LAZY_INIT_DIALOG(m_compareSelDrawingDialog, Mx2dCompareSelDrawingDialog, std::true_type{}); }
void MxCADViewer::onDimSetting(MxPageType pageType) { if (pageType != MxPageType::PAGE_2D) return; LAZY_INIT_DIALOG(m_annotationSettingDialog, Mx2dAnnotationSettingDialog, std::true_type{}); }
void MxCADViewer::onMeasurement(MxPageType pageType) { if (pageType != MxPageType::PAGE_2D) return; LAZY_INIT_DIALOG(m_measurementDialog, Mx2dMeasurementDialog, std::false_type{}); }
void MxCADViewer::onMeasurementStat(MxPageType pageType) { if (pageType != MxPageType::PAGE_2D) return; LAZY_INIT_DIALOG(m_measurementStatDialog, Mx2dMeasurementStatDialog, std::false_type{}); }
void MxCADViewer::onShapeRecognition(MxPageType pageType) { if (pageType != MxPageType::PAGE_2D) return; LAZY_INIT_DIALOG(m_shapeRecognitionDialog, Mx2dShapeRecognitionDialog, std::true_type{}); }
void MxCADViewer::onLookRecognitedShapeList(MxPageType pageType) { if (pageType != MxPageType::PAGE_2D) return; LAZY_INIT_DIALOG(m_shapeListDialog, Mx2dShapeListDialog, std::false_type{}); }
void MxCADViewer::onExtractText(MxPageType pageType) { if (pageType != MxPageType::PAGE_2D) return; LAZY_INIT_DIALOG(m_extractTextDialog, Mx2dExtractTextDialog, std::false_type{}); }
void MxCADViewer::onExtractTable(MxPageType pageType) { if (pageType != MxPageType::PAGE_2D) return; LAZY_INIT_DIALOG(m_extractTableSaveDialog, Mx2dTableSaveDialog, std::true_type{}); }
void MxCADViewer::onPrint(MxPageType pageType) { if (pageType != MxPageType::PAGE_2D) return; LAZY_INIT_DIALOG(m_printHintDialog, Mx2dPrintHintDialog, std::true_type{}); }
void MxCADViewer::onShortcutSettings(MxPageType pageType) { if (pageType != MxPageType::PAGE_2D) return; LAZY_INIT_DIALOG(m_shortcutSettingsDialog, Mx2dShortcutSettingsDialog, std::true_type{}); }
void MxCADViewer::onExportPDF(MxPageType pageType) { if (pageType != MxPageType::PAGE_2D) return; LAZY_INIT_DIALOG(m_exportPdfDialog, Mx2dExportPdfDialog, std::true_type{}); }
void MxCADViewer::onBatchExportPDF(MxPageType pageType) { if (pageType != MxPageType::PAGE_2D) return; LAZY_INIT_DIALOG(m_batchExportPdfDialog, Mx2dBatchExportPdfDialog, std::true_type{}); }
void MxCADViewer::onInsertSingleLineText(MxPageType pageType) { if (pageType != MxPageType::PAGE_2D) return; LAZY_INIT_DIALOG(m_insertSingleLineTextDialog, Mx2dInsertSingleLineTextDialog, std::true_type{}); }
void MxCADViewer::onMultiLineInput(MxPageType pageType) { if (pageType != MxPageType::PAGE_2D) return; LAZY_INIT_DIALOG(m_multiLineInputDialog, Mx2dMultiLineInputDialog, std::true_type{}); }
void MxCADViewer::onNumberedText(MxPageType pageType) { if (pageType != MxPageType::PAGE_2D) return; LAZY_INIT_DIALOG(m_numberedTextDialog, Mx2dNumberedTextDialog, std::true_type{}); }
void MxCADViewer::onPdfToDwg(MxPageType pageType) { if (pageType != MxPageType::PAGE_2D) return; LAZY_INIT_DIALOG(m_pdfToDwgDialog, Mx2dPdfToDwgDialog, std::true_type{}); }
void MxCADViewer::onCADSplitExport(MxPageType pageType) { if (pageType != MxPageType::PAGE_2D) return; LAZY_INIT_DIALOG(m_splitExportDialog, Mx2dSplitExportDialog, std::true_type{}); }
void MxCADViewer::onCalculateSideArea(MxPageType pageType) { if (pageType != MxPageType::PAGE_2D) return; LAZY_INIT_DIALOG(m_calculateSideAreaDialog, Mx2dCalculateSideAreaDialog, std::true_type{}); }
void MxCADViewer::onMoveText(MxPageType) { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_MoveText"); }
void MxCADViewer::onCloneText(MxPageType) { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_CopyText"); }////////////