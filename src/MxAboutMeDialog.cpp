/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "MxAboutMeDialog.h"
#include "Clickablelabel.h"
#include "MxUtils.h"
#include <QApplication>
#include <QDesktopServices>
#include <QDir>
#include <QGraphicsDropShadowEffect>
#include <QMessageBox>
#include <QMouseEvent>
#include <QSpacerItem>
#include <QUrl>

MxAboutMeDialog::MxAboutMeDialog(QWidget* parent)
    : QDialog(parent)
    , m_isDragging(false)
{
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    this->setAttribute(Qt::WA_TranslucentBackground);

    setupUi();

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setOffset(0, 0);
    shadow->setColor(QColor("#272822"));
    shadow->setBlurRadius(30);
    this->setGraphicsEffect(shadow);

    adjustSize();
    connectSignalsSlots();
}

MxAboutMeDialog::~MxAboutMeDialog()
{
}

void MxAboutMeDialog::connectSignalsSlots()
{
    connect(m_labelAboutMe, &ClickableLabel::clicked, this, [this]() {
        setCurrentTab(0);
        });

    connect(m_labelAbout3rdParty, &ClickableLabel::clicked, this, [this]() {
        setCurrentTab(1);
        });

    connect(m_buttonClose, &QPushButton::clicked, this, &MxAboutMeDialog::on_button_close_clicked);
    connect(m_labelOpenQtLicense, &QLabel::linkActivated, this, &MxAboutMeDialog::onOpenQtLicense);
    connect(m_labelOpenOcctLicense, &QLabel::linkActivated, this, &MxAboutMeDialog::onOpenOcctLicense);
    connect(m_labelGetSourceCode, &QLabel::linkActivated, this, &MxAboutMeDialog::onOpenSourceAddressFile);
}

void MxAboutMeDialog::setCurrentTab(int index)
{
    if (index == 0) {
        m_labelAboutMe->setStyleSheet(m_tabSelectStyle);
        m_labelAbout3rdParty->setStyleSheet(m_tabUnselectStyle);
    }
    else {
        m_labelAboutMe->setStyleSheet(m_tabUnselectStyle);
        m_labelAbout3rdParty->setStyleSheet(m_tabSelectStyle);
    }
    m_stackedWidget->setCurrentIndex(index);
}


void MxAboutMeDialog::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_widgetHeader->underMouse()) {
        m_isDragging = true;
        m_mouseStartPos = event->globalPos();
        m_windowStartPos = this->frameGeometry().topLeft();
        this->setMouseTracking(true);
        this->grabMouse();
        event->accept();
    }
    else {
        QDialog::mousePressEvent(event);
    }
}

void MxAboutMeDialog::mouseMoveEvent(QMouseEvent* event)
{
    if (m_isDragging && (event->buttons() & Qt::LeftButton)) {
        QPoint delta = event->globalPos() - m_mouseStartPos;
        this->move(m_windowStartPos + delta);
        event->accept();
    }
    else {
        QDialog::mouseMoveEvent(event);
    }
}

void MxAboutMeDialog::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_isDragging) {
        m_isDragging = false;
        this->releaseMouse();
        this->setMouseTracking(false);
        event->accept();
    }
    else {
        QDialog::mouseReleaseEvent(event);
    }
}


void MxAboutMeDialog::setupUi()
{
    this->resize(392, 374);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setObjectName(QStringLiteral("verticalLayout"));

    setupHeader(mainLayout);

    setupBody(mainLayout);

    mainLayout->setStretch(1, 1);

    setCurrentTab(0);
}

void MxAboutMeDialog::setupHeader(QVBoxLayout* mainLayout)
{
    m_widgetHeader = new QWidget(this);
    m_widgetHeader->setObjectName(QStringLiteral("widget_header"));
    m_widgetHeader->setStyleSheet(QStringLiteral(
        "#widget_header {"
        "background-color: rgb( 36, 50, 76);"
        "border-top-left-radius: 12px;"
        "border-top-right-radius: 12px;"
        "border: 1px solid #3C968E;"
        "border-bottom: none;"
        "}"
    ));

    QHBoxLayout* headerLayout = new QHBoxLayout(m_widgetHeader);
    headerLayout->setSpacing(6);
    headerLayout->setContentsMargins(5, 0, 0, 0);

    m_labelMxlogo = new QLabel(m_widgetHeader);
    m_labelMxlogo->setFixedSize(32, 32);
    m_labelMxlogo->setStyleSheet(QStringLiteral("margin: 2px;"));
    m_labelMxlogo->setPixmap(QPixmap(QStringLiteral(":/resources/images/mxlogo.svg")));
    m_labelMxlogo->setScaledContents(true);
    headerLayout->addWidget(m_labelMxlogo);

    m_labelWindowTitle = new QLabel(tr("About"), m_widgetHeader);
    m_labelWindowTitle->setStyleSheet(QStringLiteral("color: #D0D0D0;"));
    headerLayout->addWidget(m_labelWindowTitle);

    headerLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    m_buttonClose = new QPushButton(m_widgetHeader);
    m_buttonClose->setFixedSize(50, 44);
    m_buttonClose->setStyleSheet(QStringLiteral(
        "QPushButton {"
        "	border: none;"
        "	padding: 6px;"
        "	padding-left: 12px;"
        "	border-top-right-radius: 12px;"
        "   border-top: 1px solid #3C968E;"
        "   border-right: 1px solid #3C968E;"
        "	image: url(:/resources/images/close_normal.svg);"
        "}"
        "QPushButton:hover {"
        "	background-color: #C42B1C;"
        "	image: url(:/resources/images/close_hover.svg);"
        "}"
    ));
    headerLayout->addWidget(m_buttonClose);

    mainLayout->addWidget(m_widgetHeader);
}

void MxAboutMeDialog::setupBody(QVBoxLayout* mainLayout)
{
    m_widgetBody = new QWidget(this);
    m_widgetBody->setObjectName(QStringLiteral("widget_body"));
    m_widgetBody->setStyleSheet(QStringLiteral(
        "#widget_body {"
        "background-color: rgb(231, 237, 235);"
        "border-bottom-left-radius: 12px;"
        "border-bottom-right-radius: 12px;"
        "border: 1px solid #3C968E;"
        "border-top: none;"
        "}"
    ));

    QVBoxLayout* bodyLayout = new QVBoxLayout(m_widgetBody);
    bodyLayout->setSpacing(0);
    bodyLayout->setContentsMargins(5, 5, 5, 10);

    QHBoxLayout* tabsLayout = new QHBoxLayout();
    tabsLayout->setSpacing(0);

    m_labelAboutMe = new ClickableLabel(m_widgetBody);
    m_labelAboutMe->setText(tr("About MxCADViewer"));

    m_labelAbout3rdParty = new ClickableLabel(m_widgetBody);
    m_labelAbout3rdParty->setText(tr("Third-Party Licenses"));

    tabsLayout->addWidget(m_labelAboutMe);
    tabsLayout->addWidget(m_labelAbout3rdParty);

    QWidget* tabSpacer = new QWidget(m_widgetBody);
    tabSpacer->setStyleSheet(QStringLiteral("border-bottom: 1px solid gray;"));
    tabsLayout->addWidget(tabSpacer);
    tabsLayout->setStretch(2, 1);

    bodyLayout->addLayout(tabsLayout);

    m_stackedWidget = new QStackedWidget(m_widgetBody);

    m_stackedWidget->addWidget(setupAboutPage());
    m_stackedWidget->addWidget(setupLicensePage());

    bodyLayout->addWidget(m_stackedWidget);
    mainLayout->addWidget(m_widgetBody);
}

QWidget* MxAboutMeDialog::setupAboutPage()
{
    QWidget* page = new QWidget();
    page->setObjectName(QStringLiteral("page_aboutMe"));
    page->setStyleSheet(QStringLiteral(
        "#page_aboutMe {"
        "background-color: rgb(255, 255, 255);"
        "border-left: 1px solid gray;"
        "border-bottom: 1px solid gray;"
        "border-right: 1px solid gray;"
        "}"
    ));

    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed)); // Top margin

    QHBoxLayout* logoLayout = new QHBoxLayout();
    logoLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    m_labelBigMxlogo = new QLabel(page);
    m_labelBigMxlogo->setFixedSize(80, 80);
    m_labelBigMxlogo->setPixmap(QPixmap(QStringLiteral(":/resources/images/mxlogo.svg")));
    m_labelBigMxlogo->setScaledContents(true);
    logoLayout->addWidget(m_labelBigMxlogo);

    logoLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    layout->addLayout(logoLayout);

    layout->addItem(new QSpacerItem(20, 5, QSizePolicy::Minimum, QSizePolicy::Fixed));

    QLabel* labelName = new QLabel(tr("MxCADViewer"), page);
    labelName->setAlignment(Qt::AlignCenter);
    layout->addWidget(labelName);

    m_labelVersion = new QLabel(QString(tr("Version:%1")).arg(MxUtils::getAppVersion()), page);
    m_labelVersion->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_labelVersion);

    m_labelCopyright = new QLabel(tr("Chengdu Dreamkaide Technology Co., Ltd. All Rights Reserved."), page);
    m_labelCopyright->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_labelCopyright);

    m_labelWebsite = new QLabel(tr("Official Website: <a href='https://www.webcadsdk.com' style='color: #0078D4; text-decoration: none;'>https://www.webcadsdk.com</a>"), page);
    m_labelWebsite->setAlignment(Qt::AlignCenter);
    m_labelWebsite->setOpenExternalLinks(true);
    layout->addWidget(m_labelWebsite);

    layout->addItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Minimum));

    return page;
}

QWidget* MxAboutMeDialog::setupLicensePage()
{
    QWidget* page = new QWidget();
    page->setObjectName(QStringLiteral("page_about3rdParty"));
    page->setStyleSheet(QStringLiteral(
        "#page_about3rdParty {"
        "background-color: rgb(255, 255, 255);"
        "border-left: 1px solid gray;"
        "border-bottom: 1px solid gray;"
        "border-right: 1px solid gray;"
        "}"
    ));

    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->addItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed));

    m_labelLicenseDesc = new QLabel(page);
    m_labelLicenseDesc->setAlignment(Qt::AlignCenter);
    m_labelLicenseDesc->setText(tr("This software is developed using the Qt5.14.2 library under the LGPLv3 license,\nand the OpenCASCADE7.7.0 library under the LGPLv2.1 license."));
    layout->addWidget(m_labelLicenseDesc);

    layout->addItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed));

    m_labelGetSourceCode = new QLabel(tr("<a href='#open_source_address' style='color: #0078D4; text-decoration: none;'>Get Source Code</a>"), page);
    m_labelGetSourceCode->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_labelGetSourceCode);

    m_labelOpenQtLicense = new QLabel(tr("<a href='#open_qt_license' style='color: #0078D4; text-decoration: none;'>Read LGPLv3 License</a>"), page);
    m_labelOpenQtLicense->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_labelOpenQtLicense);

    m_labelOpenOcctLicense = new QLabel(tr("<a href='#open_occt_license' style='color: #0078D4; text-decoration: none;'>Read LGPLv2.1 License</a>"), page);
    m_labelOpenOcctLicense->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_labelOpenOcctLicense);

    layout->addItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed));

    return page;
}


void MxAboutMeDialog::openLicenseFile(const QString& fileName)
{
    QString filePath = QDir(QDir(QApplication::applicationDirPath()).filePath("licenses")).filePath(fileName);
    filePath = QDir::cleanPath(filePath);

    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(filePath))) {
        QMessageBox::warning(this, tr("Error"),
            tr("Could not open the license information file: %1\n"
                "Please ensure you have a default text file viewer installed.").arg(filePath));
    }
}

void MxAboutMeDialog::onOpenQtLicense()
{
    openLicenseFile(QStringLiteral("LICENSE.LGPL3.txt"));
}

void MxAboutMeDialog::onOpenOcctLicense()
{
    openLicenseFile(QStringLiteral("LICENSE_LGPL_21.txt"));
}

void MxAboutMeDialog::onOpenSourceAddressFile()
{
    openLicenseFile(QStringLiteral("thirdPartySourceCodeLink.txt"));
}

void MxAboutMeDialog::on_button_close_clicked()
{
    this->close();
}