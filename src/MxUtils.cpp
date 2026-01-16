/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "MxUtils.h"
#include <QCoreApplication>
#include "MxCADViewerVersion.h"
#include "MxCADViewerBuildDate.h"
#include <QTextCodec>
#include <QNetworkRequest>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkReply>
#include "MxSignalTransfer.h"

#include <QFileInfo>
#include <QRegularExpression>
#include <regex>

#ifdef MX_BUILD_LOGIN
#include "MxNetworkManager.h"
#endif

namespace MxUtils {
	QString getAppVersion()
	{
		QString ret = QCoreApplication::translate("MxUtils", "MxCADViewer%1(%2)");
		return ret.arg(MXCADVIEWER_VERSION).arg(MXCADVIEWER_BUILD_DATE);
	}

	QString GB2312ToQString(const std::string& str) {
		QTextCodec* codec = QTextCodec::codecForName("GB2312");
		if (!codec) {
			codec = QTextCodec::codecForName("GBK");
		}
		if (!codec) {
			return QString::fromLatin1(str.data(), static_cast<int>(str.size()));
		}
		return codec->toUnicode(str.data(), static_cast<int>(str.size()));
	}

	std::string QStringToGB2312(const QString& qstr) {
		QTextCodec* codec = QTextCodec::codecForName("GB2312");
		if (!codec) {
			return "";
		}
		QByteArray encodedData = codec->fromUnicode(qstr);
		return std::string(encodedData.constData(), encodedData.length());
	}

	std::string QStringToUtf8(const QString& qstr) {

		return qstr.toUtf8().toStdString();

	}
	MxFormat getFileFormat(const QString& filePath)
	{
		MxFormat format = MxFormat::Format_Unknown;
		QString ext = QFileInfo(filePath).suffix().toLower();
		if (ext == "stp" || ext == "step")
		{
			format = MxFormat::Format_STEP;
		}
		else if (ext == "stl")
		{
			format = MxFormat::Format_STL;
		}
		else if (ext == "iges" || ext == "igs")
		{
			format = MxFormat::Format_IGES;
		}
		else if (ext == "gltf" || ext == "glb")
		{
			format = MxFormat::Format_GLTF;
		}
		else if (ext == "wrl" || ext == "wrz" || ext == "vrml")
		{
			format = MxFormat::Format_VRML;
		}
		else if (ext == "obj")
		{
			format = MxFormat::Format_OBJ;
		}
		else if (ext == "brep" || ext == "rle" || ext == "occ")
		{
			format = MxFormat::Format_OCCBREP;
		}
		else if (ext == "dwg")
		{
			format = MxFormat::Format_DWG;
		}
		else if (ext == "dxf")
		{
			format = MxFormat::Format_DXF;
		}
		else if (ext == "dwf")
		{
			format = MxFormat::Format_DWF;
		}
		else if (ext == "mxweb")
		{
			format = MxFormat::Format_MXWEB;
		}
		return format;
	}

	QString getOsReleaseValue(const QString& key)
	{
		QFile file("/etc/os-release");
		if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			QTextStream in(&file);
			while (!in.atEnd()) {
				QString line = in.readLine();
				QRegularExpression re(QString("^%1=(.*)$").arg(key));
				QRegularExpressionMatch match = re.match(line);
				if (match.hasMatch()) {
					QString value = match.captured(1);
					if (value.startsWith("\"") && value.endsWith("\"")) {
						value = value.mid(1, value.length() - 2);
					}
					else if (value.startsWith("'") && value.endsWith("'")) {
						value = value.mid(1, value.length() - 2);
					}
					return value;
				}
			}
			file.close();
		}
		return QString();
	}
	bool isSpecialSystem(const QString& sysName)
	{
		QString id = getOsReleaseValue("ID");
		return id.contains(sysName, Qt::CaseInsensitive);
	}


	bool isValidPhoneNumber(const QString& phoneNumber) {
		// Start with 1, followed by a digit between [345789], followed by 9 digits
		std::regex pattern(R"(^1[345789]\d{9}$)");
		return std::regex_match(phoneNumber.toStdString(), pattern);
	}

	bool isValidPassword(const QString& password) {
		std::regex lowerCasePattern("[a-z]");
		std::regex upperCasePattern("[A-Z]");

		if ((std::regex_search(password.toStdString(), lowerCasePattern) || std::regex_search(password.toStdString(), upperCasePattern))
			&& password.length() >= 5) {
			return true;
		}
		return false;
	}

	void drawScale9Pixmap(QPainter& painter, const QPixmap& pixmap, const QRect& targetRect, int leftMargin, int topMargin, int rightMargin, int bottomMargin)
	{
		int srcWidth = pixmap.width();
		int srcHeight = pixmap.height();
		int dstWidth = targetRect.width();
		int dstHeight = targetRect.height();

		painter.drawPixmap(targetRect.left(), targetRect.top(), leftMargin, topMargin,
			pixmap, 0, 0, leftMargin, topMargin);

		painter.drawPixmap(targetRect.left() + leftMargin, targetRect.top(),
			dstWidth - leftMargin - rightMargin, topMargin,
			pixmap, leftMargin, 0, srcWidth - leftMargin - rightMargin, topMargin);

		painter.drawPixmap(targetRect.right() - rightMargin, targetRect.top(),
			rightMargin, topMargin,
			pixmap, srcWidth - rightMargin, 0, rightMargin, topMargin);

		painter.drawPixmap(targetRect.left(), targetRect.top() + topMargin,
			leftMargin, dstHeight - topMargin - bottomMargin,
			pixmap, 0, topMargin, leftMargin, srcHeight - topMargin - bottomMargin);

		painter.drawPixmap(targetRect.left() + leftMargin, targetRect.top() + topMargin,
			dstWidth - leftMargin - rightMargin, dstHeight - topMargin - bottomMargin,
			pixmap, leftMargin, topMargin,
			srcWidth - leftMargin - rightMargin, srcHeight - topMargin - bottomMargin);

		painter.drawPixmap(targetRect.right() - rightMargin, targetRect.top() + topMargin,
			rightMargin, dstHeight - topMargin - bottomMargin,
			pixmap, srcWidth - rightMargin, topMargin,
			rightMargin, srcHeight - topMargin - bottomMargin);

		painter.drawPixmap(targetRect.left(), targetRect.bottom() - bottomMargin,
			leftMargin, bottomMargin,
			pixmap, 0, srcHeight - bottomMargin, leftMargin, bottomMargin);

		painter.drawPixmap(targetRect.left() + leftMargin, targetRect.bottom() - bottomMargin,
			dstWidth - leftMargin - rightMargin, bottomMargin,
			pixmap, leftMargin, srcHeight - bottomMargin,
			srcWidth - leftMargin - rightMargin, bottomMargin);

		painter.drawPixmap(targetRect.right() - rightMargin, targetRect.bottom() - bottomMargin,
			rightMargin, bottomMargin,
			pixmap, srcWidth - rightMargin, srcHeight - bottomMargin,
			rightMargin, bottomMargin);
	}

	QPixmap roundedPixmap(const QPixmap& source) {
		int radius = qMin(source.width(), source.height()) / 2;
		QPixmap rounded = QPixmap(source.size());
		rounded.fill(Qt::transparent);

		QPainter painter(&rounded);
		painter.setRenderHint(QPainter::Antialiasing);

		QPainterPath path;
		path.addRoundedRect(rounded.rect(), radius, radius);
		painter.setClipPath(path);
		painter.drawPixmap(0, 0, source);

		return rounded;
	}

	int doAction(std::function<void()> func)
	{
#ifdef MX_BUILD_LOGIN
		return MxNetworkManager::getInstance().doVipFunction(func);
#else
		func();
		return 2;
#endif
	}

	QWidget* gCurrentTab = nullptr;
}
