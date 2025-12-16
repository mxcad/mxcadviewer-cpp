/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "MxRecentFileManager.h"


MxRecentFileManager* MxRecentFileManager::instance()
{
	static MxRecentFileManager instance;
	return &instance;
}

void MxRecentFileManager::addRecentFile(const MxRecentFile& newFile)
{
	auto it = std::find_if(m_recentFiles.begin(), m_recentFiles.end(),
		[&](const MxRecentFile& f) { return f.filePath == newFile.filePath; });

	if (it != m_recentFiles.end()) {
		*it = newFile;
	}
	else {
		m_recentFiles.prepend(newFile);
	}

	// most 20
	while (m_recentFiles.size() > 20) {
		m_recentFiles.removeLast();
	}
	saveToSettings();
	loadFromSettings();
	emit recentFilesChanged();
}

void MxRecentFileManager::loadFromSettings()
{
	QVariant recentFiles = m_settings.value("RecentFiles");
	if (recentFiles.canConvert<QList<MxRecentFile>>()) {
		m_recentFiles = recentFiles.value<QList<MxRecentFile>>();
	}
}

void MxRecentFileManager::saveToSettings()
{
	m_settings.setValue("RecentFiles", QVariant::fromValue(m_recentFiles));
}

QList<MxRecentFile> MxRecentFileManager::getRecentFiles() const
{
	return m_recentFiles;
}

void MxRecentFileManager::clear()
{
	m_recentFiles.clear();
	saveToSettings();
	loadFromSettings();
	emit recentFilesChanged();
}

void MxRecentFileManager::removeFile(const QString& filePath)
{
	auto it = std::find_if(m_recentFiles.begin(), m_recentFiles.end(),
		[&](const MxRecentFile& f) { return f.filePath == filePath; });

	if (it != m_recentFiles.end()) {
		m_recentFiles.removeAt(it - m_recentFiles.begin());
		saveToSettings();
		loadFromSettings();
		emit recentFilesChanged();
	}
}
