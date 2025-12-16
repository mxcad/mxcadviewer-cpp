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
class QComboBox;
class QSpinBox;
class QRadioButton;
class QCheckBox;
class QPushButton;
class DrawingPreviewWidget;

class Mx2dExportPdfDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Mx2dExportPdfDialog(QWidget* parent = nullptr);
	~Mx2dExportPdfDialog();

private slots:

	void onPaperSizeChanged(int index);

	void onScaleChanged(int index);

	void onOrientationChanged();

private:

	void setupUi();

	void setupConnections();

	void updateUiState();

	void updatePreview();

private:

	QGroupBox* m_pPageSizeGroup;
	QComboBox* m_pPageSizeComboBox;
	QComboBox* m_pScaleComboBox;
	QSpinBox* m_pWidthSpinBox;
	QSpinBox* m_pHeightSpinBox;

	QGroupBox* m_pOrientationGroup;
	QRadioButton* m_pLandscapeRadio;
	QRadioButton* m_pPortraitRadio;

	QGroupBox* m_pColorGroup;
	QRadioButton* m_pColorRadio;
	QRadioButton* m_pBlackWhiteRadio;

	QGroupBox* m_pBoldGroup;
	QCheckBox* m_pBoldCheckBox;

	QGroupBox* m_pAnnotationGroup;
	QCheckBox* m_pAnnotationCheckBox;

	QGroupBox* m_pLayerGroup;
	QCheckBox* m_pLayerCheckBox;

	QPushButton* m_pExportButton;


	QPushButton* m_pFullViewButton;
	QPushButton* m_pSelectRangeButton;
	DrawingPreviewWidget* m_pPreviewWidget;


	QSize m_basePageSize;
};


class DrawingPreviewWidget : public QWidget
{
	Q_OBJECT
public:
	explicit DrawingPreviewWidget(QWidget* parent = nullptr);

	void setPageSize(const QSize& size);

protected:
	void paintEvent(QPaintEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;

private:
	QSize m_pageSize;
};