/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once
#include <QObject>
#include <Message_ProgressIndicator.hxx>
class Mx3dProgressIndicator : public QObject, public Message_ProgressIndicator
{
    Q_OBJECT
public:
    Mx3dProgressIndicator(QObject *parent = nullptr);
    ~Mx3dProgressIndicator();


	virtual bool UserBreak() override;
	virtual void Reset() override;
	virtual void Show(const Message_ProgressScope& theScope, const bool isForce) override;

public slots:
	void onAborted();

private:
	bool            m_isAborted		= false;
	QString         m_sLastStepName	= "";
	int             m_iLastProgress	= -1;

signals:
	void progressDetailChanged(const QString& detail);
	void progressPercentChanged(int percent);
};

