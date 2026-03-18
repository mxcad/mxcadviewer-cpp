/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/
#pragma once
#include <QComboBox>

// -------------------------------------------------------------------------
// Custom ComboBox for the Color Property
// -------------------------------------------------------------------------
class ColorComboBox : public QComboBox {
	Q_OBJECT
public:
	explicit ColorComboBox(QWidget* parent = nullptr);

	void setCurrentColor(const QColor& color);

	QColor getCurrentColor() const;

protected:
	// Override paintEvent to draw the stretched color block on the ComboBox button
	void paintEvent(QPaintEvent* event) override;

private:
	void addColorItem(const QColor& color);

	void insertColorItem(int index, const QColor& color);

private slots:
	void onItemActivated(int index);
};