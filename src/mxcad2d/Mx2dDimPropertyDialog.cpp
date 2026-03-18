/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dDimPropertyDialog.h"
#include "Mx2dIndexColorDialog.h"

#include <QVBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>

#include <QComboBox>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>

#include <QStylePainter>
#include <QListView>
#include <QColorDialog>
#include "DimPropertyDelegate.h"
#include "Mx2dCustomAnnotation.h"
#include "Mx2dCustomText.h"
#include "Mx2dCustomMText.h"
#include "Mx2dCustomLeader.h"
#include "Mx2dGuiDocument.h"
#include "MxUtils.h"
#include "Mx2dAnnotationEditor.h"

#include "Mx2dModifySingleLineTextDialog.h"
#include "Mx2dModifyMultiLineTextDialog.h"

// -------------------------------------------------------------------------
// Main Dialog Implementation
// -------------------------------------------------------------------------

const int Mx2dDimPropertyDialog::PropertyTypeRole;

Mx2dDimPropertyDialog::Mx2dDimPropertyDialog(McDbObjectId id, Mx2dAnnotationEditor* annoEditor, QWidget* parent)
    : QDialog(parent), m_id(id), m_pAnnoEditor(annoEditor)
{
    Qt::WindowFlags winFlags = Qt::Dialog;
    winFlags |= Qt::CustomizeWindowHint;
    winFlags |= Qt::WindowTitleHint;
    winFlags |= Qt::WindowCloseButtonHint;
    setWindowFlags(winFlags);
    McDbObjectPointer<Mx2dCustomAnnotation> spAnno(m_id, McDb::kForRead);
    assert(spAnno.openStatus() == Mcad::eOk);
    m_flags = spAnno->dimPropertyFlags();
    m_initialData.color = Mx2d::cadColorToQColor(spAnno->color());
    m_initialData.currentCategory = spAnno->category();
    m_initialData.textHeight = spAnno->textHeight();
    m_initialData.currentRatio = spAnno->dimRatio();
    Mx2dGuiDocument* pCadGuiDoc = qobject_cast<Mx2dGuiDocument*>(MxUtils::gCurrentTab);
    if (pCadGuiDoc)
    {
        QList<DimCategoryData> data = pCadGuiDoc->getCategoryDataFromTable();
        QStringList categories;
        for (auto& cat : data)
        {
            categories.append(cat.name);
        }
        m_initialData.availableCategories = categories;
    }
    setWindowTitle(tr("Annotation Properties"));
    resize(350, 400);

    initUI();
    buildTable();
}

void Mx2dDimPropertyDialog::initUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    m_pTable = new QTableWidget(0, 2, this);
    m_pTable->setHorizontalHeaderLabels({ tr("Property Name"), tr("Property Value") });
    
    // Header styling
    m_pTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_pTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_pTable->verticalHeader()->setVisible(false);
    m_pTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_pTable->setSelectionBehavior(QAbstractItemView::SelectItems);
    
    // Trigger editing on single click for combo boxes to feel responsive
    m_pTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    // Row height
    m_pTable->verticalHeader()->setDefaultSectionSize(40);

    // Apply custom delegate
    m_pTable->setItemDelegate(new DimPropertyDelegate(m_initialData, m_pTable));
    m_pTable->viewport()->installEventFilter(this);
    connect(m_pTable, &QTableWidget::cellClicked, this, &Mx2dDimPropertyDialog::onTableCellClicked);
    connect(m_pTable, &QTableWidget::cellChanged, this, &Mx2dDimPropertyDialog::onTableCellChanged);
    mainLayout->addWidget(m_pTable);
}

void Mx2dDimPropertyDialog::buildTable()
{
    m_pTable->setRowCount(0);

    if (m_flags.testFlag(Prop_Color)) {
        addRow(Prop_Color, tr("Color"), m_initialData.color);
    }
    if (m_flags.testFlag(Prop_Category)) {
        addRow(Prop_Category, tr("Category"), m_initialData.currentCategory);
    }
    if (m_flags.testFlag(Prop_TextHeight)) {
        addRow(Prop_TextHeight, tr("Text Height"), QString::number(m_initialData.textHeight, 'f', 2));
    }
    if (m_flags.testFlag(Prop_Ratio)) {
        addRow(Prop_Ratio, tr("Ratio"), QString::number(m_initialData.currentRatio, 'f', 3));
    }
    if (m_flags.testFlag(Prop_TextPosition)) {
        addRow(Prop_TextPosition, tr("Text Position"), QVariant()); // Value handled by button
    }
    if (m_flags.testFlag(Prop_Content)){
        addRow(Prop_Content, tr("Content"), QVariant());
    }
}

void Mx2dDimPropertyDialog::addRow(DimPropertyFlag propType, const QString& name, const QVariant& value)
{
    int row = m_pTable->rowCount();
    m_pTable->insertRow(row);

    // Column 0: Property Name (Not Editable)
    QTableWidgetItem* nameItem = new QTableWidgetItem(name);
    nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
    nameItem->setTextAlignment(Qt::AlignCenter);
    nameItem->setData(PropertyTypeRole, static_cast<int>(propType)); // Store property type identifier
    m_pTable->setItem(row, 0, nameItem);

    // Column 1: Property Value
    if (propType == Prop_TextPosition) {
        // Special Case: Text Position is always a button
        QTableWidgetItem* emptyItem = new QTableWidgetItem();
        emptyItem->setFlags(emptyItem->flags() & ~Qt::ItemIsEditable); // Prevent default editor
        m_pTable->setItem(row, 1, emptyItem);

        QPushButton* btn = new QPushButton(tr("Modify"));
        btn->setStyleSheet("margin: 2px 10px;"); // Add some padding inside the cell
        connect(btn, &QPushButton::clicked, this, &Mx2dDimPropertyDialog::onTextPositionModifyClicked);
        m_pTable->setCellWidget(row, 1, btn);
    } 
    else if (propType == Prop_Content) {
        // Special Case: Content is always a button
        QTableWidgetItem* emptyItem = new QTableWidgetItem();
        emptyItem->setFlags(emptyItem->flags() & ~Qt::ItemIsEditable); // Prevent default editor
        m_pTable->setItem(row, 1, emptyItem);

        QPushButton* btn = new QPushButton(tr("Modify"));
        btn->setStyleSheet("margin: 2px 10px;"); // Add some padding inside the cell
        connect(btn, &QPushButton::clicked, this, &Mx2dDimPropertyDialog::onContentModifyClicked);
        m_pTable->setCellWidget(row, 1, btn);
    }
    else {
        // Standard Case: Data handled by Delegate
        QTableWidgetItem* valItem = new QTableWidgetItem();
        valItem->setTextAlignment(Qt::AlignCenter);
        
        if (propType == Prop_Color) {
            valItem->setData(Qt::UserRole, value.value<QColor>());
        } else {
            valItem->setText(value.toString());
        }
        m_pTable->setItem(row, 1, valItem);
    }
}

// -------------------------------------------------------------------------
// Data Retrieval Getters
// -------------------------------------------------------------------------

QColor Mx2dDimPropertyDialog::getSelectedColor() const
{
    for (int i = 0; i < m_pTable->rowCount(); ++i) {
        if (m_pTable->item(i, 0)->data(PropertyTypeRole).toInt() == Prop_Color) {
            return m_pTable->item(i, 1)->data(Qt::UserRole).value<QColor>();
        }
    }
    return Qt::white;
}

QString Mx2dDimPropertyDialog::getSelectedCategory() const
{
    for (int i = 0; i < m_pTable->rowCount(); ++i) {
        if (m_pTable->item(i, 0)->data(PropertyTypeRole).toInt() == Prop_Category) {
            return m_pTable->item(i, 1)->text();
        }
    }
    return QString();
}

double Mx2dDimPropertyDialog::getTextHeight() const
{
    for (int i = 0; i < m_pTable->rowCount(); ++i) {
        if (m_pTable->item(i, 0)->data(PropertyTypeRole).toInt() == Prop_TextHeight) {
            return m_pTable->item(i, 1)->text().toDouble();
        }
    }
    return 0.0;
}

int Mx2dDimPropertyDialog::getRatioSelectionIndex() const
{
    for (int i = 0; i < m_pTable->rowCount(); ++i) {
        if (m_pTable->item(i, 0)->data(PropertyTypeRole).toInt() == Prop_Ratio) {
            // We stored the combo box index in UserRole during setModelData
            return m_pTable->item(i, 1)->data(Qt::UserRole).toInt(); 
        }
    }
    return 0;
}

// -------------------------------------------------------------------------
// Event Filter
// -------------------------------------------------------------------------
bool Mx2dDimPropertyDialog::eventFilter(QObject* watched, QEvent* event)
{
	// Check if the event is a mouse press on the table's viewport
	if (watched == m_pTable->viewport() && event->type() == QEvent::MouseButtonPress) {
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

		// If clicking left button and there is no item at the mouse position
		if (mouseEvent->button() == Qt::LeftButton && !m_pTable->itemAt(mouseEvent->pos())) {
			m_pTable->clearSelection(); // Clear all selections
		}
	}

	// Always call the base class implementation
	return QDialog::eventFilter(watched, event);
}

void Mx2dDimPropertyDialog::onTableCellClicked(int row, int column)
{
	if (column == 1) {
		QTableWidgetItem* nameItem = m_pTable->item(row, 0);
        QTableWidgetItem* valItem = m_pTable->item(row, 1);
        int propType = nameItem->data(PropertyTypeRole).toInt();
        if (propType == Prop_Color)
        {
            
            QColor currentColor = valItem->data(Qt::UserRole).value<QColor>();
			QColor newColor = QColorDialog::getColor(currentColor, this, tr("Select Color"));
			if (newColor.isValid()) {
                
                valItem->setData(Qt::UserRole, newColor);
                m_pAnnoEditor->modifyAnnotationColor(m_id, currentColor, newColor);
                MxDrawApp::UpdateDisplay();
			}
        }else if (propType == Prop_Category || propType == Prop_TextHeight || propType == Prop_Ratio)
        {
            m_pTable->editItem(valItem);
        }

		
	}
}

void Mx2dDimPropertyDialog::onTableCellChanged(int row, int column)
{
    if (column == 1) { 
        QTableWidgetItem* nameItem = m_pTable->item(row, 0);
        int propType = nameItem->data(PropertyTypeRole).toInt();
        if (propType != Prop_Category && propType != Prop_TextHeight && propType != Prop_Ratio)
            return;

        McDbObjectPointer<Mx2dCustomAnnotation> spAnno(m_id, McDb::kForRead);
        QString cat = spAnno->category();
        double textHeight = spAnno->textHeight();
        double ratio = spAnno->dimRatio();

        if (propType == Prop_Category)
        {
            QString currentCat = m_pTable->item(row, 1)->text();
            if (currentCat == cat)
                return;
            m_pAnnoEditor->modifyAnnotationCategory(m_id, cat, currentCat);
        }
        else if (propType == Prop_TextHeight)
        {
            double currentTextHeight = m_pTable->item(row, 1)->text().toDouble();
            if (currentTextHeight == textHeight)
                return;
            m_pAnnoEditor->modifyAnnotationTextHeight(m_id, textHeight, currentTextHeight);
        }
        else if (propType == Prop_Ratio)
        {
            bool isOk = false;
            double currentRatio = m_pTable->item(row, 1)->text().toDouble(&isOk);
            if (currentRatio == ratio)
                return;
            m_pAnnoEditor->modifyAnnotationRatio(m_id, ratio, currentRatio);
        }
        MxDrawApp::UpdateDisplay();
    }
}

void Mx2dDimPropertyDialog::onTextPositionModifyClicked()
{
    close();
	MrxDbgRbList spParam = Mx::mcutBuildList(RTId, m_id.asOldId(), 0);
    Mx2d::execCmd2d(MxUtils::gCurrentTab, "Mx_MoveDimPt", spParam.orphanData());
}

void Mx2dDimPropertyDialog::onContentModifyClicked()
{
    McDbObjectPointer<Mx2dCustomAnnotation> spAnno(m_id, McDb::kForRead);
    if (spAnno.openStatus() != Mcad::eOk)
        return;
    if (spAnno->isA() == Mx2dCustomText::desc())
    {
        Mx2dCustomText* pText = Mx2dCustomText::cast(spAnno.object());
        Mx2dModifySingleLineTextDialog dlg(this);
        dlg.setTextId(m_id);
        dlg.setTextContent(pText->textString());
        dlg.exec();
    }
    else if (spAnno->isA() == Mx2dCustomMText::desc())
    {
        Mx2dCustomMText* pMText = Mx2dCustomMText::cast(spAnno.object());
        Mx2dModifyMultiLineTextDialog dlg(this);
        dlg.setTextId(m_id);
        dlg.setTextContent(pMText->contents());
        dlg.exec();
    }
    else if (spAnno->isA() == Mx2dCustomLeader::desc())
    {
        Mx2dCustomLeader* pLeader = Mx2dCustomLeader::cast(spAnno.object());
        Mx2dModifyMultiLineTextDialog dlg(this);
        dlg.setTextId(m_id);
        dlg.setTextContent(pLeader->text());
        dlg.exec();
    }
    else
    {
        return;
    }
    MxDrawApp::UpdateDisplay();
}
