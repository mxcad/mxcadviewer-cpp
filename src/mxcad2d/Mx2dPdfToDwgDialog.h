/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QDialog>

// 前向声明
class QListWidget;
class QPushButton;
class QRadioButton;
class QLineEdit;
class QSpinBox;

class Mx2dPdfToDwgDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Mx2dPdfToDwgDialog(QWidget* parent = nullptr);
	~Mx2dPdfToDwgDialog();

private slots:

	void onAddClicked();
	void onDeleteClicked();
	void onClearClicked();
	void onConvertClicked();

	void onBrowseClicked();
	void onConversionOptionChanged();

	void onSelectionChanged();

private:
	void setupUi();
	void setupConnections();
	void updateButtonStates();

	QListWidget* m_pFileListWidget;

	QPushButton* m_pAddButton;
	QPushButton* m_pDeleteButton;
	QPushButton* m_pClearButton;
	QPushButton* m_pConvertButton;

	QRadioButton* m_pPageByPageRadio;
	QRadioButton* m_pMergeRadio;
	QSpinBox* m_pMergeCountSpinBox;

	QLineEdit* m_pSaveLocationEdit;
	QPushButton* m_pBrowseButton;
};