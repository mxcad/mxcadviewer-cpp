/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "MxProgressDialog.h"
#include "ClickableLabel.h"

#include <QProgressBar>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include "MxSignalTransfer.h"
MxProgressDialog::MxProgressDialog(QWidget* parent)
    : QDialog(parent)
{
    setupUi();
    connectSignals();
}

MxProgressDialog::~MxProgressDialog()
{
}

void MxProgressDialog::setupUi()
{
    setWindowTitle(tr("Opening..."));
    setFixedSize(500, 200);

    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);


    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);


    auto* iconLabel = new QLabel();
    iconLabel->setPixmap(QPixmap(":/resources/images/opening.svg"));
    iconLabel->setScaledContents(false); // Do not scale the icon
    iconLabel->setAlignment(Qt::AlignCenter);


    m_detailLabel = new QLabel(tr("Reading..."));
    m_detailLabel->setAlignment(Qt::AlignCenter);
    m_detailLabel->setWordWrap(true);


    auto* progressLayout = new QHBoxLayout();
    m_progressBar = new QProgressBar();
    m_progressBar->setMinimumSize(250, 24);
    m_progressBar->setTextVisible(false); // A cleaner look

    m_abortLabel = new ClickableLabel();
    m_abortLabel->setCursor(Qt::PointingHandCursor);
    m_abortLabel->setPixmap(QPixmap(":/resources/images/abort.svg"));
    m_abortLabel->setToolTip(tr("Abort"));

    progressLayout->addStretch();
    progressLayout->addWidget(m_progressBar);
    progressLayout->addSpacing(10);
    progressLayout->addWidget(m_abortLabel);
    progressLayout->addStretch();


    mainLayout->addWidget(iconLabel);
    mainLayout->addStretch(1);
    mainLayout->addWidget(m_detailLabel);
    mainLayout->addLayout(progressLayout);
    mainLayout->addStretch(2);
}

void MxProgressDialog::connectSignals()
{

    connect(m_abortLabel, &ClickableLabel::clicked, this, &MxProgressDialog::abort);
}

void MxProgressDialog::start()
{
    m_detailLabel->setText("");
    m_progressBar->setValue(0);
    // exec() makes the dialog modal
    this->exec();
}

void MxProgressDialog::stop()
{
    this->close();
}

void MxProgressDialog::setProgressDetail(const QString& detail)
{
    m_detailLabel->setText(detail);
}

void MxProgressDialog::setProgressPercent(int percent)
{
    m_progressBar->setValue(percent);
}