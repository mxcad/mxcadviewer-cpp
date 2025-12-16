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
class DrawingPreviewWidgetBatch;
class QListWidget;
class Mx2dFrameRegionWidget;
class QAction;

class Mx2dBatchExportPdfDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Mx2dBatchExportPdfDialog(QWidget* parent = nullptr);
	~Mx2dBatchExportPdfDialog();

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

	QGroupBox* m_pMergeExportGroup;
	QRadioButton* m_pOneFrameOnePDF;
	QRadioButton* m_pAllFrameOnePDF;

	QPushButton* m_pExportButton;


	QListWidget* m_pMidPanel;


	QPushButton* m_pAddFramButton;
	QPushButton* m_pRemoveFrameButton;
	QPushButton* m_pRecognizeFrameButton;
	QPushButton* m_pFullViewButton;
	QCheckBox* m_pShowSelOnlyCheckBox;
	Mx2dFrameRegionWidget* m_pViewWidget;

	QAction* m_pRemoveAllFrameAction;

	DrawingPreviewWidgetBatch* m_pPreviewWidget;


	QSize m_basePageSize;
};

