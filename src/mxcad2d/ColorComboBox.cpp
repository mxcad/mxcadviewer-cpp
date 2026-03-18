/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "ColorComboBox.h"
#include <QStylePainter>
#include "ColorListDelegate.h"
#include "Mx2dIndexColorDialog.h"

ColorComboBox::ColorComboBox(QWidget* parent)
	: QComboBox(parent) 
{
	// Apply custom delegate to the drop-down list view
	setItemDelegate(new ColorListDelegate(this));

	// Add quick colors (No icons, just data)
	addColorItem(Qt::red);
	addColorItem(Qt::green);
	addColorItem(Qt::blue);
	addColorItem(Qt::yellow);
	addColorItem(Qt::cyan);

	// Add the "More Colors..." option at the end
	addItem("", tr("MORE")); // Empty display text, "MORE" as data

	connect(this, QOverload<int>::of(&QComboBox::activated), this, &ColorComboBox::onItemActivated);
}

void ColorComboBox::setCurrentColor(const QColor& color) 
{
	int idx = findData(color, Qt::UserRole);
	if (idx >= 0) {
		setCurrentIndex(idx);
	}
	else {
		insertColorItem(0, color);
		setCurrentIndex(0);
	}
}

QColor ColorComboBox::getCurrentColor() const
{
	return currentData(Qt::UserRole).value<QColor>();
}

void ColorComboBox::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event);
	QStylePainter painter(this);
	painter.setPen(palette().color(QPalette::Text));

	// 1. Draw the basic ComboBox frame and drop-down arrow
	QStyleOptionComboBox opt;
	initStyleOption(&opt);
	opt.currentText = ""; // Prevent default text drawing
	opt.currentIcon = QIcon(); // Prevent default icon drawing
	painter.drawComplexControl(QStyle::CC_ComboBox, opt);

	// 2. Calculate the exact area for text/color (excluding the arrow button)
	QRect editRect = style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxEditField, this);
	editRect.adjust(2, 2, -2, -2); // Add small padding

	// 3. Draw our custom content inside the edit area
	QString strData = currentData().toString();
	if (strData == "MORE") {
		// If "More Colors..." is selected (though usually it bounces back)
		painter.drawText(editRect, Qt::AlignLeft | Qt::AlignVCenter, tr("More Colors..."));
	}
	else {
		// Draw the stretched color block
		QColor color = currentData(Qt::UserRole).value<QColor>();
		if (color.isValid()) {
			painter.save();
			painter.setRenderHint(QPainter::Antialiasing);
			painter.setBrush(color);
			painter.setPen(QPen(Qt::black, 1)); // Black border like the image
			painter.drawRect(editRect);
			painter.restore();
		}
	}
}

void ColorComboBox::addColorItem(const QColor& color)
{
	addItem("", color); // Empty display text, QColor as UserRole data
}

void ColorComboBox::insertColorItem(int index, const QColor& color)
{
	insertItem(index, "", color);
}

void ColorComboBox::onItemActivated(int index)
{
	if (itemData(index).toString() == "MORE") {
		Mx2dIndexColorDialog dlg(this);
		if (dlg.exec() == QDialog::Accepted) {
			QColor newColor = dlg.getSelectedColor();
			if (newColor.isValid()) {
				insertColorItem(0, newColor);
				setCurrentIndex(0);
			}
		}
		else {
			// Revert to index 0 if cancelled
			setCurrentIndex(0);
		}
	}
}
