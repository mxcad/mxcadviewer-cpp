/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QDialog>
#include <QVariant>
#include <QColor>
#include <QStringList>
#include "DimPropertyData.h"
#include "DimPropertyFlag.h"
#include "MxCADInclude.h"
class QTableWidget;
class QTableWidgetItem;

class Mx2dAnnotationEditor;


class Mx2dDimPropertyDialog : public QDialog
{
    Q_OBJECT

public:
    // Pass the flags to determine which rows are visible
    explicit Mx2dDimPropertyDialog(McDbObjectId id, Mx2dAnnotationEditor* annoEditor, QWidget* parent = nullptr);
    ~Mx2dDimPropertyDialog() override = default;

    // Getters for the modified data
    QColor getSelectedColor() const;
    QString getSelectedCategory() const;
    double getTextHeight() const;
    int getRatioSelectionIndex() const; // 0 for the specific value, 1 for "Current Dimension Ratio"

signals:
    // Emitted when the "Click here to modify" button is clicked
    void textPositionModifyClicked();
    void contentModifyClicked();

private:
    void initUI();
    void buildTable();

    // Helper to add a row to the table
    void addRow(DimPropertyFlag propType, const QString& name, const QVariant& value);

private:
    QTableWidget* m_pTable;
    DimPropertyFlags m_flags;
    DimPropertyData m_initialData;
    McDbObjectId m_id;
    Mx2dAnnotationEditor* m_pAnnoEditor;
public:
    // Custom role to identify what property a row represents
    static const int PropertyTypeRole = Qt::UserRole + 1;
protected:
	// Add this to filter viewport mouse events
	bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void onTableCellClicked(int row, int column);
    void onTableCellChanged(int row, int column);


    void onTextPositionModifyClicked();
    void onContentModifyClicked();
};