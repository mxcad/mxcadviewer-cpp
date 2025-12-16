/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QDialog>

class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QCheckBox;
class QWidget;
class ShapePreviewWidget;

class Mx2dShapeRecognitionDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Mx2dShapeRecognitionDialog(QWidget* parent = nullptr);
	~Mx2dShapeRecognitionDialog();

private slots:
	void onScopeChanged();

	void onSelectColor();

private:
	void setupUi();
	void setupConnections();

	QGroupBox* m_pShapeGroup;
	QLabel* m_pNameLabel;
	QLineEdit* m_pNameLineEdit;
	QLabel* m_pNameHintLabel;
	ShapePreviewWidget* m_pShapePreview;
	QPushButton* m_pReselectButton;

	QGroupBox* m_pScopeGroup;
	QRadioButton* m_pEntireDrawingRadio;
	QRadioButton* m_pRectangularSearchRadio;
	QRadioButton* m_pIrregularSearchRadio;
	QPushButton* m_pSelectAreaButton;

	QGroupBox* m_pColorGroup;
	QLabel* m_pCurrentColorLabel;
	QWidget* m_pColorSwatch;
	QPushButton* m_pSelectColorButton;

	QCheckBox* m_pFilterCheckBox;
	QPushButton* m_pHelpButton;

	QPushButton* m_pStartButton;
};

class ShapePreviewWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ShapePreviewWidget(QWidget* parent = nullptr);

protected:
	void paintEvent(QPaintEvent* event) override;
};