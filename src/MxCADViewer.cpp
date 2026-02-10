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
#ifdef MX_BUILD_LOGIN
#include "mxloginscene.h"
#include "mxpersonalcenterwidget.h"
#include "MxNetworkManager.h"
#endif // MX_BUILD_LOGIN
#include "MxLogger.h"

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
#ifdef MX_BUILD_LOGIN
	setWindowTitle(MxUtils::getAppVersion() + tr("<Guest>"));
#else
	setWindowTitle(MxUtils::getAppVersion());
#endif
	setWindowIcon(QIcon(":/resources/images/mxlogo.svg"));
	setAcceptDrops(true);
	resize(1500, 1200);
	setMinimumSize(1200, 700);

	// --- Create UI Components ---
	createActions();
	createToolBars();
	createCentralWidget();

#ifdef MX_BUILD_LOGIN
	// --- Setup Login Scene ---
	m_loginScene = new MxLoginScene;
	m_currentTabIndex = m_tabWidget->addTab(m_loginScene, tr("Login"));
	connect(m_loginScene, &MxLoginScene::loginSuccess, this, [this](const QString& nickName, bool isVip) {
		m_tabWidget->setTabText(m_tabWidget->indexOf(m_loginScene), tr("Profile"));
		this->setWindowTitle(MxUtils::getAppVersion() + QString("<%1,%2>").arg(nickName).arg(isVip ? tr("VIP") : tr("Not VIP")));
		m_accountAction->setMenu(m_accountMenuIsLogin);
		});
	connect(m_loginScene, &MxLoginScene::logoutSuccess, this, [this]() {
		m_tabWidget->setTabText(m_tabWidget->indexOf(m_loginScene), tr("Login"));
		this->setWindowTitle(MxUtils::getAppVersion() + tr("<Guest>"));
		m_accountAction->setMenu(m_accountMenuIsLogout);
		});
	/*connect(&MxNetworkManager::getInstance(), &MxNetworkManager::changeWindowSubTitle, this, [this](const QString& subTitle) {
		this->setWindowTitle(MxUtils::getAppVersion() + subTitle);
		});*/
	connect(&MxNetworkManager::getInstance(), &MxNetworkManager::notLogin, this, [this]() {
		QMessageBox::information(this, tr("Tip"), tr("Please login to continue"));
		m_loginScene->gotoLoginWidget("userLoginWidget");
		m_tabWidget->setCurrentWidget(m_loginScene);
		});
	connect(&MxNetworkManager::getInstance(), &MxNetworkManager::notVip, this, [this]() {
		QMessageBox::information(this, tr("Tip"), tr("Please upgrade to VIP to continue"));
		MxLoginWidget* loginWidget = m_loginScene->getLoginWidget("personalCenterWidget");
		MxPersonalCenterWidget* personalCenterWidget = dynamic_cast<MxPersonalCenterWidget*>(loginWidget);
		personalCenterWidget->showBuyVipPage();
		m_tabWidget->setCurrentWidget(m_loginScene);
		});

	// Attempt to fetch user info to auto-login
	MxNetworkManager::getInstance().fetchUserInfo([this](bool success, MxUtils::LoginUserInfo userInfo) {
		if (success)
		{
			m_loginScene->gotoLoginWidget("personalCenterWidget"); // Switch to personal center on success	
			m_tabWidget->setTabText(m_tabWidget->indexOf(m_loginScene), tr("Profile"));
			this->setWindowTitle(MxUtils::getAppVersion() + QString("<%1,%2>").arg(userInfo.nickName).arg(userInfo.isVip ? tr("VIP") : tr("Not VIP")));
			m_accountAction->setMenu(m_accountMenuIsLogin);
		}
		});

#endif

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
	m_openAction->setToolTip(tr("Open File"));
	m_openAction->setShortcut(QKeySequence::Open);

	m_recentFileAction = new QAction(QIcon(":/resources/images/recent.svg"), tr("Recent"), this);
	m_recentFileAction->setToolTip(tr("Recent Files"));

#ifdef MX_BUILD_LOGIN

	auto loginAction = new QAction(QIcon(":/resources/images_login/login.svg"), tr("Login"), this);
	auto purchaseVipAction = new QAction(QIcon(":/resources/images_login/purchaseVip.svg"), tr("Purchase VIP"), this);
	auto personalCenterAction = new QAction(QIcon(":/resources/images_login/account.svg"), tr("Profile"), this);
	auto logoutAction = new QAction(QIcon(":/resources/images_login/logout.svg"), tr("Logout"), this);

	connect(loginAction, &QAction::triggered, this, [this]() {
		int idx = m_tabWidget->indexOf(m_loginScene);
		if (idx == -1) {
			m_tabWidget->insertTab(0, m_loginScene, tr("Login"));
		}
		m_loginScene->gotoLoginWidget("userLoginWidget");
		m_tabWidget->setCurrentWidget(m_loginScene);
		});
	connect(purchaseVipAction, &QAction::triggered, this, [this]() {
		int idx = m_tabWidget->indexOf(m_loginScene);
		if (idx == -1) {
			m_tabWidget->insertTab(0, m_loginScene, MxNetworkManager::getInstance().getUserInfo().isLogin ? tr("Profile") : tr("Login"));
		}
		if (!MxNetworkManager::getInstance().getUserInfo().isLogin)
		{
            QMessageBox::information(this, tr("Tip"), tr("Please login to continue"));
            m_loginScene->gotoLoginWidget("userLoginWidget");
            m_tabWidget->setCurrentWidget(m_loginScene);
            return;
		}
		MxLoginWidget* loginWidget = m_loginScene->getLoginWidget("personalCenterWidget");
		MxPersonalCenterWidget* personalCenterWidget = dynamic_cast<MxPersonalCenterWidget*>(loginWidget);
		personalCenterWidget->showBuyVipPage();
		m_tabWidget->setCurrentWidget(m_loginScene);
		});
	connect(personalCenterAction, &QAction::triggered, this, [this]() {
		int idx = m_tabWidget->indexOf(m_loginScene);
		if (idx == -1) {
			m_tabWidget->insertTab(0, m_loginScene, tr("Profile"));
		}
		MxLoginWidget* loginWidget = m_loginScene->getLoginWidget("personalCenterWidget");
		MxPersonalCenterWidget* personalCenterWidget = dynamic_cast<MxPersonalCenterWidget*>(loginWidget);
		personalCenterWidget->showUserInfoPage();
		m_tabWidget->setCurrentWidget(m_loginScene);
		});
	connect(logoutAction, &QAction::triggered, this, [this]() {
		int idx = m_tabWidget->indexOf(m_loginScene);
		if (idx == -1) {
			m_tabWidget->insertTab(0, m_loginScene, tr("Login"));
		}
		MxLoginWidget* loginWidget = m_loginScene->getLoginWidget("personalCenterWidget");
		MxPersonalCenterWidget* personalCenterWidget = dynamic_cast<MxPersonalCenterWidget*>(loginWidget);
		personalCenterWidget->logout();
		m_tabWidget->setCurrentWidget(m_loginScene);
		});
	m_accountMenuIsLogin = new QMenu(this);
	m_accountMenuIsLogin->addAction(personalCenterAction);
	m_accountMenuIsLogin->addAction(purchaseVipAction);
	m_accountMenuIsLogin->addAction(logoutAction);
	
    

	m_accountMenuIsLogout = new QMenu(this);
	m_accountMenuIsLogout->addAction(loginAction);
	m_accountMenuIsLogout->addAction(purchaseVipAction);

	m_accountAction = new QAction(QIcon(":/resources/images_login/account.svg"), tr("Account"), this);
	m_accountAction->setMenu(m_accountMenuIsLogout);
	
#endif // MX_BUILD_LOGIN


	m_settingmenuAction = new QAction(QIcon(":/resources/images3d/setting.svg"), tr("Setting"), this);
	m_settingmenuAction->setToolTip(tr("Setting"));

	m_aboutAction = new QAction(QIcon(":/resources/images/about.svg"), tr("About"), this);
	m_aboutAction->setToolTip(tr("About MxCADViewer"));

	m_settingLanguageAction = new QAction(QIcon(":/resources/images/language.svg"), tr("Language"), this);
    m_settingLanguageAction->setToolTip(tr("Language Settings"));

	m_englishAction = new QAction(tr("English"), this);
	m_englishAction->setToolTip(tr("English"));
	m_chineseAction = new QAction(tr("Simplified Chinese"), this);
	m_chineseAction->setToolTip(tr("Simplified Chinese"));

	auto languageMenu = new QMenu(this);
    languageMenu->addAction(m_englishAction);
    languageMenu->addAction(m_chineseAction);
    m_settingLanguageAction->setMenu(languageMenu);

	m_closeAction = new QAction(QIcon(":/resources/images/close.svg"), tr("Exit"), this);
	m_closeAction->setToolTip(tr("Exit App"));
}

void MxCADViewer::createToolBars()
{
	m_mainToolBar = new QToolBar(this);
	m_mainToolBar->setFloatable(false);
	m_mainToolBar->setMovable(false);
	m_mainToolBar->setIconSize(QSize(32, 32));
	m_mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	m_mainToolBar->setContextMenuPolicy(Qt::PreventContextMenu);
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

#ifdef MX_BUILD_LOGIN
	m_tabUserActions.append(createSeparator());
	m_tabUserActions.append(m_accountAction);
#endif

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
#ifdef MX_BUILD_LOGIN
	m_tabRecentFileActions.append(createSeparator());
	m_tabRecentFileActions.append(m_accountAction);
#endif
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
#ifdef MX_BUILD_LOGIN
	m_tab3dViewActions.append(m_accountAction);
#endif
	auto* changeBgColorAction = new QAction(QIcon(":/resources/images3d/changeBgColor.svg"), tr("Change Background Color"), this);
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
#ifdef MX_BUILD_LOGIN
	m_tab2dViewActions.append(createSeparator());
	m_tab2dViewActions.append(m_accountAction);
#endif
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
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_window.svg", tr("Zoom"), tr("Zoom viewport"), MxPageType::PAGE_2D, [this](MxPageType type) { onDoNothing(type); }, pWinMenu);

	// --- Basic 2D Actions ---
	m_layerManagerAction = new QAction(QIcon(":/resources/images2d/2d_layer.svg"), tr("Layer Manager"), this);
	m_layerManagerAction->setToolTip(tr("Open layer manager"));
	m_tab2dViewActions.append(m_layerManagerAction);
	
#if 0
	connect(m_layerManagerAction, &QAction::triggered, this, [this]() { onLayerManager(MxPageType::PAGE_2D); });
#else
	connect(m_layerManagerAction, &QAction::triggered, this, [this]() { emit showLayerManagerDialog(currentGuiDoc2d()); });
#endif

	m_undoAction = new QAction(QIcon(":/resources/images2d/2d_undo.svg"), tr("Undo"), this);
	m_undoAction->setEnabled(false);
	m_undoAction->setShortcut(QKeySequence::Undo);
	m_tab2dViewActions.append(m_undoAction);
    connect(m_undoAction, &QAction::triggered, this, [this]() { emit Mx2dSignalTransfer::getInstance().signalUndo(MxUtils::gCurrentTab); });

	m_redoAction = new QAction(QIcon(":/resources/images2d/2d_redo.svg"), tr("Redo"), this);
    m_redoAction->setEnabled(false);
    m_redoAction->setShortcut(QKeySequence::Redo);
	m_tab2dViewActions.append(m_redoAction);
    connect(m_redoAction, &QAction::triggered, this, [this]() { emit Mx2dSignalTransfer::getInstance().signalRedo(MxUtils::gCurrentTab); });

	
	

	// --- VIP Menu (fully populated) ---
	auto* pVipMenu = new QMenu(this);
	auto* pActExtractText = new QAction(QIcon(":/resources/images2d/2d_extractText.svg"), tr("Extract Text"), this);
	connect(pActExtractText, &QAction::triggered, this, [this]() { 
		MxUtils::doAction([this]() { 
			emit extractText(currentGuiDoc2d());
			//Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_ExtractTextRect");
			/*if (m_extractTextDialog && m_extractTextDialog->isVisible())
			{
				m_extractTextDialog->accept(); 
				Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_ExtractTextRect");
			}*/
			}); 
		});
	auto* pActExtractTable = new QAction(QIcon(":/resources/images2d/2d_extractTable.svg"), tr("Extract Table"), this);
	connect(pActExtractTable, &QAction::triggered, this, [this]() { MxUtils::doAction([this]() { emit extractTable(currentGuiDoc2d()); }); });

	auto* pActArcLength = new QAction(QIcon(":/resources/images2d/2d_arcLength.svg"), tr("Arc Length"), this);
	connect(pActArcLength, &QAction::triggered, this, [this]() { MxUtils::doAction([this]() { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_DrawArcLengthDimMark"); }); });

	auto* pActPt2LineDist = new QAction(QIcon(":/resources/images2d/2d_pt2lineDist.svg"), tr("Point to Line Distance"), this);
	connect(pActPt2LineDist, &QAction::triggered, this, [this]() { MxUtils::doAction([this]() { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_DrawDistPointToLineMark"); }); });

	auto* pActContinuousMeasurement = new QAction(QIcon(":/resources/images2d/2d_continuousMeasurement.svg"), tr("Continuous Measurement"), this);
    connect(pActContinuousMeasurement, &QAction::triggered, this, [this]() { MxUtils::doAction([this]() { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_DrawContinuousMeasurementMark"); }); });

	auto* pActBatchMeasurement = new QAction(QIcon(":/resources/images2d/2d_batchMeasurement.svg"), tr("Batch Measurement"), this);
    connect(pActBatchMeasurement, &QAction::triggered, this, [this]() { MxUtils::doAction([this]() { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_BatchMeasure"); }); });

	auto* pActViewSegmentLength = new QAction(QIcon(":/resources/images2d/2d_segLength.svg"), tr("Show Segment Length"), this);
    connect(pActViewSegmentLength, &QAction::triggered, this, [this]() { MxUtils::doAction([this]() { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_ShowSegmentLengths"); }); });

	auto* pActAreaWithArc = new QAction(QIcon(":/resources/images2d/2d_areaWithArc.svg"), tr("Area(with arcs)"), this);
    connect(pActAreaWithArc, &QAction::triggered, this, [this]() { MxUtils::doAction([this]() { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_DrawArcPolyAreaMark"); }); });

	auto* pActMeasureFillArea = new QAction(QIcon(":/resources/images2d/2d_fillArea.svg"), tr("Hatch Area"), this);
    connect(pActMeasureFillArea, &QAction::triggered, this, [this]() { MxUtils::doAction([this]() { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_DrawHatchArea2Mark"); }); });

	auto* pActCalculateSideArea = new QAction(QIcon(":/resources/images2d/2d_sideArea.svg"), tr("Calculate Side Area"), this);
    connect(pActCalculateSideArea, &QAction::triggered, this, [this]() { MxUtils::doAction([this]() { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_CalculateSiderArea"); }); });

	auto* pActAreaOffset = new QAction(QIcon(":/resources/images2d/2d_areaOffset.svg"), tr("Area Offset"), this);
    connect(pActAreaOffset, &QAction::triggered, this, [this]() { MxUtils::doAction([this]() { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_SelectAreaToOffset"); }); });

	auto* pActMeasureCircle = new QAction(QIcon(":/resources/images2d/2d_measureCircle.svg"), tr("Circle"), this);
    connect(pActMeasureCircle, &QAction::triggered, this, [this]() { MxUtils::doAction([this]() { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_DrawCircleMeasurementMark"); }); });

	auto* pActMeasureAngle = new QAction(QIcon(":/resources/images2d/2d_measureAngle.svg"), tr("Angle"), this);
    connect(pActMeasureAngle, &QAction::triggered, this, [this]() { MxUtils::doAction([this]() { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_DrawAngleMeasurementMark"); }); });

	pVipMenu->addAction(pActExtractText);
	pVipMenu->addAction(pActExtractTable);
	pVipMenu->addAction(pActArcLength);
	pVipMenu->addAction(pActPt2LineDist);
	pVipMenu->addAction(pActContinuousMeasurement);
	pVipMenu->addAction(pActBatchMeasurement);
	pVipMenu->addAction(pActViewSegmentLength);
	pVipMenu->addAction(pActAreaWithArc);
	pVipMenu->addAction(pActMeasureFillArea);
	pVipMenu->addAction(pActCalculateSideArea);
	pVipMenu->addAction(pActAreaOffset);
	pVipMenu->addAction(pActMeasureCircle);
	pVipMenu->addAction(pActMeasureAngle);
#ifdef MX_DEVELOPING
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
	
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_vip.svg", tr("VIP"), tr("VIP functions"), MxPageType::PAGE_2D, [this](MxPageType type) { onDoNothing(type); }, pVipMenu);


	auto* pActMeasurement = new QAction(QIcon(":/resources/images2d/2d_measure.svg"), tr("Measure"), this);
	pActMeasurement->setToolTip(tr("Measure tools"));
	m_tab2dViewActions.append(pActMeasurement);
    connect(pActMeasurement, &QAction::triggered, this, [this]() { emit showMeasurementDialog(currentGuiDoc2d()); });

#ifdef MX_DEVELOPING
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_measureStatistics.svg", tr("Measure Stats"), tr("Statistics of measurement data"), MxPageType::PAGE_2D, [this](MxPageType type) { onMeasurementStat(type); });
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_compare.svg", tr("Drawing Compare"), tr("Compare drawing differences"), MxPageType::PAGE_2D, [this](MxPageType type) { onDrawingCompare(type); });
	// --- Shape Recognition Menu ---
	auto* pShapeRecognitionMenu = new QMenu(this);
	auto* pActShapeRecognition = new QAction(QIcon(":/resources/images2d/2d_graphicRecognition.svg"), tr("Entity Recognition"), this);
	connect(pActShapeRecognition, &QAction::triggered, this, [this]() { onShapeRecognition(MxPageType::PAGE_2D); });
	auto* pActRecognitedList = new QAction(tr("View Detected Entities"), this);
	connect(pActRecognitedList, &QAction::triggered, this, [this]() { onLookRecognitedShapeList(MxPageType::PAGE_2D); });
	pShapeRecognitionMenu->addAction(pActShapeRecognition);
	pShapeRecognitionMenu->addAction(pActRecognitedList);
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_graphicRecognition.svg", tr("Entity Recognition"), tr("Detect CAD drawing entities"), MxPageType::PAGE_2D, [this](MxPageType type) { onDoNothing(type); }, pShapeRecognitionMenu);
#endif
	

	// --- Text Menu ---
	auto* pTextMenu = new QMenu(this);
	auto* pActSingleLineText = new QAction(QIcon(":/resources/images2d/2d_singleLineText.svg"), tr("Single Line Text"), this);
	connect(pActSingleLineText, &QAction::triggered, this, [this]() { onInsertSingleLineText(MxPageType::PAGE_2D); });
	auto* pActMultLinesText = new QAction(QIcon(":/resources/images2d/2d_multiLineText.svg"), tr("Multi Line Text"), this);
	connect(pActMultLinesText, &QAction::triggered, this, [this]() { MxUtils::doAction([this]() { onMultiLineInput(MxPageType::PAGE_2D); }); });
	auto* pActNumberText = new QAction(QIcon(":/resources/images2d/2d_numberedText.svg"), tr("Numbered Text"), this);
	connect(pActNumberText, &QAction::triggered, this, [this]() { MxUtils::doAction([this]() {onNumberedText(MxPageType::PAGE_2D); }); });
	auto* pActModText = new QAction(QIcon(":/resources/images2d/2d_modifyText.svg"), tr("Modify Text"), this);
	connect(pActModText, &QAction::triggered, this, []() { MxUtils::doAction([]() { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_SelectTextToModify"); }); });
	auto* pActMoveText = new QAction(QIcon(":/resources/images2d/2d_moveText.svg"), tr("Move Text"), this);
	connect(pActMoveText, &QAction::triggered, this, [this](bool) { MxUtils::doAction([this]() {onMoveText(MxPageType::PAGE_2D); }); });
	auto* pActCloneText = new QAction(QIcon(":/resources/images2d/2d_copyText.svg"), tr("Copy Text"), this);
	connect(pActCloneText, &QAction::triggered, this, [this](bool) { MxUtils::doAction([this]() {onCloneText(MxPageType::PAGE_2D); }); });
	pTextMenu->addAction(pActSingleLineText); 
	pTextMenu->addAction(pActMultLinesText); 
	pTextMenu->addAction(pActNumberText);
	pTextMenu->addAction(pActModText); 
	pTextMenu->addAction(pActMoveText); 
	pTextMenu->addAction(pActCloneText);
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_text.svg", tr("Text"), tr("Create or modify text"), MxPageType::PAGE_2D, [this](MxPageType type) { onDoNothing(type); }, pTextMenu);

	// --- Draw Line ---
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_line.svg", tr("Draw Line"), tr("Draw Line"), MxPageType::PAGE_2D, [](MxPageType) { MxUtils::doAction([]() {Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_DrawLine"); }); });

	// --- Shapes Menu ---
	auto* pShapeMenu = new QMenu(this);
	auto* pActRectangle = new QAction(QIcon(":/resources/images2d/2d_dimRectangle.svg"), tr("Rectangle"), this);
	connect(pActRectangle, &QAction::triggered, this, []() { MxUtils::doAction([]() {Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_DrawRectMark"); }); });
	auto* pActEllipse = new QAction(QIcon(":/resources/images2d/2d_dimEllipse.svg"), tr("Ellipse"), this);
	connect(pActEllipse, &QAction::triggered, this, []() { MxUtils::doAction([]() {Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_DrawEllipseMark"); }); });
	auto* pActCloudLine = new QAction(QIcon(":/resources/images2d/2d_dimCloud.svg"), tr("Cloud"), this);
	connect(pActCloudLine, &QAction::triggered, this, []() { MxUtils::doAction([]() {Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_DrawRectCloudMark"); }); });
	auto* pActLeadLine = new QAction(QIcon(":/resources/images2d/2d_dimLeader.svg"), tr("Leader"), this);
	connect(pActLeadLine, &QAction::triggered, this, [this]() { 
		MxUtils::doAction([this]() {

			emit showLeaderTextInputDialog(currentGuiDoc2d());
			});
		});
	pShapeMenu->addAction(pActRectangle); pShapeMenu->addAction(pActEllipse); pShapeMenu->addAction(pActCloudLine); pShapeMenu->addAction(pActLeadLine);
#ifdef MX_DEVELOPING
	pShapeMenu->addAction(new QAction(QIcon(":/resources/images2d/2d_dimPic.svg"), tr("Picture"), this));
	pShapeMenu->addAction(new QAction(QIcon(":/resources/images2d/2d_dimFreeDraw.svg"), tr("Freehand"), this));
#endif // MX_DEVELOPING
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_shapes.svg", tr("Shapes"), tr("Shapes"), MxPageType::PAGE_2D, [this](MxPageType type) { onDoNothing(type); }, pShapeMenu);

	// --- Delete Menu ---
	auto* pDeleteMenu = new QMenu(this);
	auto* pActDelDim = new QAction(QIcon(":/resources/images2d/2d_dimAnnotations.svg"), tr("Delete Annotation"), this);
	connect(pActDelDim, &QAction::triggered, this, []() { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_EraseAnnotation"); });
	auto* pActDelAllDim = new QAction(QIcon(":/resources/images2d/2d_dimAllAnnotations.svg"), tr("Delete All Annotations"), this);
	connect(pActDelAllDim, &QAction::triggered, this, [this]() {
		if (QMessageBox::question(this, tr("Confirm"), tr("Are you sure you want to delete all annotations?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
			Mx2dSignalTransfer::getInstance().signalEraseAllAnnotations(MxUtils::gCurrentTab);
		}
		});
	pDeleteMenu->addAction(pActDelDim); pDeleteMenu->addAction(pActDelAllDim);
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_delete.svg", tr("Delete"), tr("Delete one or more annotations"), MxPageType::PAGE_2D, [this](MxPageType type) { onDoNothing(type); }, pDeleteMenu);

	// --- Hide/Show Annotation ---
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_hideDim.svg", tr("Hide Annotations"), tr("Hide or show all annotations"), MxPageType::PAGE_2D, [this](MxPageType) {
		static bool isShow = false;
		//MrxDbgRbList spParam = Mx::mcutBuildList(RTSHORT, isShow ? 1 : 0, 0);
		//Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_ShowHideAnnotation", spParam.orphanData());
		emit Mx2dSignalTransfer::getInstance().signalShowAllAnnotations(MxUtils::gCurrentTab, isShow);
		isShow = !isShow;
		});

#ifdef MX_DEVELOPING
	// --- Import/Export Menu ---
	auto* pImportExportMenu = new QMenu(this);
	pImportExportMenu->addAction(new QAction(tr("Import Drawing and Annotations"), this));
	pImportExportMenu->addAction(new QAction(tr("Export Drawing and Annotations"), this));
	pImportExportMenu->addAction(pActExportPDF);       // Reuse existing action
	pImportExportMenu->addAction(pActBatchExportPDF);  // Reuse existing action

	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_inputOutput.svg", tr("Import/Export"), tr("Import/Export Annotations or PDF"), MxPageType::PAGE_2D, [this](MxPageType type) { onDoNothing(type); }, pImportExportMenu);

	// --- More Tools ---
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_dimSetting.svg", tr("Annotation Settings"), tr("Annotation settings"), MxPageType::PAGE_2D, [this](MxPageType type) { onDimSetting(type); });
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_proportion.svg", tr("Ratio"), tr("Annotation ratio setting"), MxPageType::PAGE_2D, [](MxPageType) { /* TODO: Implement scale logic */ });
#endif // MX_DEVELOPING

	auto* pActTextSearch = new QAction(QIcon(":/resources/images2d/2d_textSearch.svg"), tr("Text Search"), this);
	pActTextSearch->setToolTip(tr("Locate text in drawings or annotations"));
    m_tab2dViewActions.append(pActTextSearch);
    connect(pActTextSearch, &QAction::triggered, this, [this]() {
		emit showTextSearchDialog(currentGuiDoc2d());
		});

	//createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_test.svg", tr("Test"), tr("Test"), MxPageType::PAGE_2D, [this](MxPageType type) { Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_TestThread"); });

#ifdef MX_DEVELOPING
	// --- Screen Rotation Menu ---
	auto* pScreenRotMenu = new QMenu(this);
	pScreenRotMenu->addAction(new QAction(tr("Rotate Clockwise 90 Degree"), this));
	pScreenRotMenu->addAction(new QAction(tr("Rotate Counter-Clockwise 90 Degree"), this));
	pScreenRotMenu->addAction(new QAction(tr("Custom Rotation"), this));
	pScreenRotMenu->addSeparator();
	pScreenRotMenu->addAction(new QAction(tr("Restore Initial View"), this));
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_screenRotate.svg", tr("Rotate View"), tr("Rotate viewport"), MxPageType::PAGE_2D, [this](MxPageType type) { onDoNothing(type); }, pScreenRotMenu);

	// --- Final Tools ---
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_print.svg", tr("Print"), tr("Print"), MxPageType::PAGE_2D, [this](MxPageType type) { onPrint(type); });
	createAndAddAction(m_tab2dViewActions, ":/resources/images2d/2d_dimClassify.svg", tr("Annotation Categories"), tr("Open annotation categories manager"), MxPageType::PAGE_2D, [this](MxPageType type) { onDimCategoryManager(type); });
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
	/*connect(&Mx2dSignalTransfer::getInstance(), &Mx2dSignalTransfer::signalExtractTextFinished, this, [this](QWidget* tab, const QStringList& textList) {
		Q_UNUSED(tab); onExtractText(MxPageType::PAGE_2D); if (m_extractTextDialog) m_extractTextDialog->setTexts(textList);
		});*/
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
#ifdef MX_BUILD_LOGIN
	int index = m_tabWidget->indexOf(m_loginScene);
#else
	int index = -1;
#endif

	m_tabWidget->insertTab(index + 1, m_recentFileWidget, tr("Recent Files"));
	m_tabWidget->setCurrentIndex(index + 1);
}


void MxCADViewer::onAbout()
{
	MxAboutMeDialog aboutDialog(this);
	aboutDialog.exec();
}

void MxCADViewer::onEnglish()
{
	if (MxLanguageManager::getInstance().getCurrentLanguage() == "en_US")
	{
		QMessageBox::information(this, tr("Tip"), tr("The current language is English. No changes are needed."));
        return;
	}
	MxLanguageManager::getInstance().setLanguage("en_US");
	QMessageBox::StandardButton result = QMessageBox::question(this, tr("Tip"), tr("Language settings updated successfully. The changes will take effect after restarting the application. Do you want to restart the application immediately?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (result == QMessageBox::Yes)
	{
		restartApp();
	}
}

void MxCADViewer::onChinese()
{
    if (MxLanguageManager::getInstance().getCurrentLanguage() == "zh_CN")
	{
		QMessageBox::information(this, tr("Tip"), tr("The current language is Chinese. No changes are needed."));
        return;
	}
    MxLanguageManager::getInstance().setLanguage("zh_CN");
	QMessageBox::StandardButton result = QMessageBox::question(this, tr("Tip"), tr("Language settings updated successfully. The changes will take effect after restarting the application. Do you want to restart the application immediately?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (result == QMessageBox::Yes)
	{
		restartApp();
	}
}

void MxCADViewer::restartApp()
{
	QString appPath = QCoreApplication::applicationFilePath();
	QStringList appArgs = QCoreApplication::arguments();
	appArgs.removeFirst();
	bool isNewProcessStarted = QProcess::startDetached(appPath, appArgs);
	if (isNewProcessStarted)
	{
		QCoreApplication::quit();
	}
	else
	{
		QMessageBox::critical(this, tr("Error"), tr("Failed to start new application process. Restart aborted."));
	}
}

void MxCADViewer::onTabChanged(int index)
{
	MxDrawApp::StopAllCommand();
	emit MxSignalTransfer::getInstance().signalHidePromptMessage(MxUtils::gCurrentTab);

	QWidget* prevWidget = m_tabWidget->widget(m_currentTabIndex);
	if (prevWidget)
	{
		if (auto* pPrevMxCAD = qobject_cast<Mx2dGuiDocument*>(prevWidget))
		{
			pPrevMxCAD->closeAllModelessDialogs();
			
		}
	}
	m_currentTabIndex = index;

	QWidget* widget = m_tabWidget->widget(index);
	MxUtils::gCurrentTab = widget;
	if (!widget) return;
	if (auto* pMxCAD = qobject_cast<Mx2dGuiDocument*>(widget)) {
		pMxCAD->triggerUpdate();
		pMxCAD->makeCurrent();
		int nbUndo = pMxCAD->undoCount();
		int nbRedo = pMxCAD->redoCount();
		m_undoAction->setEnabled(nbUndo > 0);
		m_redoAction->setEnabled(nbRedo > 0);
	}
	else if(auto* pRecent = qobject_cast<MxRecentFileWidget*>(widget)) {
		pRecent->updateLayout();
	}
}

void MxCADViewer::onTabCloseRequested(int index)
{
	QWidget* widget = m_tabWidget->widget(index);
	if (!widget) return;

	// For special tabs, just remove them from view
	if (widget == m_recentFileWidget
#ifdef MX_BUILD_LOGIN
        || widget == m_loginScene
#endif
		) {
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
	LOG_INFO(QString("Opened file:  %1").arg(path));
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

void MxCADViewer::createAndAddAction(QList<QAction*>& group, const QString& iconPath, const QString& text, const QString& toolTip, MxPageType pageType, const FunctionCallBack& callback, QMenu* menu)
{
	auto* action = new QAction(QIcon(iconPath), text, this);
	action->setToolTip(toolTip);
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
	connect(guiDoc, &Mx2dGuiDocument::startConvert, this, [this, path]() {
		m_progressDialog->start();
		m_progressDialog->setProgressPercent(20);
		m_progressDialog->setProgressDetail(tr("Convert File: ") + path);
		QApplication::processEvents();
		});
	connect(guiDoc, &Mx2dGuiDocument::startReadFile, this, [this, path]() {
		m_progressDialog->start();
		m_progressDialog->setProgressPercent(0);
		m_progressDialog->setProgressDetail(tr("Reading: ") + path);
		QApplication::processEvents();
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
		QApplication::processEvents();
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
	connect(guiDoc, &Mx2dGuiDocument::undoRedoChanged, this, [this](int nbUndo, int nbRedo) {
		m_undoAction->setEnabled(nbUndo > 0);
        m_redoAction->setEnabled(nbRedo > 0);
		});

	connect(this, &MxCADViewer::showLayerManagerDialog, guiDoc, &Mx2dGuiDocument::showLayerManagerDialog);
	connect(this, &MxCADViewer::showMeasurementDialog, guiDoc, &Mx2dGuiDocument::showMeasurementDialog);
	connect(this, &MxCADViewer::showLeaderTextInputDialog, guiDoc, &Mx2dGuiDocument::showLeaderTextInputDialog);
	connect(this, &MxCADViewer::showTextSearchDialog, guiDoc, &Mx2dGuiDocument::showTextSearchDialog);
	connect(this, &MxCADViewer::extractText, guiDoc, &Mx2dGuiDocument::extractText);
	connect(this, &MxCADViewer::extractTable, guiDoc, &Mx2dGuiDocument::extractTable);

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

#ifdef MX_BUILD_LOGIN
void MxCADViewer::onSetWindowSubTitle(const QString& subTitle)
{
	setWindowTitle(MxUtils::getAppVersion() + subTitle);
}
#endif // MX_BUILD_LOGIN


void MxCADViewer::onChangeBgColor() { 
#ifdef MX_BUILD_3D
	LAZY_INIT_DIALOG(m_setBgColorDialog, Mx3dSetBgColorDialog, std::false_type{});
#endif
	
}
void MxCADViewer::onDoNothing(MxPageType) {}
void MxCADViewer::onDoTest(MxPageType) {

}

QWidget* MxCADViewer::currentGuiDoc2d()
{
	QWidget* w = m_tabWidget->currentWidget();
    if (w && w->inherits("Mx2dGuiDocument"))
		return w;
	return nullptr;
}


void MxCADViewer::onDimCategoryManager(MxPageType pageType) { if (pageType != MxPageType::PAGE_2D) return; LAZY_INIT_DIALOG(m_dimCategoryManagerDialog, Mx2dDimCategoryManagerDialog, std::true_type{}); }
void MxCADViewer::onDrawingCompare(MxPageType pageType) { if (pageType != MxPageType::PAGE_2D) return; LAZY_INIT_DIALOG(m_compareSelDrawingDialog, Mx2dCompareSelDrawingDialog, std::true_type{}); }
void MxCADViewer::onDimSetting(MxPageType pageType) { if (pageType != MxPageType::PAGE_2D) return; LAZY_INIT_DIALOG(m_annotationSettingDialog, Mx2dAnnotationSettingDialog, std::true_type{}); }
void MxCADViewer::onMeasurementStat(MxPageType pageType) { if (pageType != MxPageType::PAGE_2D) return; LAZY_INIT_DIALOG(m_measurementStatDialog, Mx2dMeasurementStatDialog, std::false_type{}); }
void MxCADViewer::onShapeRecognition(MxPageType pageType) { if (pageType != MxPageType::PAGE_2D) return; LAZY_INIT_DIALOG(m_shapeRecognitionDialog, Mx2dShapeRecognitionDialog, std::true_type{}); }
void MxCADViewer::onLookRecognitedShapeList(MxPageType pageType) { if (pageType != MxPageType::PAGE_2D) return; LAZY_INIT_DIALOG(m_shapeListDialog, Mx2dShapeListDialog, std::false_type{}); }
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