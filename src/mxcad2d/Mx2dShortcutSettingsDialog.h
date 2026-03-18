/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/
#pragma once

#include <QDialog>
#include <QTableView>
#include <QStandardItemModel>
#include <QMap>

struct ShortcutData {
	QString id;
	QString displayName;
	QString shortcut;
};

class Mx2dShortcutSettingsDialog : public QDialog
{
	Q_OBJECT
public:
	explicit Mx2dShortcutSettingsDialog(const QList<ShortcutData>& shortcutList, QWidget* parent = nullptr);

	QMap<QString, QString> getSavedShortcuts() const;

protected:
	bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
	void onCustomDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);

private:
	void setupUI();
	bool checkConflict(const QString& shortcut, int currentRowToExclude);

	QTableView* m_fixedTable;
	QTableView* m_customTable;
	QStandardItemModel* m_fixedModel;
	QStandardItemModel* m_customModel;

	QString m_oldShortcut;

private:
	QList<ShortcutData> m_shortcutDataList;
};
