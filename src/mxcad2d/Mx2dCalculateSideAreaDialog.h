/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QDialog>
#include "Mx2dUtils.h"

class AreaShapePreviewWidget;
class QPushButton;
class QLabel;
class QCheckBox;
class QTableWidget;
class QDoubleSpinBox;
class QLineEdit;
class QTextEdit;

class Mx2dCalculateSideAreaDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Mx2dCalculateSideAreaDialog(QWidget* parent = nullptr);
	~Mx2dCalculateSideAreaDialog();

public:
	void setPolylineData(const Mx2d::PLVertexList& vertices);

private slots:
	void onShapeSelectionChanged();
	void onSeparateHeightsToggled(bool checked);
	void onCalculate();
	void onCopyResult();
	void onCopyProcess();
	void onFullViewClicked();
	void onHeightEditTextChanged(const QString& text);

private:
	void setupUi();
	void setupConnections();
	void updateLengthLabel();
	void populateTable();

	// Left side
	AreaShapePreviewWidget* m_pShapePreview;
	QPushButton* m_pFullViewButton;
	QLabel* m_pSelectedLengthLabel;
	QLabel* m_pSelectedLengthValueLabel;
	QLabel* m_pHintLabel;

	// Right side
	QLabel* m_pHeightLabel;
	QLineEdit* m_pHeightEdit;
	QCheckBox* m_pSeparateHeightsCheckBox;
	QTableWidget* m_pHeightTable;
	QPushButton* m_pCalculateButton;
	QLabel* m_pAreaLabel;
	QLineEdit* m_pAreaResultEdit;
	QPushButton* m_pCopyResultButton;
	QLabel* m_pProcessLabel;
	QTextEdit* m_pProcessTextEdit;
	QPushButton* m_pCopyProcessButton;
};