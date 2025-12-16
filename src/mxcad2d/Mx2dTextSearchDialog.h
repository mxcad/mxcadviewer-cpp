/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QDialog>
#include <QTableWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QButtonGroup>
#include "MxCADInclude.h"

#include "Mx2dUtils.h"

class Mx2dTextSearchDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Mx2dTextSearchDialog(QWidget* parent = nullptr);
	~Mx2dTextSearchDialog();

private slots:
	void onSearchClicked();
	void onSearchResults(const QList<QPair<QString, Mx2d::Mx2dExtents>>& result);
	void onSearchAreaChanged();
	void onSelectAreaClicked();
	void onToggleShowResult(bool checked);
	void onNextResultClicked();
	void onCurrentItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous);
	void onExportToExcelClicked();
private:
	QComboBox* m_pSearchContentCombo;
	QPushButton* m_pSearchBtn;
	QCheckBox* m_pFullMatchCheck;

	QRadioButton* m_pWholePaperRadio;
	QRadioButton* m_pRectAreaRadio;
	QRadioButton* m_pIrregularAreaRadio;
	QPushButton* m_pSelectAreaBtn;

	QRadioButton* m_pPaperTextRadio;
	QRadioButton* m_pAppTextRadio;

	QCheckBox* m_pShowResultCheck;
	QLabel* m_pResultCountLabel;
	QCheckBox* m_pMarkAllCheck;

	QList<QPair<QString, Mx2d::Mx2dExtents>> m_results;
	QTableWidget* m_pResultTable;

	QPushButton* m_pExportExcelBtn;
	QPushButton* m_pNextResultBtn;
	QPushButton* m_pCompleteBtn;

	QWidget* m_pResultPanel;

	QButtonGroup* m_pAreaButtonGroup;
	QButtonGroup* m_pScopeButtonGroup;

	void initControls();
	void initLayout();
	void addToTable(int count, const QString& text, Mx2d::Mx2dExtents ext2d);
	void updateSearchResult();
};