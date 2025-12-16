/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QDialog>
#include <QString>

class QGroupBox;
class QRadioButton;
class QLineEdit;
class QPushButton;
class QCheckBox;

class Mx2dTableSaveDialog : public QDialog
{
	Q_OBJECT

public:
	enum class SaveMode {
		SaveToNewFile,
		SaveToExistingFile
	};

	explicit Mx2dTableSaveDialog(QWidget* parent = nullptr);
	~Mx2dTableSaveDialog();

	SaveMode getSaveMode() const;
	QString getFilePath() const;
	bool shouldOpenFileAfterSave() const;

private slots:
	void onBrowseButtonClicked();
	void onSaveModeChanged();

private:
	void setupUi();
	void setupConnections();

	QGroupBox* m_pSaveModeGroupBox;
	QRadioButton* m_pNewFileRadio;
	QRadioButton* m_pExistingFileRadio;

	QGroupBox* m_pSavePathGroupBox;
	QLineEdit* m_pPathLineEdit;
	QPushButton* m_pBrowseButton;

	QCheckBox* m_pOpenAfterSaveCheck;

	QPushButton* m_pSaveButton;
	QPushButton* m_pCancelButton;
};