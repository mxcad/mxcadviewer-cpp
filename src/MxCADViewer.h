/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QMainWindow>
#include <QMap>
#include <QList>
#include <QAction>
#include <functional>

// Forward declarations to reduce include dependencies in the header file
QT_BEGIN_NAMESPACE
class QDragEnterEvent;
class QDropEvent;
class QTabWidget;
class QToolBar;
class QMenu;
QT_END_NAMESPACE

class MxProgressDialog;
class MxRecentFileWidget;
#ifdef MX_BUILD_3D
class Mx3dSetBgColorDialog;
#endif
class Mx2dDimCategoryManagerDialog;
class Mx2dCompareSelDrawingDialog;
class Mx2dAnnotationSettingDialog;
class Mx2dExtractTextDialog;
class Mx2dTableSaveDialog;
class Mx2dMeasurementStatDialog;
class Mx2dShapeRecognitionDialog;
class Mx2dPrintHintDialog;
class Mx2dShapeListDialog;
class Mx2dShortcutSettingsDialog;
class Mx2dExportPdfDialog;
class Mx2dInsertSingleLineTextDialog;
class Mx2dMultiLineInputDialog;
class Mx2dNumberedTextDialog;
class Mx2dPdfToDwgDialog;
class Mx2dSplitExportDialog;
class Mx2dBatchExportPdfDialog;
class Mx2dCalculateSideAreaDialog;
class Mx2dModifySingleLineTextDialog;
class Mx2dModifyMultiLineTextDialog;
class Mx2dAreaOffsetDialog;
#ifdef MX_BUILD_LOGIN
class MxLoginScene;
#endif

/**
 * @class MxCADViewer
 * @brief The main window for the CAD viewer application.
 *
 * This class manages the main user interface, including the toolbar, tabbed document area,
 * and event handling for file operations like drag-and-drop. It orchestrates the display
 * of different views (recent files, 2D/3D CAD documents).
 */
class MxCADViewer : public QMainWindow
{
    Q_OBJECT

public:
    //! Enum to identify the type of page currently active in a tab.
    enum class MxPageType
    {
        PAGE_2D,
        PAGE_3D,
        PAGE_USER,
        PAGE_RECENT
    };

    //! Type definition for a callback function.
    using FunctionCallBack = std::function<void(MxPageType)>;

    explicit MxCADViewer(QWidget* parent = nullptr);
    ~MxCADViewer() override;

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

public slots:
    void onOpenFile(const QString& filePath);

private:
    // --- Initialization Methods ---
    void setupUi();
    void createActions();
    void createToolBars();
    void createCentralWidget();
    void populateActionLists();
    void connectSignalSlots();
    
    // --- Action List Population Methods ---
    void addActionsToUserToolBar();
    void addActionsToRecentFileToolBar();
    void addActionsTo3dViewToolBar();
    void addActionsTo2dViewToolBar();

    // --- Document Handling ---
    void newDocument(const QString& path);
    void newCADDocument(const QString& path);

    // --- Helper Methods ---
    void createAndAddAction(QList<QAction*>& group, const QString& iconPath, const QString& text, const QString& toolTip, MxPageType pageType, const FunctionCallBack& callback, QMenu* menu = nullptr);
    QAction* createSeparator();
    void setToolButtonPopupMode(QAction* action);
    static void openFolderAndSelectFile(const QString& filePath);

private slots:
    // --- Toolbar Action Handlers ---
    void onOpen();
    void onRecentFile();
    void onAbout();
    void onEnglish();
    void onChinese();
    void restartApp();

    // --- Tab and Window Management ---
    void onTabCloseRequested(int index);
    void onTabChanged(int index);
    void updateToolBar(int index);
    void onCloseGuiDocument(QWidget* widget);
#ifdef MX_BUILD_LOGIN
    void onSetWindowSubTitle(const QString& subTitle);
#endif // MX_BUILD_LOGIN


    // --- File and Document Management ---
    void onAddOpenedFile(QString path, QWidget* guidoc);

    // --- 3D View Specific Slots ---
    void onChangeBgColor();

    // --- 2D View Specific Slots ---
    void onDoNothing(MxPageType pageType);
    void onDimCategoryManager(MxPageType pageType);
    void onDrawingCompare(MxPageType pageType);
    void onDimSetting(MxPageType pageType);
    void onMeasurementStat(MxPageType pageType);
    void onShapeRecognition(MxPageType pageType);
    void onLookRecognitedShapeList(MxPageType pageType);
    void onExtractTable(MxPageType pageType);
    void onPrint(MxPageType pageType);
    void onShortcutSettings(MxPageType pageType);
    void onExportPDF(MxPageType pageType);
    void onBatchExportPDF(MxPageType pageType);
    void onInsertSingleLineText(MxPageType pageType);
    void onMultiLineInput(MxPageType pageType);
    void onNumberedText(MxPageType pageType);
    void onMoveText(MxPageType pageType);
    void onCloneText(MxPageType pageType);
    void onPdfToDwg(MxPageType pageType);
    void onCADSplitExport(MxPageType pageType);
    void onCalculateSideArea(MxPageType pageType);
    void onDoTest(MxPageType pageType);

private:
    // --- Core UI Components ---
    QTabWidget* m_tabWidget;
    QToolBar* m_mainToolBar;
    int m_currentTabIndex;
    // --- Main Actions ---
    QAction* m_openAction;
    QAction* m_closeAction;
#ifdef MX_BUILD_LOGIN
	QAction* m_accountAction;
    QMenu* m_accountMenuIsLogin;
    QMenu* m_accountMenuIsLogout;
#endif
    QAction* m_settingmenuAction;
    QAction* m_aboutAction;
    QAction* m_settingLanguageAction;
    QAction* m_englishAction;
    QAction* m_chineseAction;
    QAction* m_recentFileAction;
    QAction* m_undoAction;
    QAction* m_redoAction;
    QAction* m_layerManagerAction;
    
    // --- Main Scenes/Widgets ---
#ifdef MX_BUILD_LOGIN
	MxLoginScene* m_loginScene;
#endif
    MxRecentFileWidget* m_recentFileWidget;
    MxProgressDialog* m_progressDialog;

    //! Map to track opened files and their corresponding widget tabs.
    QMap<QString, QWidget*> m_mapFileOpened;

    // --- Action Lists for different tab contexts ---
    QList<QAction*> m_tabUserActions;
    QList<QAction*> m_tabRecentFileActions;
    QList<QAction*> m_tab3dViewActions;
    QList<QAction*> m_tab2dViewActions;

    // --- Dialogs (lazily initialized) ---
#ifdef MX_BUILD_3D
    Mx3dSetBgColorDialog* m_setBgColorDialog = nullptr;
#endif
    
    Mx2dDimCategoryManagerDialog*   m_dimCategoryManagerDialog = nullptr;
    Mx2dCompareSelDrawingDialog*    m_compareSelDrawingDialog = nullptr;
    Mx2dAnnotationSettingDialog*    m_annotationSettingDialog = nullptr;
    
    Mx2dTableSaveDialog*            m_extractTableSaveDialog = nullptr;
    Mx2dMeasurementStatDialog*      m_measurementStatDialog = nullptr;
    Mx2dShapeRecognitionDialog*     m_shapeRecognitionDialog = nullptr;
    Mx2dPrintHintDialog*            m_printHintDialog = nullptr;
    Mx2dShapeListDialog*            m_shapeListDialog = nullptr;
    Mx2dShortcutSettingsDialog*     m_shortcutSettingsDialog = nullptr;
    Mx2dExportPdfDialog*            m_exportPdfDialog = nullptr;
    Mx2dInsertSingleLineTextDialog* m_insertSingleLineTextDialog = nullptr;
    Mx2dMultiLineInputDialog*       m_multiLineInputDialog = nullptr;
    Mx2dNumberedTextDialog*         m_numberedTextDialog = nullptr;
    Mx2dPdfToDwgDialog*             m_pdfToDwgDialog = nullptr;
    Mx2dSplitExportDialog*          m_splitExportDialog = nullptr;
    Mx2dBatchExportPdfDialog*       m_batchExportPdfDialog = nullptr;
    Mx2dCalculateSideAreaDialog*    m_calculateSideAreaDialog = nullptr;
    Mx2dModifySingleLineTextDialog* m_modifySingleLineTextDialog = nullptr;
    Mx2dModifyMultiLineTextDialog*  m_modifyMultiLineTextDialog = nullptr;
    Mx2dAreaOffsetDialog*           m_areaOffsetDialog = nullptr;
private:
    QWidget* currentGuiDoc2d();

signals:
    void showLayerManagerDialog(QWidget* guiDoc2d);
    void showMeasurementDialog(QWidget* guiDoc2d);
    void showLeaderTextInputDialog(QWidget* guiDoc2d);
    void showTextSearchDialog(QWidget* guiDoc2d);
    void extractText(QWidget* guiDoc2d);
    void extractTable(QWidget* guiDoc2d);
};