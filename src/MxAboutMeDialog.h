/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QDialog>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QString>
#include <QPoint>

class ClickableLabel;

class MxAboutMeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MxAboutMeDialog(QWidget* parent = nullptr);
    ~MxAboutMeDialog();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private slots:
    void on_button_close_clicked();
    void onOpenQtLicense();
    void onOpenOcctLicense();
    void onOpenSourceAddressFile();

private:
    void setupUi();
    void connectSignalsSlots();

    void setupHeader(QVBoxLayout* mainLayout);
    void setupBody(QVBoxLayout* mainLayout);
    QWidget* setupAboutPage();
    QWidget* setupLicensePage();

    void openLicenseFile(const QString& fileName);
    void setCurrentTab(int index);

private:
    bool m_isDragging;
    QPoint m_mouseStartPos;
    QPoint m_windowStartPos;

    QWidget* m_widgetHeader;
    QWidget* m_widgetBody;

    QLabel* m_labelMxlogo;
    QLabel* m_labelWindowTitle;
    QPushButton* m_buttonClose;

    ClickableLabel* m_labelAboutMe;
    ClickableLabel* m_labelAbout3rdParty;
    QStackedWidget* m_stackedWidget;

    QLabel* m_labelBigMxlogo;
    QLabel* m_labelVersion;
    QLabel* m_labelCopyright;
    QLabel* m_labelWebsite;

    QLabel* m_labelLicenseDesc;
    QLabel* m_labelGetSourceCode;
    QLabel* m_labelOpenQtLicense;
    QLabel* m_labelOpenOcctLicense;

    const QString m_tabSelectStyle = QStringLiteral(
        "QLabel {"
        "background-color: rgb(255, 255, 255);"
        "border-top: 1px solid gray;"
        "border-left: 1px solid gray;"
        "border-right: 1px solid gray;"
        "border-top-left-radius: 3px;"
        "border-top-right-radius: 3px;"
        "padding: 6px;"
        "}"
    );

    const QString m_tabUnselectStyle = QStringLiteral(
        "QLabel {"
        "background-color: rgb(208, 208, 208);"
        "border-bottom: 1px solid gray;"
        "padding: 6px;"
        "border-top-left-radius: 3px;"
        "border-top-right-radius: 3px;"
        "}"
        "QLabel:hover {"
        "background-color: rgb(240, 240, 240);"
        "}"
    );
};