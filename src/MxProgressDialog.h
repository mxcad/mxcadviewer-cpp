/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QDialog>


QT_BEGIN_NAMESPACE
class QProgressBar;
class QLabel;
QT_END_NAMESPACE

class ClickableLabel;

/**
 * @class MxProgressDialog
 * @brief A modal dialog that displays the progress of a long-running operation.
 *
 * It shows a progress bar, a detail text label, and an icon to abort the operation.
 * The dialog is controlled via public slots.
 */
class MxProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MxProgressDialog(QWidget* parent = nullptr);
    ~MxProgressDialog() override;

public slots:
    void start();
    void stop();
    void setProgressDetail(const QString& detail);
    void setProgressPercent(int percent);

private:
    void setupUi();
    void connectSignals();

private:

    QLabel* m_detailLabel;
    QProgressBar* m_progressBar;
    ClickableLabel* m_abortLabel;

signals:
	void abort();
};