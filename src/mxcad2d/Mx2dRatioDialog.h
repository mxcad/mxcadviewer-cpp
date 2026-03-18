/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QDialog>

class QLabel;
class QDoubleSpinBox;
class QPushButton;

class Mx2dRatioDialog : public QDialog
{
    Q_OBJECT

public:
    explicit Mx2dRatioDialog(QWidget *parent = nullptr);
    ~Mx2dRatioDialog();

    void setValue(double value);
    double value() const;

    void loadFromFile(const QString& filePath);
    void saveToFile();

    double dimRatio() const;

private:
    void setupUi();
    void connectSignals();

    QLabel* m_pInfoLabel;
    QDoubleSpinBox* m_pSpinBox;
    QPushButton* m_pOkButton;
    QPushButton* m_pCancelButton;

    double m_realValue;
    double m_dimRatio;
    QString m_filePath;
};