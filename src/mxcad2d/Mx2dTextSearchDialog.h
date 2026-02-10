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
class Mx2dGuiDocument;
class Mx2dTextSearchDialog : public QDialog
{
	Q_OBJECT

public:
	Mx2dTextSearchDialog(Mx2dGuiDocument* guiDoc);
	~Mx2dTextSearchDialog();

private slots:
	void onSearchClicked();
	void onSearchResults(const Mx2d::TextInfoList& result);
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

	Mx2d::TextInfoList m_results;
	QTableWidget* m_pResultTable;

	QPushButton* m_pExportExcelBtn;
	QPushButton* m_pNextResultBtn;
	QPushButton* m_pCompleteBtn;

	QWidget* m_pResultPanel;

	QButtonGroup* m_pAreaButtonGroup;
	QButtonGroup* m_pScopeButtonGroup;
	Mx2dGuiDocument* m_guiDoc;
	void initControls();
	void initLayout();
	void addToTable(int count, const QString& text, Mx2d::Extents ext2d);
	void updateSearchResult();

protected:
	void closeEvent(QCloseEvent* ) override;
private:
	Mx2d::Rect2D m_searchAreaRect;
	Mx2d::Point2DList m_searchAreaIrregular;

private:
	int getCheckedButtonId(QButtonGroup* group);
	Mx2d::TextInfoList searchText(const QString& text, bool isExactMatch, int searchArea, int searchScope, bool markAllResults);
	Mx2d::TextInfoList traverseTextInBlockTableRecord(McDbObjectId btrId, const McGeMatrix3d& trsfMat);
	void drawRectByCorner(const McGePoint3d& corner1, const McGePoint3d& corner2, MxColor color, bool isReserve = false);
	void moveViewCenterTo(Mx2d::Extents ext2d);
};