#include "MxLogger.h"
#include <QCoreApplication>
#include <QThread>

MxLogger* MxLogger::m_instance = nullptr;
QMutex MxLogger::m_mutex;

MxLogger::MxLogger(QObject *parent) : QObject(parent)
{
}

MxLogger::~MxLogger()
{
    if (m_logFile.isOpen()) {
        m_logStream.flush();
        m_logFile.close();
    }
}

MxLogger *MxLogger::instance()
{
    if (!m_instance) {
        QMutexLocker locker(&m_mutex);
        if (!m_instance) {
            m_instance = new MxLogger;
        }
    }
    return m_instance;
}

void MxLogger::init(const QString &logPath)
{
    QMutexLocker locker(&m_mutex);
    QString logPath_ = logPath;
    if (logPath == "")
    {
        logPath_ = QDir(QCoreApplication::applicationDirPath()).filePath("log");
    }
    QDir dir(logPath_);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    m_logFile.setFileName(logPath_ + "/log.txt");
    if (!m_logFile.open(QIODevice::Append | QIODevice::Text)) {
        return;
    }

    m_logStream.setDevice(&m_logFile);
    m_logStream.setCodec("UTF-8");

    qInstallMessageHandler(MxLogger::messageHandler);
}

void MxLogger::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QMutexLocker locker(&m_mutex);

    QString level;
    switch (type) {
    case QtDebugMsg:     level = "DEBUG"; break;
    case QtInfoMsg:      level = "INFO";  break;
    case QtWarningMsg:   level = "WARN";  break;
    case QtCriticalMsg:  level = "ERROR"; break;
    case QtFatalMsg:     level = "FATAL"; break;
    default:             level = "UNKNOWN";
    }

    QString timeStr = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString logContent = QString("[%1] [%2] %3")
                          .arg(timeStr)
                          .arg(level)
                          .arg(msg);

    fprintf(stderr, "%s\n", qPrintable(logContent));

    if (instance()->m_logFile.isOpen()) {
        instance()->m_logStream << logContent << endl;
        instance()->m_logStream.flush();
    }

    if (type == QtFatalMsg) {
        abort();
    }
}