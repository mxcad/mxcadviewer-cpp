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
class QRadioButton;
class QPushButton;
class QCheckBox;
class QTabWidget;
class QTableWidget;
class QTextEdit;
class QTableWidgetItem;

class Mx2dMeasurementStatDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Mx2dMeasurementStatDialog(QWidget* parent = nullptr);
	~Mx2dMeasurementStatDialog();

private slots:
	void onScopeChanged();
	void onFilterByColorToggled(bool checked);
	void onFilterByCategoryToggled(bool checked);

private:
	void setupUi();
	void setupConnections();
	void populateTables();
	void updateTotals();

	QWidget* createColorWidget(const QColor& color);

	QGroupBox* m_pScopeGroupBox;
	QRadioButton* m_pAllAnnotationsRadio;
	QRadioButton* m_pManualSelectRadio;
	QPushButton* m_pSelectButton;
	QPushButton* m_pClearButton;

	QCheckBox* m_pFilterByColorCheck;
	QPushButton* m_pOpenColorFilterButton;
	QCheckBox* m_pFilterByCategoryCheck;
	QPushButton* m_pOpenCategoryFilterButton;

	QTabWidget* m_pTabWidget;
	QTableWidget* m_pLengthTable;
	QTableWidget* m_pAreaTable;

	QTextEdit* m_pTotalsTextEdit;
	QPushButton* m_pCopyTotalsButton;
	QCheckBox* m_pShowGrayCheck;
	QPushButton* m_pExportButton;
};