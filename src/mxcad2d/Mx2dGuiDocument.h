/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QWidget>
#include <QString>
#include <QProcess>
#include "MxCADInclude.h"
#include "Mx2dAnnotationEditor.h"
#include "MxRecentFile.h"
#include "Mx2dView.h"
#ifdef Q_OS_LINUX
class MxOpenGLView;
#endif
class QTabWidget;
class MxViewToolBar;
class QLabel;
class QHBoxLayout;
class QButtonGroup;
class QTimer;

class Mx2dLayerManagerDialog;
class Mx2dMeasurementDialog;
class Mx2dLeaderTextInputDialog;
class Mx2dTextSearchDialog;
class Mx2dExtractTextDialog;

class Mx2dGuiDocument : public QWidget
{
	Q_OBJECT
public:
	Mx2dGuiDocument(QWidget* parent);
	~Mx2dGuiDocument();
public:
	void openFile(const QString& filePath);
	void triggerUpdate();
	bool makeCurrent();
	bool executeCommand(const QString& cmdStr, struct resbuf* pParam = 0);
	int undoCount() const;
	int redoCount() const;

	MxCADView& cadView(){ return m_cadView;}


	void closeAllModelessDialogs();

protected:
	void resizeEvent(QResizeEvent* event) override;
	void paintEvent(QPaintEvent* event) override;
	void wheelEvent(QWheelEvent*) override;
	void mousePressEvent(QMouseEvent*) override;
	void mouseReleaseEvent(QMouseEvent*) override;
	void mouseMoveEvent(QMouseEvent*) override;

	void keyPressEvent(QKeyEvent* event) override;
private:
	MxRecentFile createRecentFile();

private:
	MxCADView		m_cadView;
	Mx2dView*		m_pViewWidget = nullptr;
	MxViewToolBar*	m_p2dViewToolBar = nullptr;
	QLabel*			m_pPromptLabel = nullptr;
	QString			m_filePath;
	QHBoxLayout*	m_spaceBtnLayout = nullptr;
	std::unique_ptr<Mx2dAnnotationEditor> m_pAnnoEditor;
	QStringList m_spaceNames;
	QButtonGroup* m_pSpaceBtnGroup = nullptr;
	QProcess* m_mxConvertProcess;
	QTimer* m_convertTimeoutTimer;
	QString m_convertSrcFilePath;
	QString m_convertOutFilePath;
	QString m_originalOpenFilePath;
signals:
	void transferDone();
private slots:
	void renderShape();
	void mouseEvent(QMouseEvent* theEvent, int action);
	void onShowPromptMessage(QWidget* target,const QString& message);
	void onHidePromptMessage(QWidget* target);
	void onMessageBoxInformation(QWidget* target, const QString& message);

	void onProcessStarted();
	void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
	void onProcessErrorOccurred(QProcess::ProcessError error);
	void onConvertTimeout();
private:
	void computeToolBarPosition();
	void computePromptLabelPosition();
	void connectSignals();
	QStringList getSpaceNames();
	void readMxwebFile(const QString& mxwebPath, const QString& originalFilePath);
public:
	double m_devicePixelRatio;
	bool m_updatePending;
#ifdef Q_OS_LINUX
	bool    m_isOpenGLPollEvents;
	MxOpenGLView* m_pGLView;
#endif
signals:
	void startConvert();
	void startReadFile();
	void fileRead(bool success);
	void startRender();
	void fileReady();
	void runConverterFailed();
	void convertFailed(const QString& error);

	void undoRedoChanged(int nbUndo, int nbRedo);
private:
	Mx2dLayerManagerDialog* m_layerManagerDialog = nullptr;
	Mx2dMeasurementDialog* m_measurementDialog = nullptr;
	Mx2dLeaderTextInputDialog* m_leaderTextInputDialog = nullptr;
	Mx2dTextSearchDialog* m_textSearchDialog = nullptr;
	Mx2dExtractTextDialog* m_extractTextDialog = nullptr;
public:
	void showLayerManagerDialog(QWidget* guiDoc2d);
	void showMeasurementDialog(QWidget* guiDoc2d);
	void showLeaderTextInputDialog(QWidget* guiDoc2d);
	void showTextSearchDialog(QWidget* guiDoc2d);
	void extractText(QWidget* guiDoc2d);
	void extractTable(QWidget* guiDoc2d);
private:
	void onExtractTable(QWidget* guiDoc2d, const McGePoint3d& new_corner1, const McGePoint3d& new_corner2);
};
