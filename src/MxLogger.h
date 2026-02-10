/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/


#pragma once

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QDateTime>
#include <QString>
#include <QDir>

class MxLogger : public QObject
{
    Q_OBJECT
public:
    static MxLogger* instance();

    void init(const QString& logPath = "");

    static void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

private:
    explicit MxLogger(QObject *parent = nullptr);
    ~MxLogger() override;

    MxLogger(const MxLogger&) = delete;
    MxLogger& operator=(const MxLogger&) = delete;

    static MxLogger* m_instance;
    static QMutex m_mutex;
    QFile m_logFile;
    QTextStream m_logStream;
};

inline QString getFileName(const QString& fullPath) {
	return fullPath.split(QDir::separator()).last();
}

#define LOG_DEBUG(msg) qDebug("[%s:%d] %s", qPrintable(getFileName(__FILE__)), __LINE__, msg.toUtf8().constData())
#define LOG_INFO(msg) qInfo("[%s:%d] %s", qPrintable(getFileName(__FILE__)), __LINE__, msg.toUtf8().constData())
#define LOG_WARN(msg) qWarning("[%s:%d] %s", qPrintable(getFileName(__FILE__)), __LINE__, msg.toUtf8().constData())
#define LOG_ERROR(msg) qCritical("[%s:%d] %s", qPrintable(getFileName(__FILE__)), __LINE__, msg.toUtf8().constData())
#define LOG_FATAL(msg) qFatal("[%s:%d] %s", qPrintable(getFileName(__FILE__)), __LINE__, msg.toUtf8().constData())

#define LOG_DEBUG_F(fmt, ...) qDebug("[%s:%d] " fmt, qPrintable(getFileName(__FILE__)), __LINE__, ##__VA_ARGS__)
#define LOG_INFO_F(fmt, ...) qInfo("[%s:%d] " fmt, qPrintable(getFileName(__FILE__)), __LINE__, ##__VA_ARGS__)
#define LOG_WARN_F(fmt, ...) qWarning("[%s:%d] " fmt, qPrintable(getFileName(__FILE__)), __LINE__, ##__VA_ARGS__)
#define LOG_ERROR_F(fmt, ...) qCritical("[%s:%d] " fmt, qPrintable(getFileName(__FILE__)), __LINE__, ##__VA_ARGS__)
#define LOG_FATAL_F(fmt, ...) qFatal("[%s:%d] " fmt, qPrintable(getFileName(__FILE__)), __LINE__, ##__VA_ARGS__)