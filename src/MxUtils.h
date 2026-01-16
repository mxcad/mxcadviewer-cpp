/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QString>
#include <functional>
#include <QPainter>

class QNetworkReply;
class QWidget;
namespace MxUtils {
	QString getAppVersion();
	QString GB2312ToQString(const std::string& str);
	std::string QStringToGB2312(const QString& qstr);
	std::string QStringToUtf8(const QString& qstr);


	enum class MxFormat
	{
		Format_Unknown,
		Format_DXF,
		Format_DWG,
		Format_DWF,
		Format_MXWEB,
		Format_GLTF,
		Format_IGES,
		Format_OBJ,
		Format_OCCBREP,
		Format_STEP,
		Format_STL,
		Format_VRML
	};

	MxFormat getFileFormat(const QString& filePath);


	QString getOsReleaseValue(const QString& key);

	bool isSpecialSystem(const QString& sysName);


	bool isValidPhoneNumber(const QString& phoneNumber);
	bool isValidPassword(const QString& password);
	void drawScale9Pixmap(QPainter& painter, const QPixmap& pixmap, const QRect& targetRect, int leftMargin, int topMargin, int rightMargin, int bottomMargin);

	QPixmap roundedPixmap(const QPixmap& source);

	int doAction(std::function<void()> func);

	extern QWidget* gCurrentTab;
}