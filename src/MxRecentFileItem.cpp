/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "MxRecentFileItem.h"
#include <QFileInfo>
#include <QTimer>
#include <QDebug>
#include <QPainter>
#include <QPaintEvent>
#include "MxRecentFileManager.h"
#include "MxSignalTransfer.h"
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>

MxRecentFileItem::MxRecentFileItem(const MxRecentFile& recentFile, QWidget* parent)
    : QWidget(parent)
    , m_recentFile(recentFile)
    // Initialize pointers to nullptr
    , widget_topContainer(nullptr)
    , label_thumbNail(nullptr)
    , btn_open(nullptr)
    , btn_remove(nullptr)
    , label_fileName(nullptr)
    , widget_bottomContainer(nullptr)
    , layout_bottom(nullptr)
    , label_openTime(nullptr)
    , label_fileSize(nullptr)
{
    initUi();

    QPixmap pixmap;
    pixmap.loadFromData(m_recentFile.thumbnail, "PNG");
    pixmap = roundedPixmap(pixmap, 12);
    label_thumbNail->setPixmap(pixmap);
    label_fileName->setToolTip(m_recentFile.filePath);
    QFileInfo fileInfo(m_recentFile.filePath);

    QFontMetrics fm(label_fileName->font());
    // 16 * 3 - 17 calculation preserved from original
    QString elidedText = fm.elidedText(fileInfo.fileName(), Qt::ElideRight, label_thumbNail->width() - 48 - 17);

    label_fileName->setText(elidedText);

    label_openTime->setText(m_recentFile.lastOpened.toString("yyyy-MM-dd HH:mm:ss"));
    // _recentFile.fileSize eg., 100KB, 10MB, 1GB
    label_fileSize->setText(convertFileSize(m_recentFile.fileSize));

    btn_open->hide();
    btn_remove->hide();

    connect(btn_open, &QPushButton::clicked, [=]() {
        emit MxSignalTransfer::getInstance().signalOpenFile(m_recentFile.filePath);
        });

    connect(btn_remove, &QPushButton::clicked, [=]() {
        MxRecentFileManager::instance()->removeFile(m_recentFile.filePath);
        });
}

MxRecentFileItem::~MxRecentFileItem()
{
}

void MxRecentFileItem::initUi()
{
    if (objectName().isEmpty())
        setObjectName("RecentFileItem");

    resize(255, 260);
    setMinimumSize(QSize(255, 260));
    setMaximumSize(QSize(255, 260));


    // --- Top Container Widget ---
    widget_topContainer = new QWidget(this);
    widget_topContainer->setObjectName("widget");
    widget_topContainer->setGeometry(QRect(0, 0, 255, 175));
    widget_topContainer->setMinimumSize(QSize(0, 175));
    widget_topContainer->setMaximumSize(QSize(16777215, 175));
    widget_topContainer->setStyleSheet(R"(
        QWidget {
            padding: 0px;
            margin: 0px;
        }
    )");

    // --- Thumbnail Label ---
    label_thumbNail = new QLabel(widget_topContainer);
    label_thumbNail->setObjectName("label_thumbNail");
    label_thumbNail->setGeometry(QRect(10, 10, 235, 165));
    label_thumbNail->setMinimumSize(QSize(235, 165));
    label_thumbNail->setMaximumSize(QSize(235, 165));
    label_thumbNail->setCursor(QCursor(Qt::ArrowCursor));
    label_thumbNail->setStyleSheet(R"(
        QLabel { 
            border-top-left-radius: 20px;
            border-top-right-radius: 20px;
        }
    )");
    label_thumbNail->setScaledContents(true);

    // Shared Button Style
    QString btnStyle = R"(
        QPushButton {
            border: none;
            border-radius: 4px;
            background-color: rgba( 61, 73, 80, 100%);
            color: #FEFEFE;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: rgba( 61, 73, 80, 80%);
        }
        QPushButton:pressed {
            background-color: rgba( 61, 73, 80, 60%);
        }
    )";

    // --- Open Button ---
    btn_open = new QPushButton(widget_topContainer);
    btn_open->setObjectName("btn_open");
    btn_open->setGeometry(QRect(90, 60, 80, 30));
    btn_open->setStyleSheet(btnStyle);
    btn_open->setText(tr("Open"));

    // --- Remove Button ---
    btn_remove = new QPushButton(widget_topContainer);
    btn_remove->setObjectName("btn_remove");
    btn_remove->setGeometry(QRect(90, 100, 80, 30));
    btn_remove->setStyleSheet(btnStyle);
    btn_remove->setText(tr("Remove"));

    // --- File Name Label ---
    label_fileName = new QLabel(this);
    label_fileName->setObjectName("label_fileName");
    label_fileName->setGeometry(QRect(0, 180, 255, 25));
    label_fileName->setMinimumSize(QSize(0, 25));
    label_fileName->setMaximumSize(QSize(16777215, 25));
    label_fileName->setStyleSheet(R"(
        QLabel { 
            font-size: 16px; 
            margin-left: 15px;
            margin-right: 5px;
            color: #003D37;
        }
        QToolTip { background-color: white; color: black; border: 1px solid gray; }
    )");
    label_fileName->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);
    label_fileName->setWordWrap(false);

    // --- Bottom Info Container ---
    widget_bottomContainer = new QWidget(this);
    widget_bottomContainer->setObjectName("layoutWidget");
    widget_bottomContainer->setGeometry(QRect(0, 210, 251, 40));

    layout_bottom = new QHBoxLayout(widget_bottomContainer);
    layout_bottom->setObjectName("horizontalLayout");
    layout_bottom->setContentsMargins(0, 0, 0, 0);

    // --- Open Time Label ---
    label_openTime = new QLabel(widget_bottomContainer);
    label_openTime->setObjectName("label_openTime");
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(label_openTime->sizePolicy().hasHeightForWidth());
    label_openTime->setSizePolicy(sizePolicy);
    label_openTime->setMinimumSize(QSize(0, 24));
    label_openTime->setMaximumSize(QSize(16777215, 24));
    label_openTime->setStyleSheet(R"(
        QLabel {
            font-size: 14px;
            margin-left: 15px;
            color: #003D37;
        }
    )");
    layout_bottom->addWidget(label_openTime);

    // --- File Size Label ---
    label_fileSize = new QLabel(widget_bottomContainer);
    label_fileSize->setObjectName("label_fileSize");
    QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Preferred);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(label_fileSize->sizePolicy().hasHeightForWidth());
    label_fileSize->setSizePolicy(sizePolicy1);
    label_fileSize->setMinimumSize(QSize(0, 24));
    label_fileSize->setMaximumSize(QSize(16777215, 24));
    label_fileSize->setStyleSheet(R"(
        QLabel {
            font-size: 14px;
            margin-right: 15px;
            color: #003D37;
        }
    )");
    label_fileSize->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);
    layout_bottom->addWidget(label_fileSize);
}

void MxRecentFileItem::enterEvent(QEvent* event)
{
    btn_open->show();
    btn_remove->show();
}

void MxRecentFileItem::leaveEvent(QEvent* event)
{
    btn_open->hide();
    btn_remove->hide();
}

void MxRecentFileItem::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, this->width(), this->height(), QPixmap(":/resources/images/recentFileItemBg.png"));
    event->accept();
}

MxRecentFile MxRecentFileItem::getRecentFile() const
{
    return m_recentFile;
}

bool MxRecentFileItem::contains(const QString& str) const
{
    QFileInfo fileInfo(m_recentFile.filePath);
    QString fileName = fileInfo.fileName();

    if (fileName.contains(str, Qt::CaseInsensitive)) {
        return true;
    }
    return false;
}

QString MxRecentFileItem::convertFileSize(qint64 fileSize)
{
    QStringList units = { "B", "KB", "MB", "GB", "TB" };
    int unitIndex = 0;
    double size = static_cast<double>(fileSize);

    while (size >= 1024 && unitIndex < units.size() - 1) {
        size /= 1024;
        unitIndex++;
    }

    return QString("%1 %2").arg(size, 0, 'f', 1).arg(units[unitIndex]);
}

QPixmap MxRecentFileItem::roundedPixmap(const QPixmap& source, int radius)
{
    if (source.isNull())
        return QPixmap();

    QPixmap result(source.size());
    result.setDevicePixelRatio(source.devicePixelRatio());
    result.fill(Qt::transparent);

    QPainter painter(&result);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    const int w = source.width();
    const int h = source.height();

    QPainterPath path;
    path.moveTo(radius, 0);
    path.arcTo(0, 0, 2 * radius, 2 * radius, 90, 90);
    path.lineTo(0, h);
    path.lineTo(w, h);
    path.lineTo(w, radius);
    path.arcTo(w - 2 * radius, 0, 2 * radius, 2 * radius, 0, 90);
    path.closeSubpath();

    painter.setClipPath(path);
    painter.drawPixmap(0, 0, source);

    return result;
}