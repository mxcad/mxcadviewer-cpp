/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include <QObject>
#include <QSettings>
#include <QString>

class MxLanguageManager : public QObject
{
	Q_OBJECT
public:

	static MxLanguageManager& getInstance();

	MxLanguageManager(const MxLanguageManager&) = delete;
	MxLanguageManager& operator=(const MxLanguageManager&) = delete;

	QString getLanguage();
    void setLanguage(const QString& language);

private:
	MxLanguageManager(QObject* parent = nullptr);
	~MxLanguageManager();


private:
	QSettings m_settings{ "MxDraw", "MxCADViewer" };
};
