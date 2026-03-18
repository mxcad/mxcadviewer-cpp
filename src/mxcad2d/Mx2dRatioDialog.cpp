/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dRatioDialog.h"
#include <QtWidgets>

Mx2dRatioDialog::Mx2dRatioDialog(QWidget *parent)
    : QDialog(parent)
{
	Qt::WindowFlags flags = Qt::Dialog;
	flags |= Qt::CustomizeWindowHint;
	flags |= Qt::WindowTitleHint;
	flags |= Qt::WindowCloseButtonHint;
	setWindowFlags(flags);
    setupUi();
    connectSignals();

    if (m_pSpinBox) {
        m_pSpinBox->selectAll();
    }
}

Mx2dRatioDialog::~Mx2dRatioDialog()
{
}

void Mx2dRatioDialog::setupUi()
{
    setWindowTitle(tr("Enter Actual Length"));
    QVBoxLayout *pMainLayout = new QVBoxLayout(this);
    pMainLayout->setSpacing(10);
    pMainLayout->setContentsMargins(15, 15, 15, 15);

    m_pInfoLabel = new QLabel(tr("Enter the actual size between two points to set the annotation ratio:"));

    m_pSpinBox = new QDoubleSpinBox();
    m_pSpinBox->setDecimals(3);
    m_pSpinBox->setRange(0.001, 999999.999);
    m_pSpinBox->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
    m_pSpinBox->setMinimumHeight(30);

    QHBoxLayout *pButtonLayout = new QHBoxLayout();
    m_pOkButton = new QPushButton(tr("OK"));
    m_pCancelButton = new QPushButton(tr("Cancel"));

    m_pOkButton->setDefault(true);
    m_pOkButton->setMinimumWidth(80);
    m_pCancelButton->setMinimumWidth(80);

    pButtonLayout->addStretch();
    pButtonLayout->addWidget(m_pOkButton);
    pButtonLayout->addWidget(m_pCancelButton);
    //pButtonLayout->addStretch();

    pMainLayout->addWidget(m_pInfoLabel);
    pMainLayout->addWidget(m_pSpinBox);
    pMainLayout->addSpacing(10);
    pMainLayout->addLayout(pButtonLayout);

    adjustSize();
    setFixedSize(size());
}

void Mx2dRatioDialog::connectSignals()
{
    connect(m_pOkButton, &QPushButton::clicked, this, [this]() {
        m_dimRatio = value() / m_realValue;
        m_dimRatio = qRound(m_dimRatio * 100) / 100.0;
        saveToFile();
        QDialog::accept();
        });
    connect(m_pCancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void Mx2dRatioDialog::setValue(double value)
{
    m_pSpinBox->setValue(value);
    m_pSpinBox->selectAll();
    m_realValue = value;
}

double Mx2dRatioDialog::value() const
{
    return m_pSpinBox->value();
}

void Mx2dRatioDialog::loadFromFile(const QString& filePath)
{
    if (QFile::exists(filePath))
    {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
            if (jsonDoc.isObject())
            {
                QJsonObject jsonObj = jsonDoc.object();
                if (jsonObj.contains("ratio"))
                {
                    QJsonValue ratioValue = jsonObj.value("ratio");
                    if (ratioValue.isDouble())
                    {
                        m_dimRatio = ratioValue.toDouble();
                    }
                }
            }
            file.close();
        }
    }
    else
    {
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            m_dimRatio = 1.0;
            QJsonObject jsonObj;
            jsonObj.insert("ratio", m_dimRatio);
            QJsonDocument jsonDoc(jsonObj);
            file.write(jsonDoc.toJson());
            file.close();
        }
    }
    m_filePath = filePath;
}

void Mx2dRatioDialog::saveToFile()
{
    if (!m_filePath.isEmpty())
    {
        QFile file(m_filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QJsonObject jsonObj;
            jsonObj.insert("ratio", m_dimRatio);
            QJsonDocument jsonDoc(jsonObj);
            file.write(jsonDoc.toJson());
            file.close();
        }
    }
}

double Mx2dRatioDialog::dimRatio() const
{
    return m_dimRatio;
}
