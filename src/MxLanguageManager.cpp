/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "MxLanguageManager.h"
#include <QLocale>

MxLanguageManager& MxLanguageManager::getInstance()
{
    static MxLanguageManager instance;
    return instance;
}

QString MxLanguageManager::getLanguage()
{
    return m_settings.value("language", QLocale::system().name()).toString();
}

void MxLanguageManager::setLanguage(const QString& language)
{
    m_settings.setValue("language", language);
}

QString MxLanguageManager::getCurrentLanguage()
{
    return m_currentLanguage;
}

MxLanguageManager::MxLanguageManager(QObject* parent)
	: QObject(parent)
{
    m_currentLanguage = getLanguage();
}

MxLanguageManager::~MxLanguageManager()
{
}
