/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QString>
#include <QDateTime>
#include <QByteArray>
#include <QMetaType>
#include <QBuffer>

struct MxRecentFile
{
	QString filePath;
	QDateTime lastOpened;
	QString fileType;
	qint64 fileSize;
	QByteArray thumbnail;

	bool operator==(const MxRecentFile& other) const;
};

Q_DECLARE_METATYPE(MxRecentFile)
Q_DECLARE_METATYPE(QList<MxRecentFile>)
