/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QWidget>

class QPushButton;

class Mx2dShapeListItemWidget : public QWidget
{
	Q_OBJECT

public:
	explicit Mx2dShapeListItemWidget(const QString& name, int count, QWidget* parent = nullptr);

	QString shapeName() const { return m_sName; }
	int shapeCount() const { return m_nCount; }

signals:
	void viewClicked(const QString& name, int count);
	void deleteClicked();

	void buttonHoverChanged(const QString& hint);

protected:
	bool eventFilter(QObject* watched, QEvent* event) override;

private:
	QString m_sName;
	int m_nCount;

	QPushButton* m_pViewButton;
	QPushButton* m_pDeleteButton;
};