/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dGuiDocument.h"
#include "MxRecentFileManager.h"
#include <QFileInfo>
#include "Mx2dSignalTransfer.h"
#include <QSplitter>
#include "MxViewToolBar.h"
#include <QMenu>
#include <QHBoxLayout>
#include <QResizeEvent>
#include <QDebug>
#include <QtGui/qscreen.h>


#include <QTimer>
#include "MxOpenGLView.h"
#include <QTableWidget>
#include <QLabel>
#include <QComboBox>
#include <QMessageBox>
#include "MxSignalTransfer.h"
#include <QCoreApplication>
#include "MxUtils.h"
#include <QtConcurrent>
#include "MxLogger.h"
#include <QPushButton>
#include <QButtonGroup>
#include <QJsonDocument>
#include <QFileDialog>

#include "Mx2dLayerManagerDialog.h"
#include "Mx2dMeasurementDialog.h"
#include "Mx2dLeaderTextInputDialog.h"
#include "Mx2dTextSearchDialog.h"
#include "Mx2dExtractTextDialog.h"
#include "xlsxwriter.h"

Mx2dGuiDocument::Mx2dGuiDocument(QWidget* parent) 
	: QWidget(parent) 
	, m_mxConvertProcess(new QProcess(this))
	, m_convertTimeoutTimer(new QTimer(this))
{
	m_pViewWidget = new Mx2dView(this);
	m_pViewWidget->setStyleSheet("QTabWidget {border: none;}");
	

	m_p2dViewToolBar = new MxViewToolBar(this);
	m_p2dViewToolBar->setMaximumWidth(150);
#ifdef MX_DEVELOPING
	QAction* actDimClassify = new QAction(QIcon(":/resources/images2d/2d_dimClassify.svg"), tr("Annotation Category Management"), this);
	m_p2dViewToolBar->addAction(actDimClassify);
#endif
	QLabel* pLabel = new QLabel(tr("Annotation Category"), this);
	pLabel->setContentsMargins(10, 0, 10, 0);

	m_p2dViewToolBar->addWidget(pLabel);

	QComboBox* pComboBox = new QComboBox(this);
	pComboBox->setStyleSheet(R"(
		QComboBox {
			border: 1px solid gray;
			padding: 1px 18px 1px 3px;
			min-width: 6em;
			combobox-popup: 0;
		}
		QComboBox::drop-down {
			subcontrol-origin: padding;
			subcontrol-position: top right;
			width: 25px;
			border-left-width: 1px;
		}
		QComboBox::down-arrow {
			image: url(:/resources/images2d/2d_dropdown.svg);
		}
	)");
	pComboBox->setFixedHeight(40);
	pComboBox->addItem(tr("default"));
	m_p2dViewToolBar->addWidget(pComboBox);
#ifdef MX_DEVELOPING
	QAction* actDimModify = new QAction(QIcon(":/resources/images2d/2d_modifyDim.svg"), tr("Select one or more annotations to modify properties in batch"), this);
	m_p2dViewToolBar->addAction(actDimModify);
#endif

	QAction* actDimMove = new QAction(QIcon(":/resources/images2d/2d_moveDim.svg"), tr("Move Annotation"), this);
	m_p2dViewToolBar->addAction(actDimMove);
	connect(actDimMove, &QAction::triggered, [this]() {
		Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_MoveAnnotation");
		});

	QAction* actDimCopy = new QAction(QIcon(":/resources/images2d/2d_copyDim.svg"), tr("Copy Annotation"), this);
	m_p2dViewToolBar->addAction(actDimCopy);
	connect(actDimCopy, &QAction::triggered, [this]() {
		Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_CopyAnnotation");
		});
#ifdef MX_DEVELOPING
	QAction* actDimPaste = new QAction(QIcon(":/resources/images2d/2d_pasteDim.svg"), tr("Paste Annotation"), this);
	m_p2dViewToolBar->addAction(actDimPaste);
#endif

	computeToolBarPosition();

	// prompt label
	m_pPromptLabel = new QLabel(this);
	m_pPromptLabel->setAttribute(Qt::WA_DeleteOnClose);
	m_pPromptLabel->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	m_pPromptLabel->setStyleSheet(
		"background-color: rgb(70, 90,125);"
		"color: white;"
		"padding: 10px;"
	);
	m_pPromptLabel->hide();
	computePromptLabelPosition();



#ifdef Q_OS_WIN
	m_cadView.Create(1000, 1000, (HWND)(m_pViewWidget->winId()), true, 100, 100);
#else
	MxOpenGLView* pGLView = new MxOpenGLView();
	m_pGLView = pGLView;
	pGLView->m_hWnd = (HWND)(m_pModelWidget->winId());
	m_cadView.Create(1000, 1000, 0, true, 100, 100, 0, pGLView);


	setMouseTracking(true);
	m_pTabWidget->setMouseTracking(true);
	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_NativeWindow);
	setBackgroundRole(QPalette::NoRole);
	setFocusPolicy(Qt::StrongFocus);
#endif

	m_updatePending = false;
	m_devicePixelRatio = 1.0;
	if (MxUtils::isSpecialSystem("uos"))
	{
		QScreen* aScreen = parent->window()->screen();
		m_devicePixelRatio = aScreen->devicePixelRatio();
	}

#ifdef Q_OS_LINUX
	m_isOpenGLPollEvents = false;
#endif

	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(m_pViewWidget);
	mainLayout->setContentsMargins(0, 0, 0, 9);

	m_spaceBtnLayout = new QHBoxLayout();
	m_spaceBtnLayout->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
	m_spaceBtnLayout->setSpacing(6);

	mainLayout->addLayout(m_spaceBtnLayout);
	mainLayout->setStretch(0, 1);
	mainLayout->setStretch(1, 0);
	mainLayout->setSpacing(0);
	setLayout(mainLayout);

	m_pSpaceBtnGroup = new QButtonGroup(this);

	// Create timer
	QTimer* timer = new QTimer(this);
	QObject::connect(timer, &QTimer::timeout, []() {
		MXAPP.CallMainIdeaTimer();
		});

	timer->start(50);
	m_pAnnoEditor = std::make_unique<Mx2dAnnotationEditor>();

	connectSignals();
}


Mx2dGuiDocument::~Mx2dGuiDocument() {
	m_cadView.Free();
	if (m_mxConvertProcess->state() != QProcess::NotRunning) {
		m_mxConvertProcess->kill();
		m_mxConvertProcess->waitForFinished(1000);
	}
	m_convertTimeoutTimer->stop();
}

void Mx2dGuiDocument::paintEvent(QPaintEvent* event)
{
#ifdef Q_OS_LINUX
	if (m_isOpenGLPollEvents) {
		m_cadView.UpdateDisplay();
	}
#endif
}

void Mx2dGuiDocument::triggerUpdate() {
	if (!m_updatePending) {
		m_updatePending = true;
		QTimer::singleShot(50, this, [this]() {
			update();
			m_updatePending = false;
			});
	}
}

bool Mx2dGuiDocument::makeCurrent()
{
	return m_cadView.GetMxCADObject()->MakeCurrent();
}

bool Mx2dGuiDocument::executeCommand(const QString& cmdStr, resbuf* pParam)
{
	return m_cadView.GetMxCADObject()->SendStringToExecute(cmdStr.toLocal8Bit().constData(), pParam);
}

int Mx2dGuiDocument::undoCount() const
{
	return m_pAnnoEditor->undoCount();
}

int Mx2dGuiDocument::redoCount() const
{
	return m_pAnnoEditor->redoCount();
}




void Mx2dGuiDocument::wheelEvent(QWheelEvent* theEvent)
{
#ifdef Q_OS_LINUX
	double dX = theEvent->pos().x() * m_devicePixelRatio;
	double dY = theEvent->pos().y() * m_devicePixelRatio;
	m_pGLView->onCursorPostion(dX, dY);
	m_pGLView->onMouseScroll(0, theEvent->delta() / 120);
	m_cadView.UpdateDisplay();
#endif
}

void Mx2dGuiDocument::closeAllModelessDialogs()
{
	QList<QDialog*> modelessDialogs = this->findChildren<QDialog*>();
	for (QDialog* dlg : modelessDialogs) {
		if (dlg->property("modeless_dialog").toBool()) {
			if (dlg->isVisible())
			{
				dlg->close();
			}
		}
	}
}

void Mx2dGuiDocument::resizeEvent(QResizeEvent* event)
{
	double dWidth = event->size().width() * m_devicePixelRatio;
	double dHeight = event->size().height() * m_devicePixelRatio;
	m_cadView.SetMxDrawPosition(0, 0, dWidth, dHeight);
	computeToolBarPosition();
	computePromptLabelPosition();
}

void Mx2dGuiDocument::mouseMoveEvent(QMouseEvent* theEvent)
{
#ifdef Q_OS_LINUX
	m_pGLView->onMouseMove(theEvent->pos().x() * m_devicePixelRatio, theEvent->pos().y() * m_devicePixelRatio);
#endif
}

void Mx2dGuiDocument::keyPressEvent(QKeyEvent* event)
{
	if (Qt::Key_Escape == event->key())
	{
		MxDrawApp::StopAllCommand();
	}
	QWidget::keyPressEvent(event);
}


void Mx2dGuiDocument::mouseEvent(QMouseEvent* theEvent, int action) {
#ifdef Q_OS_LINUX

	double dX = theEvent->pos().x() * m_devicePixelRatio;
	double dY = theEvent->pos().y() * m_devicePixelRatio;
	m_pGLView->onCursorPostion(dX, dY);

	int button = 0;
	Qt::MouseButtons theButtons = theEvent->buttons();

	if ((theButtons & Qt::LeftButton) != 0)
	{
		button = 0;
	}
	if ((theButtons & Qt::MiddleButton) != 0)
	{
		button = 2;
	}
	if ((theButtons & Qt::RightButton) != 0)
	{
		button = 1;
	}


	int mods = 0;

	Qt::KeyboardModifiers  theModifiers = theEvent->modifiers();

	if ((theModifiers & Qt::ShiftModifier) != 0)
	{
		mods = mods |= 1;
	}
	if ((theModifiers & Qt::ControlModifier) != 0)
	{
		mods = mods |= 2;
	}
	if ((theModifiers & Qt::AltModifier) != 0)
	{
		mods = mods |= 4;
	}


	m_pGLView->m_mouseButtons[button] = action;

	m_pGLView->onMouseButton(button, action, mods);

#endif
}

void Mx2dGuiDocument::onShowPromptMessage(QWidget* target, const QString& message)
{
	if (target == this)
	{
		m_pPromptLabel->setText(message);
		m_pPromptLabel->adjustSize();
		computePromptLabelPosition();
		m_pPromptLabel->show();
	}
}

void Mx2dGuiDocument::onHidePromptMessage(QWidget* target)
{
	if (target == this)
	{
		m_pPromptLabel->hide();
	}
}


void Mx2dGuiDocument::onMessageBoxInformation(QWidget* target, const QString& message)
{
	if (target == this)
	{
		// TODO: do something
	}
}

void Mx2dGuiDocument::onProcessStarted()
{
	LOG_ERROR(QString("mxconvert process started successfully"));
	emit startConvert();
}

void Mx2dGuiDocument::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	m_convertTimeoutTimer->stop();

	if (exitStatus != QProcess::NormalExit || exitCode != 0)
	{
		QString errorMsg = QString("mxconvert process exited abnormally, exitCode: %1, exitStatus: %2")
			.arg(exitCode).arg(exitStatus);
		LOG_ERROR(errorMsg);
		emit convertFailed(errorMsg);
		return;
	}

	QByteArray stdoutData = m_mxConvertProcess->readAllStandardOutput();
	QByteArray stderrData = m_mxConvertProcess->readAllStandardError();

	if (!stderrData.isEmpty()) {
		LOG_ERROR(QString("mxconvert stderr: %1").arg(QString::fromUtf8(stderrData)));
	}

	QJsonParseError parseError;
	QJsonDocument retJsonDoc = QJsonDocument::fromJson(stdoutData, &parseError);
	if (parseError.error != QJsonParseError::NoError)
	{
		emit convertFailed(parseError.errorString());
		return;
	}

	QJsonObject retJson = retJsonDoc.object();
	if (retJson.contains("message") && retJson["message"].toString() == "ok")
	{
		readMxwebFile(m_convertOutFilePath, m_originalOpenFilePath);
		return;
	}

	emit convertFailed("mxconvert returned non-ok message");
}

void Mx2dGuiDocument::onProcessErrorOccurred(QProcess::ProcessError error)
{
	m_convertTimeoutTimer->stop();
	QString errorMsg = QString("failed start mxconvert convert program: %1").arg(m_mxConvertProcess->errorString());
	LOG_ERROR(errorMsg);
	emit runConverterFailed();
}

void Mx2dGuiDocument::onConvertTimeout()
{
	if (m_mxConvertProcess->state() == QProcess::Running) {
		m_mxConvertProcess->kill();
	}
	QString errorMsg = QString("mxconvert timeout or crashed: %1").arg(m_mxConvertProcess->errorString());
	LOG_ERROR(errorMsg);
	emit convertFailed(errorMsg);
}

void Mx2dGuiDocument::computeToolBarPosition()
{
	if (!m_pViewWidget || !m_p2dViewToolBar)
	{
		return;
	}
	int tabW = m_pViewWidget->size().width();
	int tabH = m_pViewWidget->size().height();
	int toolBarW = m_p2dViewToolBar->size().width();
	int toolBarH = m_p2dViewToolBar->size().height();
	int targetX = tabW / 2 - toolBarW / 2;
	int targetY = tabH - 80 - toolBarH;
	m_p2dViewToolBar->move(targetX, targetY);
}

void Mx2dGuiDocument::computePromptLabelPosition()
{
	if (!m_pViewWidget || !m_pPromptLabel)
	{
		return;
	}
	int yOffset = 20;
	m_pPromptLabel->move(m_pViewWidget->width() / 2 - m_pPromptLabel->width() / 2, yOffset);
}

void Mx2dGuiDocument::connectSignals()
{
	connect(this, &Mx2dGuiDocument::fileRead, this, [this](bool success) {
		if (success)
		{
			m_spaceNames = getSpaceNames();
			renderShape();
		}
		});
	connect(&MxSignalTransfer::getInstance(), &MxSignalTransfer::signalShowPromptMessage, this, &Mx2dGuiDocument::onShowPromptMessage);
	connect(&MxSignalTransfer::getInstance(), &MxSignalTransfer::signalHidePromptMessage, this, &Mx2dGuiDocument::onHidePromptMessage);
	connect(&MxSignalTransfer::getInstance(), &MxSignalTransfer::signalMessageBoxInformation, this, &Mx2dGuiDocument::onMessageBoxInformation);
	connect(&Mx2dSignalTransfer::getInstance(), &Mx2dSignalTransfer::signalAddAnnotation, this, [this](QWidget* target, McDbObjectId id) {
		if (target != this)
		{
			return;
		}
		m_pAnnoEditor->addAnnotation(id);
		});
	connect(&Mx2dSignalTransfer::getInstance(), &Mx2dSignalTransfer::signalAddAnnotations, this, [this](QWidget* target, McDbObjectIdArray ids) {
		if (target != this)
		{
			return;
		}
		m_pAnnoEditor->addAnnotations(ids);
		});
	connect(&Mx2dSignalTransfer::getInstance(), &Mx2dSignalTransfer::signalEeaseAnnotations, this, [this](QWidget* target, McDbObjectIdArray ids) {
		if (target != this)
		{
			return;
		}
		m_pAnnoEditor->eraseAnnotations(ids);
		});
	connect(&Mx2dSignalTransfer::getInstance(), &Mx2dSignalTransfer::signalEraseAllAnnotations, this, [this](QWidget* target) {
		if (target != this)
		{
			return;
		}
		m_pAnnoEditor->eraseAllAnnotations();
		});
	connect(&Mx2dSignalTransfer::getInstance(), &Mx2dSignalTransfer::signalShowAllAnnotations, this, [this](QWidget* target, bool visable) {
		if (target != this)
		{
			return;
		}
		m_pAnnoEditor->showAllAnnotations(visable);
		});
	connect(&Mx2dSignalTransfer::getInstance(), &Mx2dSignalTransfer::signalMoveAnnotations, this, [this](QWidget* target, McDbObjectIdArray ids, McGeVector3d moveVec) {
		if (target != this)
		{
			return;
		}
		m_pAnnoEditor->moveAnnotations(ids, moveVec);
		});
	connect(&Mx2dSignalTransfer::getInstance(), &Mx2dSignalTransfer::signalMoveAnnotation, this, [this](QWidget* target, McDbObjectId id, McGeVector3d moveVec) {
		if (target != this)
		{
			return;
		}
		m_pAnnoEditor->moveAnnotation(id, moveVec);
		});
	connect(&Mx2dSignalTransfer::getInstance(), &Mx2dSignalTransfer::signalModifyText, this, [this](QWidget* target, McDbObjectId id, const QString& text) {
		if (target != this)
		{
			return;
		}
		m_pAnnoEditor->modifyText(id, text);
		});
	connect(&Mx2dSignalTransfer::getInstance(), &Mx2dSignalTransfer::signalModifyPolyArea, this, [this](QWidget* target, McDbObjectId id, const McGePoint3dArray& oldPoints, const McGePoint3dArray& newPoints) {
		if (target != this)
		{
			return;
		}
		m_pAnnoEditor->modifyPolyArea(id, oldPoints, newPoints);
		});
	connect(&Mx2dSignalTransfer::getInstance(), &Mx2dSignalTransfer::signalModifyArcPolyArea, this, [this](QWidget* target, McDbObjectId id, const Mx2d::PLVertexList& oldPoints, const Mx2d::PLVertexList& newPoints) {
		if (target != this)
		{
			return;
		}
		m_pAnnoEditor->modifyArcPolyArea(id, oldPoints, newPoints);
		});
	connect(&Mx2dSignalTransfer::getInstance(), &Mx2dSignalTransfer::signalModifyRectArea, this, [this](QWidget* target, McDbObjectId id, const McGePoint3d& oldCorner1, const McGePoint3d& oldCorner2, const McGePoint3d& newCorner1, const McGePoint3d& newCorner2) {
		if (target != this)
		{
			return;
		}
		m_pAnnoEditor->modifyRectArea(id, oldCorner1, oldCorner2, newCorner1, newCorner2);
		});
	connect(&Mx2dSignalTransfer::getInstance(), &Mx2dSignalTransfer::signalModifyAnnotation, this, [this](QWidget* target, McDbObjectId id, const QJsonObject& oldJson, const QJsonObject& newJson) {
		if (target != this)
		{
			return;
		}
		m_pAnnoEditor->modifyAnnotation(id, oldJson, newJson);
		});
	connect(&Mx2dSignalTransfer::getInstance(), &Mx2dSignalTransfer::signalDoTest, this, [this](QWidget* target) {
		if (target != this)
		{
			return;
		}
		});
	connect(&Mx2dSignalTransfer::getInstance(), &Mx2dSignalTransfer::signalUndo, this, [this](QWidget* target) {
		if (target != this)
		{
			return;
		}
		m_pAnnoEditor->undo();
		MxDrawApp::UpdateDisplay();
		});
	connect(&Mx2dSignalTransfer::getInstance(), &Mx2dSignalTransfer::signalRedo, this, [this](QWidget* target) {
		if (target != this)
		{
			return;
		}
		m_pAnnoEditor->redo();
		MxDrawApp::UpdateDisplay();
		});
	connect(m_pAnnoEditor.get(), &Mx2dAnnotationEditor::undoRedoChanged, this, &Mx2dGuiDocument::undoRedoChanged);

	connect(m_mxConvertProcess, &QProcess::started, this, &Mx2dGuiDocument::onProcessStarted);
	connect(m_mxConvertProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &Mx2dGuiDocument::onProcessFinished);
	connect(m_mxConvertProcess, &QProcess::errorOccurred, this, &Mx2dGuiDocument::onProcessErrorOccurred);
	m_convertTimeoutTimer->setSingleShot(true);
	connect(m_convertTimeoutTimer, &QTimer::timeout, this, &Mx2dGuiDocument::onConvertTimeout);

	connect(&Mx2dSignalTransfer::getInstance(), &Mx2dSignalTransfer::signalTableRect, this, &Mx2dGuiDocument::onExtractTable);
}

QStringList Mx2dGuiDocument::getSpaceNames()
{
	QStringList spaceNames;
	MxStringArray layoutNames;
	MrxDbgUtils::GetAllLayoutName(Mx::mcdbCurDwg(), layoutNames);
	for (int i = 0; i < layoutNames.size(); ++i)
	{
        MxString layoutName = layoutNames[i];
		QString currentLayoutName = QString::fromLocal8Bit(layoutName.c_str());
		spaceNames.append(currentLayoutName);
		QPushButton* btn = new QPushButton(currentLayoutName, this);
		// set btn checked style
        btn->setStyleSheet("QPushButton{ padding: 10px;}"
            "QPushButton:checked{background-color: white; color: #57b8ff;}");
		btn->setCheckable(true);
		m_spaceBtnLayout->insertWidget(i, btn, 0);
		m_pSpaceBtnGroup->addButton(btn, i);
		if (i == 0)
			btn->setChecked(true);
	}
	connect(m_pSpaceBtnGroup, QOverload<int, bool>::of(&QButtonGroup::buttonToggled), this, [this](int id, bool checked) {
		if (checked)
		{
			MxDrawApp::SetCurrentLayout(m_spaceNames[id].toLocal8Bit().constData());
            MxDrawApp::UpdateDisplay();
		}
		});
	return spaceNames;
}

void Mx2dGuiDocument::readMxwebFile(const QString& mxwebPath, const QString& originalFilePath)
{
	QtConcurrent::run([this, mxwebPath, originalFilePath]()
		{
			std::string sOpenFile;
#ifdef Q_OS_LINUX
			sOpenFile = mxwebPath.toUtf8().toStdString();
#else
			sOpenFile = mxwebPath.toLocal8Bit().toStdString();
#endif
			emit startReadFile();
			if (m_cadView.ReadFile(sOpenFile.c_str()))
			{
				m_filePath = originalFilePath;
				emit fileRead(true);
				return;
			}
			emit fileRead(false);
		});
}

void Mx2dGuiDocument::mousePressEvent(QMouseEvent* theEvent)
{
	mouseEvent(theEvent, 1);
}

void Mx2dGuiDocument::mouseReleaseEvent(QMouseEvent* theEvent)
{
	mouseEvent(theEvent, 0);
}


MxRecentFile Mx2dGuiDocument::createRecentFile()
{
	QByteArray ba;
	QBuffer buffer(&ba);
	buffer.open(QIODevice::WriteOnly);
	QPixmap pixmap(":/resources/images2d/default_dwg.png"); // dwg default thumbnail size is 235 * 165
	pixmap.save(&buffer, "PNG");
	buffer.close();

	MxRecentFile recentFile;
	QFileInfo fileInfo(m_filePath);
	recentFile.filePath = m_filePath;
	recentFile.lastOpened = QDateTime::currentDateTime();
	recentFile.fileType = fileInfo.suffix();
	recentFile.fileSize = fileInfo.size();
	recentFile.thumbnail = ba;
	return recentFile;
}


#include <QCryptographicHash>
namespace {
	QString calculatePathHash(const QString& filePath)
	{
		QCryptographicHash hash(QCryptographicHash::Sha1);
		QByteArray pathBytes = filePath.toUtf8();
		hash.addData(pathBytes);
		return hash.result().toHex();
	}
}


#include <QStandardPaths>
#include <QDir>
void Mx2dGuiDocument::renderShape()
{
	emit startRender();
	m_cadView.UpdateDisplay();
	emit fileReady();

	MxRecentFile recentFile = createRecentFile();
	MxRecentFileManager::instance()->addRecentFile(recentFile);

	MrxDbgUtils::addLayer("MxCADAnnotationLayer");

	QString docPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
	QString subDir = "MxCADViewer";
	QString pathHash = calculatePathHash(m_filePath);

	QDir dir(docPath);

	if (!dir.exists(subDir)) {
		dir.mkpath(subDir);
	}

	QString fullPath = QDir(QDir(docPath).filePath(subDir)).filePath(pathHash + ".json");

	m_pAnnoEditor->loadFromFile(fullPath);

}

void Mx2dGuiDocument::openFile(const QString& filePath) {
	
	QFileInfo fileInfo(filePath);
	QString suffix = fileInfo.suffix().toLower();
	if (suffix == "mxweb")
	{
		readMxwebFile(filePath, filePath);
	}
	else // dwg,dxf
	{
		QDateTime lastModified = fileInfo.lastModified();
		qint64 timestampMs = lastModified.toMSecsSinceEpoch();
		QString modifiedStr = QString::number(timestampMs);
		QString outname = calculatePathHash(filePath + modifiedStr);
		QString docPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
		QString subDir = "MxCADViewer";
		QDir dir(docPath);
		if (!dir.exists(subDir))
			dir.mkpath(subDir);
		QDir outDir(QDir(docPath).filePath(subDir));
		QString outfileName = outname + ".mxweb";
		QString outfilePath = QDir(outDir).filePath(outfileName);
		if (outDir.exists(outfileName)) {
			// open the file
			readMxwebFile(outfilePath, filePath);
			return;
		}

		// invoke mxconvert to convert dwg or dxf to .mxweb file
		QProcess process;
		QString appPath = QCoreApplication::applicationDirPath();
		QString mxconvertPath;
#ifdef Q_OS_WIN
		mxconvertPath = appPath + "/mxconvert.exe";
#else
		mxconvertPath = appPath + "/mxconvert";
#endif
        if(!QFile::exists(mxconvertPath)){
#ifdef Q_OS_WIN
			mxconvertPath = appPath + "/../3rdparty/mxconvert/mxconvert.exe";
#else
			mxconvertPath = appPath + "/../3rdparty/mxconvert/mxconvert";
#endif
			if(!QFile::exists(mxconvertPath)){
				emit runConverterFailed();
				return;
			}
		}

		m_convertSrcFilePath = filePath;
        m_convertOutFilePath = outfilePath;
        m_originalOpenFilePath = filePath;

        QStringList arguments;
        arguments << "-s" << filePath << "-o" << outfilePath;
        m_mxConvertProcess->setProgram(mxconvertPath);
        m_mxConvertProcess->setArguments(arguments);
        m_mxConvertProcess->setWorkingDirectory(QFileInfo(mxconvertPath).absolutePath());

        // 10min
        m_convertTimeoutTimer->setInterval(600000);
        m_convertTimeoutTimer->start();

        // async
        m_mxConvertProcess->start();

	}

#ifdef Q_OS_LINUX
	m_isOpenGLPollEvents = true;
#endif
}

void Mx2dGuiDocument::showLayerManagerDialog(QWidget* guiDoc2d)
{
	if (guiDoc2d != this)
		return;
	if (!m_layerManagerDialog)
	{
		m_layerManagerDialog = new Mx2dLayerManagerDialog(this, this);
		m_layerManagerDialog->setProperty("modeless_dialog", true);
	}

	if (!m_layerManagerDialog->isVisible())
	{
		m_layerManagerDialog->show();
	}
}

void Mx2dGuiDocument::showMeasurementDialog(QWidget* guiDoc2d)
{
    if (guiDoc2d != this)
        return;
    if (!m_measurementDialog)
    {
        m_measurementDialog = new Mx2dMeasurementDialog(this, this);
        m_measurementDialog->setProperty("modeless_dialog", true);
        m_measurementDialog->move(this->mapToGlobal(QPoint(0, 0)));
    }
    if (!m_measurementDialog->isVisible())
    {
        m_measurementDialog->show();
    }
}

void Mx2dGuiDocument::showLeaderTextInputDialog(QWidget* guiDoc2d)
{
    if (guiDoc2d != this)
        return;
    if (!m_leaderTextInputDialog)
    {
        m_leaderTextInputDialog = new Mx2dLeaderTextInputDialog(this, this);
        m_leaderTextInputDialog->setProperty("modeless_dialog", true);
    }
    if (!m_leaderTextInputDialog->isVisible())
    {
        m_leaderTextInputDialog->show();
    }
}

void Mx2dGuiDocument::showTextSearchDialog(QWidget* guiDoc2d)
{
    if (guiDoc2d != this)
        return;
    if (!m_textSearchDialog)
    {
        m_textSearchDialog = new Mx2dTextSearchDialog(this);
        m_textSearchDialog->setProperty("modeless_dialog", true);
    }
    if (!m_textSearchDialog->isVisible())
    {
        m_textSearchDialog->show();
    }
}

void Mx2dGuiDocument::extractText(QWidget* guiDoc2d)
{
	if (guiDoc2d != this)
        return;
	
	connect(&Mx2dSignalTransfer::getInstance(), &Mx2dSignalTransfer::signalExtractTextFinished, this, [this](QWidget* tab, const QStringList& textList) {
		if (tab != this)
            return;
		if (!m_extractTextDialog)
		{
			m_extractTextDialog = new Mx2dExtractTextDialog(this);
			m_extractTextDialog->setProperty("modeless_dialog", true);
		}
		if (!m_extractTextDialog->isVisible())
		{
			m_extractTextDialog->show();
		}
		m_extractTextDialog->setTexts(textList);
		disconnect(&Mx2dSignalTransfer::getInstance(), &Mx2dSignalTransfer::signalExtractTextFinished, this, nullptr);
		});
	executeCommand("Mx_ExtractTextRect");
}

void Mx2dGuiDocument::extractTable(QWidget* guiDoc2d)
{
    if (guiDoc2d != this)
        return;
	executeCommand("Mx_ExtractTable");
}

void Mx2dGuiDocument::onExtractTable(QWidget* guiDoc2d, const McGePoint3d& new_corner1, const McGePoint3d& new_corner2)
{
	if (guiDoc2d != this)
		return;

	double m_gap = 0.1;

	McGePoint3d ptLT(qMin(new_corner1.x, new_corner2.x), qMax(new_corner1.y, new_corner2.y), 0);
	McGePoint3d ptRT(qMax(new_corner1.x, new_corner2.x), qMax(new_corner1.y, new_corner2.y), 0);
	McGePoint3d ptRB(qMax(new_corner1.x, new_corner2.x), qMin(new_corner1.y, new_corner2.y), 0);
	McGePoint3d ptLB(qMin(new_corner1.x, new_corner2.x), qMin(new_corner1.y, new_corner2.y), 0);

	double new_sel_rect_left = qMin(new_corner1.x, new_corner2.x);
	double new_sel_rect_right = qMax(new_corner1.x, new_corner2.x);
	double new_sel_rect_bottom = qMin(new_corner1.y, new_corner2.y);
	double new_sel_rect_top = qMax(new_corner1.y, new_corner2.y);

	MrxDbgSelSet ss;

	if (ss.crossingSelect(new_corner1, new_corner2/*, spFilter.data()*/) != MrxDbgSelSet::kSelected)
	{
		QMessageBox::information(this, tr("Prompt"), tr("No entity selected!"));
		return;
	}
	McDbObjectIdArray aryId;
	ss.asArray(aryId);

	std::vector<McDbEntity*>  ent_ptr_array_all;
	for (int i = 0; i < aryId.length(); i++)
	{
		McDbObjectPointer<McDbEntity> spEntity(aryId[i], McDb::kForWrite);
		if (spEntity.openStatus() != Mcad::eOk)
			continue;

		if (spEntity->isA() == McDbBlockReference::desc())
		{
			McDbBlockReference* pBlockRef = McDbBlockReference::cast(spEntity.object());
			McDbVoidPtrArray entitySet;
			Mx2d::recursiveExplodeBlock(pBlockRef, entitySet);
			for (int j = 0; j < entitySet.length(); j++)
			{
				McDbEntity* pEntity = (McDbEntity*)entitySet[j];
				ent_ptr_array_all.emplace_back(pEntity);
			}
		}
		else if (spEntity->isA() == McDbProxyEntity::desc())
		{
			McDbProxyEntity* pProxyEntity = McDbProxyEntity::cast(spEntity.object());
			McDbVoidPtrArray entitySet;
			pProxyEntity->explode(entitySet);
			for (int j = 0; j < entitySet.length(); j++)
			{
				McDbEntity* pEntity = (McDbEntity*)entitySet[j];
				ent_ptr_array_all.emplace_back(pEntity);
			}
		}
		else
		{
			McDbEntity* pEntity = (McDbEntity*)spEntity->clone();
			ent_ptr_array_all.emplace_back(pEntity);
		}
	}

	std::vector<McDbText*> all_texts;
	double text_height_avg, text_height_sum = 0;
	double text_width_height_ratio_avg, text_width_height_ratio_sum = 0;
	for (int i = 0; i < (int)ent_ptr_array_all.size(); i++)
	{
		McDbEntity* pEntity = ent_ptr_array_all[i];
		if (pEntity->isA() == McDbText::desc())
		{
			McDbText* pText = (McDbText*)pEntity->clone();
			McDbExtents ext;
			pText->getGeomExtents(ext, false);
			McGePoint3d centerPoint((ext.minPoint().x + ext.maxPoint().x) / 2, (ext.minPoint().y + ext.maxPoint().y) / 2, 0);
			if (centerPoint.x >= new_sel_rect_left && centerPoint.x <= new_sel_rect_right && centerPoint.y >= new_sel_rect_bottom && centerPoint.y <= new_sel_rect_top)
			{
				all_texts.emplace_back(pText);
				text_height_sum += pText->height();
				text_width_height_ratio_sum += pText->widthFactor();
			}
		}
		else if (pEntity->isA() == McDbMText::desc())
		{
			McDbMText* pMText = McDbMText::cast(pEntity);
			McDbVoidPtrArray entitySet;
			pMText->explode(entitySet);
			for (int j = 0; j < entitySet.length(); j++)
			{
				McDbText* pText = (McDbText*)entitySet[j];

				McDbExtents ext;
				pText->getGeomExtents(ext, false);
				McGePoint3d centerPoint((ext.minPoint().x + ext.maxPoint().x) / 2, (ext.minPoint().y + ext.maxPoint().y) / 2, 0);
				if (centerPoint.x >= new_sel_rect_left && centerPoint.x <= new_sel_rect_right && centerPoint.y >= new_sel_rect_bottom && centerPoint.y <= new_sel_rect_top)
				{
					all_texts.emplace_back(pText);
					text_height_sum += pText->height();
					text_width_height_ratio_sum += pText->widthFactor();
				}
			}
		}
	}
	bool set_ratio = false;

	if (all_texts.size() != 0)
	{
		set_ratio = true;
		text_height_avg = text_height_sum / all_texts.size();
		text_width_height_ratio_avg = text_width_height_ratio_sum / all_texts.size();
	}

	std::vector<McDbLine*> linesPtr;

	for (int i = 0; i < (int)ent_ptr_array_all.size(); i++)
	{
		McDbEntity* pEntity = ent_ptr_array_all[i];
		if (pEntity->isA() == McDbPolyline::desc())
		{
			McDbPolyline* pPLine = McDbPolyline::cast(pEntity);
			McDbVoidPtrArray entitySet;
			pPLine->explode(entitySet);
			for (int j = 0; j < entitySet.length(); j++)
			{
				McDbEntity* pEntity = (McDbEntity*)entitySet[j];
				if (pEntity->isA() == McDbLine::desc())
				{
					linesPtr.emplace_back((McDbLine*)pEntity);
				}
				else
				{
					delete pEntity;
				}
			}
			continue;
		}
		else if (pEntity->isA() == McDbLine::desc())
		{
			McDbLine* pLine = (McDbLine*)pEntity->clone();
			linesPtr.emplace_back(pLine);
		}
	}

	for (int i = 0; i < (int)ent_ptr_array_all.size(); i++)
	{
		delete ent_ptr_array_all[i];
	}

	std::vector<McDbLine*> horizontalLines, verticalLines;
	std::vector<double> Xcoords, Ycoords;
	for (int i = 0; i < (int)linesPtr.size(); i++)
	{
		if (Mx2d::IsEqual(linesPtr[i]->startPoint().y, linesPtr[i]->endPoint().y, m_gap))
		{
			bool intersectTop = Mx2d::isIntersect(linesPtr[i]->startPoint(), linesPtr[i]->endPoint(), ptLT, ptRT);
			bool intersectRight = Mx2d::isIntersect(linesPtr[i]->startPoint(), linesPtr[i]->endPoint(), ptRT, ptRB);
			bool intersectBottom = Mx2d::isIntersect(linesPtr[i]->startPoint(), linesPtr[i]->endPoint(), ptRB, ptLB);
			bool intersectLeft = Mx2d::isIntersect(linesPtr[i]->startPoint(), linesPtr[i]->endPoint(), ptLB, ptLT);

			bool inRect = Mx2d::isInRect(linesPtr[i]->startPoint(), linesPtr[i]->endPoint(), ptLT.x, ptRB.x, ptRB.y, ptLT.y);

			if (intersectTop || intersectRight || intersectBottom || intersectLeft || inRect)
			{
				horizontalLines.emplace_back(linesPtr[i]);
				bool findSame = false;
				for (int j = 0; j < (int)Ycoords.size(); j++)
				{
					if (Mx2d::IsEqual(linesPtr[i]->startPoint().y, Ycoords[j], m_gap))
					{
						findSame = true;
						break;
					}
				}
				if (!findSame)
				{
					Ycoords.emplace_back(linesPtr[i]->startPoint().y);
				}
				continue;
			}
		}
		else if (Mx2d::IsEqual(linesPtr[i]->startPoint().x, linesPtr[i]->endPoint().x, m_gap))
		{
			bool intersectTop = Mx2d::isIntersect(linesPtr[i]->startPoint(), linesPtr[i]->endPoint(), ptLT, ptRT);
			bool intersectRight = Mx2d::isIntersect(linesPtr[i]->startPoint(), linesPtr[i]->endPoint(), ptRT, ptRB);
			bool intersectBottom = Mx2d::isIntersect(linesPtr[i]->startPoint(), linesPtr[i]->endPoint(), ptRB, ptLB);
			bool intersectLeft = Mx2d::isIntersect(linesPtr[i]->startPoint(), linesPtr[i]->endPoint(), ptLB, ptLT);

			bool inRect = Mx2d::isInRect(linesPtr[i]->startPoint(), linesPtr[i]->endPoint(), ptLT.x, ptRB.x, ptRB.y, ptLT.y);

			if (intersectTop || intersectRight || intersectBottom || intersectLeft || inRect)
			{
				verticalLines.emplace_back(linesPtr[i]);
				bool findSame = false;
				for (int j = 0; j < (int)Xcoords.size(); j++)
				{
					if (Mx2d::IsEqual(linesPtr[i]->startPoint().x, Xcoords[j], m_gap))
					{
						findSame = true;
						break;
					}
				}
				if (!findSame)
				{
					Xcoords.emplace_back(linesPtr[i]->startPoint().x);
				}
			}

		}
	}

	if ((int)Xcoords.size() < 2 || (int)Ycoords.size() < 2)
	{
		LOG_ERROR(QString("can not be a table"));
		QMessageBox::information(this, tr("Prompt"), tr("can not be a table"));
		return;
	}

	std::vector<double>::iterator itS = Xcoords.begin(), itE = Xcoords.end();
	std::sort(itS, itE);
	itS = Ycoords.begin(), itE = Ycoords.end();
	std::sort(itS, itE);

	std::map<double, std::vector< McDbLine* >> sortedHorizontalLines, sortedVerticalLines;

	std::map<double, std::vector<Mx2d::ContinueRange>> sortedHorizontalRanges, sortedVerticalRanges;

	for (int i = 0; i < (int)Ycoords.size(); i++)
	{
		std::vector<McDbLine*> sameYcoordLines;
		std::vector<Mx2d::ContinueRange> ranges;
		for (int j = 0; j < (int)horizontalLines.size(); j++)
		{
			if (Mx2d::IsEqual(Ycoords[i], horizontalLines[j]->startPoint().y, m_gap))
			{
				sameYcoordLines.emplace_back(horizontalLines[j]);
			}
		}
		std::vector<McDbLine*>::iterator itS = sameYcoordLines.begin(), itE = sameYcoordLines.end();
		std::sort(itS, itE, Mx2d::cmpHLine);
		sortedHorizontalLines.insert(make_pair(Ycoords[i], sameYcoordLines));
		McDbLine* pFirst = *itS;
		Mx2d::ContinueRange curRange(qMin(pFirst->startPoint().x, pFirst->endPoint().x), qMax(pFirst->startPoint().x, pFirst->endPoint().x));
		for (; itS != itE - 1; itS++)
		{
			McDbLine* pNext = *(itS + 1);
			double nextStart = qMin(pNext->startPoint().x, pNext->endPoint().x);
			double nextEnd = qMax(pNext->startPoint().x, pNext->endPoint().x);
			if ((curRange.end >= nextStart || curRange.end + m_gap > nextStart) && curRange.end <= nextEnd)
			{
				curRange = Mx2d::ContinueRange(curRange.start, nextEnd);
			}
			else
			{
				ranges.emplace_back(curRange);
				curRange = Mx2d::ContinueRange(nextStart, nextEnd);
			}
		}
		ranges.emplace_back(curRange);
		sortedHorizontalRanges.insert(make_pair(Ycoords[i], ranges));

	}
	for (int i = 0; i < (int)Xcoords.size(); i++)
	{
		std::vector<McDbLine*> sameXcoordLines;
		std::vector<Mx2d::ContinueRange> ranges;
		for (int j = 0; j < (int)verticalLines.size(); j++)
		{
			if (Mx2d::IsEqual(Xcoords[i], verticalLines[j]->startPoint().x, m_gap))
			{
				sameXcoordLines.emplace_back(verticalLines[j]);
			}
		}
		std::vector<McDbLine*>::iterator itS = sameXcoordLines.begin(), itE = sameXcoordLines.end();
		sort(itS, itE, Mx2d::cmpVLine);
		sortedVerticalLines.insert(make_pair(Xcoords[i], sameXcoordLines));
		McDbLine* pFirst = *itS;
		Mx2d::ContinueRange curRange(qMin(pFirst->startPoint().y, pFirst->endPoint().y), qMax(pFirst->startPoint().y, pFirst->endPoint().y));
		for (; itS != itE - 1; itS++)
		{
			McDbLine* pNext = *(itS + 1);
			double nextStart = qMin(pNext->startPoint().y, pNext->endPoint().y);
			double nextEnd = qMax(pNext->startPoint().y, pNext->endPoint().y);
			if ((curRange.end >= nextStart || curRange.end + m_gap > nextStart) && curRange.end <= nextEnd)
			{
				curRange = Mx2d::ContinueRange(curRange.start, nextEnd);
			}
			else
			{
				ranges.emplace_back(curRange);
				curRange = Mx2d::ContinueRange(nextStart, nextEnd);
			}
		}
		ranges.emplace_back(curRange);
		sortedVerticalRanges.insert(make_pair(Xcoords[i], ranges));

	}

	for (int i = 0; i < (int)linesPtr.size(); i++)
	{
		delete linesPtr[i];
	}

	std::vector<Mx2d::CellRect> uniqueRects;

	for (int i = 0; i < (int)Xcoords.size() - 1; i++)
	{
		for (int j = i + 1; j < (int)Xcoords.size(); j++)
		{
			for (int k = 0; k < (int)Ycoords.size() - 1; k++)
			{
				for (int m = k + 1; m < (int)Ycoords.size(); m++)
				{
					uniqueRects.emplace_back(Mx2d::CellRect(Xcoords[i], Xcoords[j], Ycoords[k], Ycoords[m]));
				}
			}
		}
	}

	std::vector<Mx2d::CellRect> noCrossRangeRects;
	std::vector<Mx2d::CellRect> cellRects;

	for (int i = 0; i < (int)uniqueRects.size(); i++)
	{
		Mx2d::CellRect rect = uniqueRects[i];
		std::map<double, std::vector<Mx2d::ContinueRange>>::iterator itLeft, itRight, itBottom, itTop, itEndH, itEndV;
		itLeft = sortedVerticalRanges.find(rect.left);
		itRight = sortedVerticalRanges.find(rect.right);
		itBottom = sortedHorizontalRanges.find(rect.bottom);
		itTop = sortedHorizontalRanges.find(rect.top);
		itEndH = sortedHorizontalRanges.end();
		itEndV = sortedVerticalRanges.end();
		if (itLeft != itEndV && itRight != itEndV && itBottom != itEndH && itTop != itEndH)
		{
			std::vector<Mx2d::ContinueRange> rangesLeft = itLeft->second;
			std::vector<Mx2d::ContinueRange> rangesRight = itRight->second;
			Mx2d::ContinueRange rect_left_right_edge(rect.bottom, rect.top);
			std::vector<Mx2d::ContinueRange> rangesBottom = itBottom->second;
			std::vector<Mx2d::ContinueRange> rangesTop = itTop->second;
			Mx2d::ContinueRange rect_bottom_top_edge(rect.left, rect.right);

			if (std::find(rangesLeft.begin(), rangesLeft.end(), rect_left_right_edge) == rangesLeft.end())
				continue;
			if (std::find(rangesRight.begin(), rangesRight.end(), rect_left_right_edge) == rangesRight.end())
				continue;
			if (std::find(rangesBottom.begin(), rangesBottom.end(), rect_bottom_top_edge) == rangesBottom.end())
				continue;
			if (std::find(rangesTop.begin(), rangesTop.end(), rect_bottom_top_edge) == rangesTop.end())
				continue;

			std::map<double, std::vector<Mx2d::ContinueRange>>::iterator it = sortedHorizontalRanges.begin();
			bool beCrossedByH = false;
			for (; it != sortedHorizontalRanges.end(); it++)
			{
				double y = it->first;
				std::vector<Mx2d::ContinueRange> ranges = it->second;
				if (y > rect.bottom && y < rect.top)
				{
					for (int j = 0; j < (int)ranges.size(); j++)
					{
						if (rect.left >= ranges[j].start - m_gap && rect.right <= ranges[j].end + m_gap)
						{
							beCrossedByH = true;
							break;
						}
					}
					if (!beCrossedByH)
					{
						continue;
					}
					else
					{
						break;
					}
				}
			}
			if (beCrossedByH)
			{
				continue;
			}
			it = sortedVerticalRanges.begin();
			bool beCrossedByV = false;
			for (; it != sortedVerticalRanges.end(); it++)// vertical line
			{
				double x = it->first;
				std::vector<Mx2d::ContinueRange> ranges = it->second;
				if (x > rect.left && x < rect.right)
				{
					for (int j = 0; j < (int)ranges.size(); j++)
					{
						if (rect.bottom >= ranges[j].start - m_gap && rect.top <= ranges[j].end + m_gap)
						{
							beCrossedByV = true;
							break;
						}
					}
					if (!beCrossedByV)
					{
						continue;
					}
					else
					{
						break;
					}
				}
			}
			if (beCrossedByV)
			{
				continue;
			}

			noCrossRangeRects.emplace_back(rect);
		}
		else
		{
			continue;
		}
	}

	for (int i = 0; i < (int)noCrossRangeRects.size(); i++)
	{
		bool isInnerRect = false;
		for (int j = 0; j < (int)noCrossRangeRects.size(); j++)
		{
			if (i == j)
			{
				continue;
			}
			if (noCrossRangeRects[i].beLongTo(noCrossRangeRects[j]))
			{
				isInnerRect = true;
				break;
			}
		}
		if (!isInnerRect)
		{
			cellRects.emplace_back(noCrossRangeRects[i]);
		}
	}

	if ((int)cellRects.size() == 0)
	{
		QMessageBox::information(this, tr("Prompt"), tr("can not be a table"));
		return;
	}

	std::vector<double> newXcoords, newYcoords;

	for (int i = 0; i < (int)cellRects.size(); i++)
	{
		Mx2d::CellRect rect = cellRects[i];
		bool findLeft = false;
		bool findRight = false;
		bool findBottom = false;
		bool findTop = false;
		for (int j = 0; j < (int)newXcoords.size(); j++)
		{
			if (Mx2d::IsEqual(rect.left, newXcoords[j], m_gap))
			{
				findLeft = true;
				break;
			}
		}
		if (!findLeft)
		{
			newXcoords.emplace_back(rect.left);
		}
		for (int j = 0; j < (int)newXcoords.size(); j++)
		{
			if (Mx2d::IsEqual(rect.right, newXcoords[j], m_gap))
			{
				findRight = true;
				break;
			}
		}
		if (!findRight)
		{
			newXcoords.emplace_back(rect.right);
		}
		for (int j = 0; j < (int)newYcoords.size(); j++)
		{
			if (Mx2d::IsEqual(rect.bottom, newYcoords[j], m_gap))
			{
				findBottom = true;
				break;
			}
		}
		if (!findBottom)
		{
			newYcoords.emplace_back(rect.bottom);
		}
		for (int j = 0; j < (int)newYcoords.size(); j++)
		{
			if (Mx2d::IsEqual(rect.top, newYcoords[j], m_gap))
			{
				findTop = true;
				break;
			}
		}
		if (!findTop)
		{
			newYcoords.emplace_back(rect.top);
		}
	}

	std::vector<double>::iterator it_newS = newXcoords.begin(), it_newE = newXcoords.end();
	std::sort(it_newS, it_newE);
	it_newS = newYcoords.begin(), it_newE = newYcoords.end();
	std::sort(it_newS, it_newE);

	int cnt = 0;
	std::map<int, double> row, colomn;
	for (int i = 0; i < (int)newYcoords.size() - 1; i++)
	{
		double YcoordS = newYcoords[i];
		double YcoordE = newYcoords[i + 1];
		double Ycoord = (YcoordS + YcoordE) / 2;
		row.insert(std::make_pair(cnt, Ycoord));
		cnt++;
	}
	cnt = 0;
	for (int i = 0; i < (int)newXcoords.size() - 1; i++)
	{
		double XcoordS = newXcoords[i];
		double XcoordE = newXcoords[i + 1];
		double Xcoord = (XcoordS + XcoordE) / 2;
		colomn.insert(std::make_pair(cnt, Xcoord));
		cnt++;
	}
	for (int i = 0; i < (int)cellRects.size(); i++)
	{
		Mx2d::CellRect& cell_rect = cellRects[i];
		std::map<int, double>::iterator it_r = row.begin(), it_c = colomn.begin();
		std::vector<int> rows_buf, colomns_buf;
		for (; it_r != row.end(); it_r++)
		{
			double Ycoord = it_r->second;
			int rowNum = it_r->first;
			if (Ycoord > cell_rect.bottom && Ycoord < cell_rect.top)
			{
				rows_buf.emplace_back(rowNum);
			}
		}
		cell_rect.rowStart = rows_buf.front();
		cell_rect.rowEnd = rows_buf.back();
		for (; it_c != colomn.end(); it_c++)
		{
			double Xcoord = it_c->second;
			int colomnNum = it_c->first;
			if (Xcoord > cell_rect.left && Xcoord < cell_rect.right)
			{
				colomns_buf.emplace_back(colomnNum);
			}
		}
		cell_rect.columnStart = colomns_buf.front();
		cell_rect.columnEnd = colomns_buf.back();
	}


	for (int i = 0; i < (int)cellRects.size(); i++)
	{
		Mx2d::CellRect& cell = cellRects[i];

		std::vector<McDbText*> texts;
		std::map<double, std::vector<McDbText*>> rows;
		QString text;
		for (int j = 0; j < (int)all_texts.size(); j++)
		{
			McDbExtents ext;
			all_texts[j]->getGeomExtents(ext, false);
			McGePoint3d pt((ext.minPoint().x + ext.maxPoint().x) / 2, (ext.minPoint().y + ext.maxPoint().y) / 2, 0);
			if (pt.x<cell.left || pt.x > cell.right || pt.y > cell.top || pt.y < cell.bottom)
				continue;
			texts.emplace_back(all_texts[j]);

		}

		if ((int)texts.size() != 0)
		{
			for (int m = 0; m < (int)texts.size() - 1; m++)
			{
				for (int n = 0; n < (int)texts.size() - 1 - m; n++)
				{
					if (texts[n]->position().x > texts[n + 1]->position().x)
					{
						McDbText* tmp_text_ptr;
						tmp_text_ptr = texts[n];
						texts[n] = texts[n + 1];
						texts[n + 1] = tmp_text_ptr;
					}
				}
			}
		}

		while ((int)texts.size() >= 1)
		{
			std::vector<McDbText*> tmp_texts;

			McDbExtents ext_firstText;
			texts[0]->getGeomExtents(ext_firstText, false);
			double row = (ext_firstText.minPoint().y + ext_firstText.maxPoint().y) / 2;
			std::vector<McDbText*> this_row_texts;
			this_row_texts.emplace_back(texts[0]);
			for (int k = 1; k < (int)texts.size(); k++)
			{
				McDbExtents ext;
				texts[k]->getGeomExtents(ext, false);

				if (ext.minPoint().y <= ext_firstText.maxPoint().y && ext.maxPoint().y >= ext_firstText.minPoint().y)
				{
					this_row_texts.emplace_back(texts[k]);
				}
				else
				{
					tmp_texts.emplace_back(texts[k]);
				}
			}
			rows.insert(make_pair(row, this_row_texts));
			texts = tmp_texts;
		}

		for (auto it = rows.rbegin(); it != rows.rend(); it++)
		{
			QString this_row_strs;
			for (int k = 0; k < (int)it->second.size(); k++)
			{
				this_row_strs += QString::fromLocal8Bit(it->second[k]->textString());
			}
			if (it != --rows.rend())
			{
				text = text + this_row_strs + "\n";
			}
			else
			{
				text += this_row_strs;
			}
		}
		cell.texts = text;
	}

	for (int i = 0; i < (int)all_texts.size(); i++)
	{
		delete all_texts[i];
	}

	QString defaultDir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
	QString fileName = QCoreApplication::translate("MxCADCommand", "table.xlsx");
	QString filter = QCoreApplication::translate("MxCADCommand", "Spreadsheet files (*.xlsx *.xls);;All files (*)");

	QFileDialog saveFileDlg(nullptr, QCoreApplication::translate("MxCADCommand", "Save Spreadsheet File"), defaultDir, filter);
	saveFileDlg.setDefaultSuffix("xlsx");
	saveFileDlg.setFileMode(QFileDialog::AnyFile);
	saveFileDlg.setAcceptMode(QFileDialog::AcceptSave);
	saveFileDlg.setOption(QFileDialog::DontConfirmOverwrite, false);

	if (saveFileDlg.exec() != QDialog::Accepted)
	{
		return;
	}

	QString filePath = saveFileDlg.selectedFiles().first();

	lxw_workbook* workbook = workbook_new(filePath.toUtf8().constData());

	lxw_worksheet* worksheet = workbook_add_worksheet(workbook, NULL);

	if (set_ratio)
	{
		for (int i = 0; i < (int)newXcoords.size() - 1; i++)
		{
			worksheet_set_column(worksheet, i, i, (newXcoords[i + 1] - newXcoords[i]) * 1.5 / (text_height_avg * text_width_height_ratio_avg), NULL);
		}
		for (int i = (int)newYcoords.size() - 1; i > 0; i--)
		{
			worksheet_set_row(worksheet, (int)newYcoords.size() - i - 1, (newYcoords[i] - newYcoords[i - 1]) * 8 / text_height_avg, NULL);
		}
	}

	lxw_format* cell_format = workbook_add_format(workbook);
	format_set_align(cell_format, LXW_ALIGN_CENTER);
	format_set_align(cell_format, LXW_ALIGN_VERTICAL_CENTER);
	format_set_border(cell_format, LXW_BORDER_THIN);
	format_set_text_wrap(cell_format);
	format_set_font_name(cell_format, "Arial");
	format_set_font_size(cell_format, 10);

	for (int i = 0; i < (int)cellRects.size(); i++)
	{
		Mx2d::CellRect cell = cellRects[i];

		if (cell.rowStart == cell.rowEnd && cell.columnStart == cell.columnEnd)
		{
			worksheet_write_string(worksheet, (int)row.size() - cell.rowStart - 1, cell.columnStart, cell.texts.toUtf8().constData(), cell_format);
		}
		else
		{
			worksheet_merge_range(worksheet, (int)row.size() - cell.rowStart - 1, cell.columnStart, (int)row.size() - cell.rowEnd - 1, cell.columnEnd, cell.texts.toUtf8().constData(), cell_format);
		}
	}

	lxw_error err = workbook_close(workbook);
	if (LXW_NO_ERROR != err) {
		LOG_ERROR(QString("Failed to export table, please check if file is already open, close it and try again!"));
		// TODO: show error message
		QMessageBox::information(this, tr("Prompt"), tr("Failed to export table, please check if file is already open, close it and try again!"));
		return;
	}
}
