/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QDialog>

class QStackedWidget;
class QListWidget;
class QTableWidget;
class QLabel;
class QPushButton;
class QListWidgetItem;
class Mx2dShapeListItemWidget;

class Mx2dShapeListDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Mx2dShapeListDialog(QWidget* parent = nullptr);
	~Mx2dShapeListDialog();

private slots:
	void switchToDetailView(const QString& shapeName, int count);
	void switchToMainView();

	void updateHint(const QString& hint);

	void onListItemDoubleClicked(QListWidgetItem* pItem);

	void onDeleteItem(QListWidgetItem* pItem);

private:

	void setupUi();

	QWidget* createMainPage();

	QWidget* createDetailPage();

	void addShapeItem(const QString& name, int count);


	QStackedWidget* m_pStackedWidget;
	QLabel* m_pHintLabel;
	QPushButton* m_pExportButton;


	QListWidget* m_pListWidget;


	QLabel* m_pDetailTitleLabel;
	QTableWidget* m_pDetailTable;
};