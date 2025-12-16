/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once
#include <QWidget>
#include <QString>
#include <QStringList>
#include "MxRecentFile.h"
#include <QPixmap>

class QLabel;
class QPushButton;
class QHBoxLayout;

class MxRecentFileItem : public QWidget
{
    Q_OBJECT

public:
    MxRecentFileItem(const MxRecentFile& recentFile, QWidget* parent = nullptr);
    ~MxRecentFileItem();

protected:
    virtual void enterEvent(QEvent* event) override;
    virtual void leaveEvent(QEvent* event) override;
    virtual void paintEvent(QPaintEvent* event) override;

private:
    void initUi();

    MxRecentFile m_recentFile;

public:
    MxRecentFile getRecentFile() const;
    bool contains(const QString& str) const;

private:
    QString convertFileSize(qint64 fileSize);
    QPixmap roundedPixmap(const QPixmap& source, int radius);

private:
    QWidget* widget_topContainer;
    QLabel* label_thumbNail;
    QPushButton* btn_open;
    QPushButton* btn_remove;
    QLabel* label_fileName;
    QWidget* widget_bottomContainer;
    QHBoxLayout* layout_bottom;
    QLabel* label_openTime;
    QLabel* label_fileSize;
};