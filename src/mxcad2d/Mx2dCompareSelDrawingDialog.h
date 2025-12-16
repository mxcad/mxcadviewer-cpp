/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once
#include <QDialog>
#include <QColor>

class QLabel;
class QComboBox;
class QPushButton;
class Mx2dDrawingCompareDialog;

class Mx2dCompareSelDrawingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Mx2dCompareSelDrawingDialog(QWidget* parent = nullptr);
	~Mx2dCompareSelDrawingDialog();

	QString getDrawing1Path() const;
	QString getDrawing2Path() const;
	QColor getDrawing1Color() const;
	QColor getDrawing2Color() const;

private slots:
	void onColor1ButtonClicked();
	void onColor2ButtonClicked();
	void onStartCompareClicked();

private:
	void setupUi();
	void setupConnections();
	void updateColorButton(QPushButton* button, const QColor& color);


	QLabel* m_pInstructionLabel;
	QLabel* m_pDrawing1Label;
	QComboBox* m_pDrawing1ComboBox;
	QPushButton* m_pColor1Button;
	QLabel* m_pDrawing2Label;
	QComboBox* m_pDrawing2ComboBox;
	QPushButton* m_pColor2Button;
	QPushButton* m_pStartButton;
	QPushButton* m_pCancelButton;
	Mx2dDrawingCompareDialog* m_pDrawingCompareDialog;

	QColor m_color1;
	QColor m_color2;
};
