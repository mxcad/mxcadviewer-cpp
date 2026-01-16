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

#include "Mx2dLayerManagerDialog.h"
#include "Mx2dMeasurementDialog.h"
#include "Mx2dLeaderTextInputDialog.h"
#include "Mx2dTextSearchDialog.h"
#include "Mx2dExtractTextDialog.h"

Mx2dGuiDocument::Mx2dGuiDocument(QWidget* parent) : QWidget(parent) {

	m_pViewWidget = new QWidget(this);
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

#include <QProcess>
#include <QJsonDocument>
void Mx2dGuiDocument::openFile(const QString& filePath) {
	
	QFileInfo fileInfo(filePath);
	QString suffix = fileInfo.suffix().toLower();
	if (suffix == "mxweb")
	{
		QtConcurrent::run([this, filePath]()
			{
				std::string sOpenFile;
#ifdef Q_OS_LINUX
				sOpenFile = filePath.toUtf8().toStdString();
#else
				sOpenFile = filePath.toLocal8Bit().toStdString();
#endif
				emit startReadFile();
				if (m_cadView.ReadFile(sOpenFile.c_str()))
				{
					m_filePath = filePath;
					emit fileRead(true);
					return;
				}
				emit fileRead(false);
			});
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
			QtConcurrent::run([this, outfilePath, filePath]()
				{
					
					std::string sOpenFile;
#ifdef Q_OS_LINUX
					sOpenFile = outfilePath.toUtf8().toStdString();
#else
					sOpenFile = outfilePath.toLocal8Bit().toStdString();
#endif
					emit startReadFile();
					if (m_cadView.ReadFile(sOpenFile.c_str()))
					{
						m_filePath = filePath;
						emit fileRead(true);
						return;
					}
					emit fileRead(false);
				});
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
			mxconvertPath = appPath + "/../3rdparty/mxconvert/mxconvert.exe";
			if(!QFile::exists(mxconvertPath)){
				emit runConverterFailed();
				return;
			}
		}

		QStringList arguments;
		arguments << "-s" << filePath << "-o" << outfilePath;
		process.setProgram(mxconvertPath);
		process.setArguments(arguments);
		process.setWorkingDirectory(QFileInfo(mxconvertPath).absolutePath());
		process.start();

		if (!process.waitForStarted(2000)) {
			LOG_ERROR(QString("failed start mxconvert convert program: %1").arg(process.errorString()));
			emit runConverterFailed();
			return;
		}

		if (process.waitForFinished(200000)) {
			QByteArray stdoutData = process.readAllStandardOutput();
			QByteArray stderrData = process.readAllStandardError();
			QJsonParseError parseError;
			QJsonDocument retJsonDoc = QJsonDocument::fromJson(stdoutData, &parseError);

			if (parseError.error != QJsonParseError::NoError) {
				emit convertFailed(parseError.errorString());
				return;
			}

			QJsonObject retJson = retJsonDoc.object();

			if (retJson.contains("message") && retJson["message"].toString() == "ok") {
				// open the file
				QtConcurrent::run([this, outfilePath, filePath]()
					{

						std::string sOpenFile;
#ifdef Q_OS_LINUX
						sOpenFile = outfilePath.toUtf8().toStdString();
#else
						sOpenFile = outfilePath.toLocal8Bit().toStdString();
#endif
						emit startReadFile();
						if (m_cadView.ReadFile(sOpenFile.c_str()))
						{
							m_filePath = filePath;
							emit fileRead(true);
							return;
						}
						emit fileRead(false);
					});

				return;
			}
		}
		else {
            LOG_ERROR(QString("mxconvert timeout or crashed: %1").arg(process.errorString()));
			emit convertFailed(process.errorString());
		}

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
