/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QDialog>
#include <QList>
#include "DimCategoryData.h"
class QPushButton;
class QLabel;
class QLineEdit;
class QTableWidget;
class QTableWidgetItem;
class Mx2dAnnotationEditor;


class Mx2dDimCategoryManagerDialog : public QDialog
{
	Q_OBJECT
public:
	enum class TableColumn
	{
        Name = 0,
        Color,
        Show,
        Current,
        Delete
	};
public:
	explicit Mx2dDimCategoryManagerDialog(Mx2dAnnotationEditor* annoEditor, QWidget* parent = nullptr);

	void setFileName(const QString& fileName);
	void loadConfig(const QString& fileName); // Function to mock loading external configuration

	void saveConfig(const QString& fileName);
	QList<DimCategoryData> getCategoryDataFromTable();
	DimCategoryData getCurrentCategoryData();
private slots:
	void onNewCategoryClicked();
	void onShowAllClicked();
	void onHideAllClicked();
	void onSearchTextChanged(const QString& text);
	void onMoveUpClicked();
	void onMoveDownClicked();

	void onTableCellClicked(int row, int column);
	void onTableCellDoubleClicked(int row, int column);
	void onTableItemChanged(QTableWidgetItem* item);
public slots:
	void setCurrentCategory(int row);
private:
	void initUI();
	void initLayout();


	void addCategoryToTable(const DimCategoryData& data);
	void updateCurrentCategoryLabel();
	QString generateUniqueCategoryName();
	bool isCategoryNameExists(const QString& name, int excludeRow = -1);

	void swapRows(int row1, int row2);
	QString getCategoryNameFromRow(int row);
	
	bool hasCategory(const QString& name);
	void loadMoreFromAnnotations();
private:
	Mx2dAnnotationEditor* m_pAnnoEditor = nullptr;

	QPushButton* m_pBtnNewCategory;
	QLabel* m_pLblCurrentCategory;
	QPushButton* m_pBtnShowAll;
	QPushButton* m_pBtnHideAll;

	QLineEdit* m_pEditSearch;
	QPushButton* m_pBtnSearch;
	QPushButton* m_pBtnMoveUp;
	QPushButton* m_pBtnMoveDown;

	QTableWidget* m_pTable;
	QLabel* m_pLblTip;

	QString m_currentCategoryName;
	bool m_bIsUpdatingTable; // Flag to prevent recursive itemChanged calls

	QString m_fileName;
    QList<DimCategoryData> m_categoryDatas;

signals:
    void categoryDataChanged();
};