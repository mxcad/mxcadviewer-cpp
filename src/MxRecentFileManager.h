/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once
#include <QList>
#include <QSettings>
#include "MxRecentFile.h"

class MxRecentFileManager : public QObject
{
	Q_OBJECT
public:
	static MxRecentFileManager* instance();

	MxRecentFileManager(const MxRecentFileManager&) = delete;
	MxRecentFileManager& operator=(const MxRecentFileManager&) = delete;

	void addRecentFile(const MxRecentFile& newFile);
	void loadFromSettings();
	void saveToSettings();
	QList<MxRecentFile> getRecentFiles() const;
	void clear();
	void removeFile(const QString& filePath);

signals:
	void recentFilesChanged();

private:
	explicit MxRecentFileManager() = default;
	~MxRecentFileManager() = default;

	QList<MxRecentFile>		m_recentFiles;
	QSettings				m_settings{ "MxDraw", "MxCADViewer" };
};