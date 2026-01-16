/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "MxCADViewer.h"
#include <QtWidgets/QApplication>
#include <QStyleFactory>
#include <QFontDatabase>
#include <QDebug>
#include "MxRecentFile.h"
#include "MxRecentFileManager.h"
#include <QFileInfo>
#include <QTranslator>
#include "MxLanguageManager.h"
#include "MxLogger.h"

#include "MxCADInclude.h"
#include "MxOpenGLView.h"
#include "Mx2dCustomAnnotation.h"
#include "MxCADCommand.h"
#include "Mx2dCustomRectCloud.h"
#include "Mx2dCustomRect.h"
#include "Mx2dCustomLine.h"
#include "Mx2dCustomEllipse.h"
#include "Mx2dCustomLeader.h"
#include "Mx2dCustomText.h"
#include "Mx2dCustomMText.h"
#include "Mx2dCustomPolyArea.h"
#include "Mx2dCustomRectArea.h"
#include "Mx2dCustomCartesianCoord.h"
#include "Mx2dCustomAlignedDim.h"
#include "Mx2dCustomLinearDim.h"
#include "Mx2dCustomContinuousMeasurement.h"
#include "Mx2dCustomRadiusDim.h"
#include "Mx2dCustomArcLengthDim.h"
#include "Mx2dCustomCircleMeasurement.h"
#include "Mx2dCustomAngleMeasurement.h"
#include "Mx2dCustomArcPolyArea.h"
#include "Mx2dCustomHatchArea.h"
#include "Mx2dCustomHatchArea2.h"
#include "Mx2dCustomBatchMeasurement.h"

QDataStream& operator<<(QDataStream& out, const MxRecentFile& file) {
	out << file.filePath
		<< file.lastOpened
		<< file.fileType
		<< file.fileSize
		<< file.thumbnail;
	return out;
}

QDataStream& operator>>(QDataStream& in, MxRecentFile& file) {
	in >> file.filePath
		>> file.lastOpened
		>> file.fileType
		>> file.fileSize
		>> file.thumbnail;
	return in;
}

int main(int argc, char* argv[]) {
#if defined(Q_OS_WIN)
	QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
#endif

	qRegisterMetaType<MxRecentFile>();
	qRegisterMetaTypeStreamOperators<MxRecentFile>();
	qRegisterMetaType<QList<MxRecentFile>>();
	qRegisterMetaTypeStreamOperators<QList<MxRecentFile>>();
	QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
	QApplication a(argc, argv);
	MxLogger::instance()->init(QDir(QCoreApplication::applicationDirPath()).filePath("log"));
	// ==================== Load local translation ====================

    QString locale = MxLanguageManager::getInstance().getLanguage();
	QTranslator mxcadviewerTranslator;
	if (mxcadviewerTranslator.load(QString(":/resources/translations/MxCADViewer_%1.qm").arg(locale)))
	{
		a.installTranslator(&mxcadviewerTranslator);
	}
	QTranslator qtTranslator;
	if (qtTranslator.load(QString(":/resources/translations/qt_%1.qm").arg(locale)))
	{
		a.installTranslator(&qtTranslator);
	}
	QTranslator qtbaseTranslator;
	if (qtbaseTranslator.load(QString(":/resources/translations/qtbase_%1.qm").arg(locale)))
	{
		a.installTranslator(&qtbaseTranslator);
	}

	// Global style
	a.setStyle(QStyleFactory::create("Fusion"));

	// Load recent file
	MxRecentFileManager::instance()->loadFromSettings();
	int exitCode = 0;
	{
		MxCADApp mxapp;
		MxCADCommand::RegisterCommand();
		// Init custom annotation
		Mx2dCustomAnnotation::rxInit();
		Mx2dCustomRectCloud::rxInit();
		Mx2dCustomRect::rxInit();
		Mx2dCustomLine::rxInit();
		Mx2dCustomEllipse::rxInit();
		Mx2dCustomLeader::rxInit();
		Mx2dCustomText::rxInit();
		Mx2dCustomMText::rxInit();
		Mx2dCustomPolyArea::rxInit();
		Mx2dCustomRectArea::rxInit();
		Mx2dCustomCartesianCoord::rxInit();
		Mx2dCustomAlignedDim::rxInit();
		Mx2dCustomLinearDim::rxInit();
		Mx2dCustomContinuousMeasurement::rxInit();
		Mx2dCustomRadiusDim::rxInit();
		Mx2dCustomArcLengthDim::rxInit();
		Mx2dCustomCircleMeasurement::rxInit();
		Mx2dCustomAngleMeasurement::rxInit();
		Mx2dCustomArcPolyArea::rxInit();
		Mx2dCustomHatchArea::rxInit();
		Mx2dCustomHatchArea2::rxInit();
        Mx2dCustomBatchMeasurement::rxInit();
		
		{

			MxCADViewer mainWindow;
			mainWindow.showMaximized();


			QStringList cmdLineFiles;
			for (int i = 1; i < a.arguments().size(); ++i) {
				QFileInfo fileInfo(a.arguments().at(i));
				if (fileInfo.isFile()) {
					cmdLineFiles.append(fileInfo.absoluteFilePath());
				}
			}
			if (!cmdLineFiles.isEmpty()) {
				mainWindow.onOpenFile(cmdLineFiles.first());
			}


			exitCode = a.exec();

		}


#ifdef MXLINUX
		MxOpenGLDriver::destroyInstance();
#endif
		mxapp.Free();

	}

	return exitCode;
}