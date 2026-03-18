/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dDimCategoryManagerDialog.h"

#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QColorDialog>
#include <QPainter>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QDebug>
#include "DimCategoryDelegate.h"
#include "Mx2dAnnotationEditor.h"
#include "Mx2dCustomAnnotation.h"

// -------------------------------------------------------------------------
// Dialog Implementation
// -------------------------------------------------------------------------

Mx2dDimCategoryManagerDialog::Mx2dDimCategoryManagerDialog(Mx2dAnnotationEditor* annoEditor, QWidget* parent)
	: QDialog(parent), m_bIsUpdatingTable(false), m_pAnnoEditor(annoEditor)
{
	Qt::WindowFlags flags = Qt::Dialog;
	flags |= Qt::CustomizeWindowHint;
	flags |= Qt::WindowTitleHint;
	flags |= Qt::WindowCloseButtonHint;
	setWindowFlags(flags);

	setWindowTitle(tr("Annotation Category Management"));
	setMinimumSize(800, 600);

	initUI();
	initLayout();
	//loadConfig();
}

void Mx2dDimCategoryManagerDialog::setFileName(const QString& fileName)
{
    m_fileName = fileName;
}

void Mx2dDimCategoryManagerDialog::initUI()
{
	m_pBtnNewCategory = new QPushButton(QIcon(":/resources/images2d/2d_newDimCategory.svg"), tr("New Category"));
	m_pLblCurrentCategory = new QLabel(tr("Current Category: None"));
	m_pBtnShowAll = new QPushButton(tr("Show All"));
	m_pBtnHideAll = new QPushButton(tr("Hide All"));

	m_pEditSearch = new QLineEdit();
	m_pEditSearch->setPlaceholderText(tr("Please enter the category name to search"));
	m_pBtnSearch = new QPushButton(tr("Search"));
	m_pBtnMoveUp = new QPushButton(QIcon(":/resources/images2d/2d_up.svg"), tr("Move Up"));
	m_pBtnMoveDown = new QPushButton(QIcon(":/resources/images2d/2d_down.svg"), tr("Move Down"));

	m_pTable = new QTableWidget(0, 5);
	m_pTable->setHorizontalHeaderLabels({ tr("Name"), tr("Color"), tr("Show"), tr("Current"), tr("") });
	m_pTable->horizontalHeader()->setSectionResizeMode(static_cast<int>(TableColumn::Name), QHeaderView::Stretch);
	m_pTable->horizontalHeader()->setSectionResizeMode(static_cast<int>(TableColumn::Color), QHeaderView::Fixed);
	m_pTable->horizontalHeader()->setSectionResizeMode(static_cast<int>(TableColumn::Show), QHeaderView::Fixed);
	m_pTable->horizontalHeader()->setSectionResizeMode(static_cast<int>(TableColumn::Current), QHeaderView::Fixed);
	m_pTable->horizontalHeader()->setSectionResizeMode(static_cast<int>(TableColumn::Delete), QHeaderView::Fixed);

	m_pTable->setColumnWidth(static_cast<int>(TableColumn::Color), 120);
	m_pTable->setColumnWidth(static_cast<int>(TableColumn::Show), 90);
	m_pTable->setColumnWidth(static_cast<int>(TableColumn::Current), 90);
	m_pTable->setColumnWidth(static_cast<int>(TableColumn::Delete), 60);

	m_pTable->verticalHeader()->setVisible(false);
	m_pTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pTable->setEditTriggers(QAbstractItemView::DoubleClicked);

	// Apply the custom delegate here
	m_pTable->setItemDelegate(new DimCategoryDelegate(m_pTable));

	m_pLblTip = new QLabel(tr("Double-click table items to modify category name and color, or set current category"));

	// Connect signals
	connect(m_pBtnNewCategory, &QPushButton::clicked, this, &Mx2dDimCategoryManagerDialog::onNewCategoryClicked);
	connect(m_pBtnShowAll, &QPushButton::clicked, this, &Mx2dDimCategoryManagerDialog::onShowAllClicked);
	connect(m_pBtnHideAll, &QPushButton::clicked, this, &Mx2dDimCategoryManagerDialog::onHideAllClicked);
	connect(m_pEditSearch, &QLineEdit::textChanged, this, &Mx2dDimCategoryManagerDialog::onSearchTextChanged);
	connect(m_pBtnMoveUp, &QPushButton::clicked, this, &Mx2dDimCategoryManagerDialog::onMoveUpClicked);
	connect(m_pBtnMoveDown, &QPushButton::clicked, this, &Mx2dDimCategoryManagerDialog::onMoveDownClicked);

	connect(m_pTable, &QTableWidget::cellClicked, this, &Mx2dDimCategoryManagerDialog::onTableCellClicked);
	connect(m_pTable, &QTableWidget::cellDoubleClicked, this, &Mx2dDimCategoryManagerDialog::onTableCellDoubleClicked);
	connect(m_pTable, &QTableWidget::itemChanged, this, &Mx2dDimCategoryManagerDialog::onTableItemChanged);
}

void Mx2dDimCategoryManagerDialog::initLayout()
{
	QHBoxLayout* pTopLayout = new QHBoxLayout();
	pTopLayout->addWidget(m_pBtnNewCategory);
	pTopLayout->addWidget(m_pLblCurrentCategory);
	pTopLayout->addStretch();
	pTopLayout->addWidget(m_pBtnShowAll);
	pTopLayout->addWidget(m_pBtnHideAll);

	QHBoxLayout* pSearchLayout = new QHBoxLayout();
	pSearchLayout->addWidget(m_pEditSearch);
	pSearchLayout->addWidget(m_pBtnSearch);
	pSearchLayout->addWidget(m_pBtnMoveUp);
	pSearchLayout->addWidget(m_pBtnMoveDown);

	QVBoxLayout* pMainLayout = new QVBoxLayout(this);
	pMainLayout->addLayout(pTopLayout);
	pMainLayout->addLayout(pSearchLayout);
	pMainLayout->addWidget(m_pTable);
	pMainLayout->addWidget(m_pLblTip);
}

void Mx2dDimCategoryManagerDialog::loadConfig(const QString& fileName)
{
	m_categoryDatas.clear();
    if (QFile::exists(fileName)) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray jsonData = file.readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
            QJsonArray jsonArray = jsonDoc.array();
            for (const auto& jsonValue : jsonArray) {
                QJsonObject jsonObject = jsonValue.toObject();
                DimCategoryData data;
                data.name = jsonObject["name"].toString();
				QJsonArray colorArray = jsonObject["color"].toArray();
                data.color = QColor(colorArray[0].toInt(), colorArray[1].toInt(), colorArray[2].toInt());
                data.isVisible = jsonObject["visible"].toBool();
                data.isCurrent = jsonObject["current"].toBool();
                //addCategoryToTable(data);
				m_categoryDatas.append(data);
				if (!data.isVisible)
					m_pAnnoEditor->setAnnotationsVisable(data.name, false);
            }
            file.close();
        }
    }
    else {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)) {
			DimCategoryData data;
            data.name = "default";
            data.color = QColor(230, 81, 0);
            data.isVisible = true;
			data.isCurrent = true;


            QJsonArray jsonArray;
			QJsonObject jsonObject;

			jsonObject["name"] = data.name;
			QJsonArray colorArray;
            colorArray.append(data.color.red());
            colorArray.append(data.color.green());
            colorArray.append(data.color.blue());
			jsonObject["color"] = colorArray;
			jsonObject["visible"] = data.isVisible;
			jsonObject["current"] = data.isCurrent;
			jsonArray.append(jsonObject);
            QJsonDocument jsonDoc(jsonArray);
            file.write(jsonDoc.toJson());
            //addCategoryToTable(data);
            m_categoryDatas.append(data);
            file.close();
        }
    }
    for (const auto & categoryData : m_categoryDatas) {
        addCategoryToTable(categoryData);
    }
	loadMoreFromAnnotations();
}

void Mx2dDimCategoryManagerDialog::saveConfig(const QString& fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) { 
        QJsonArray jsonArray;
		for (auto & categoryData : m_categoryDatas) {
			jsonArray.append(QJsonObject{
				{"name", categoryData.name},
				{"color", QJsonArray{categoryData.color.red(), categoryData.color.green(), categoryData.color.blue()}},
				{"visible", categoryData.isVisible},
				{"current", categoryData.isCurrent}
				});
        }
        QJsonDocument jsonDoc(jsonArray);
        file.write(jsonDoc.toJson());
        file.close();
    }
}

void Mx2dDimCategoryManagerDialog::addCategoryToTable(const DimCategoryData& data)
{
	m_bIsUpdatingTable = true;

	int row = m_pTable->rowCount();
	m_pTable->insertRow(row);

	// 0: Name (Editable)
	QTableWidgetItem* nameItem = new QTableWidgetItem(data.name);
	nameItem->setData(Qt::UserRole, data.name);
	nameItem->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	m_pTable->setItem(row, static_cast<int>(TableColumn::Name), nameItem);

	// 1: Color (Uses custom Delegate, just store data)
	QTableWidgetItem* colorItem = new QTableWidgetItem();
	colorItem->setData(Qt::UserRole, data.color); // Delegate reads this to draw the rect
	colorItem->setFlags(colorItem->flags() & ~Qt::ItemIsEditable);
	m_pTable->setItem(row, static_cast<int>(TableColumn::Color), colorItem);

	// 2: Show (Eye Icon, Centered by Delegate)
	QTableWidgetItem* showItem = new QTableWidgetItem();
	showItem->setIcon(QIcon(data.isVisible ? ":/resources/images2d/2d_eyeOpen.svg" : ":/resources/images2d/2d_eyeClose.svg"));
	showItem->setData(Qt::UserRole, data.isVisible); // Store boolean state
	showItem->setFlags(showItem->flags() & ~Qt::ItemIsEditable);
	m_pTable->setItem(row, static_cast<int>(TableColumn::Show), showItem);

	// 3: Current (Check Icon, Centered by Delegate)
	QTableWidgetItem* currentItem = new QTableWidgetItem();
	if (data.isCurrent) {
		currentItem->setIcon(QIcon(":/resources/images2d/2d_checkClassify.svg"));
		m_currentCategoryName = data.name;
	}
	currentItem->setData(Qt::UserRole, data.isCurrent);
	currentItem->setFlags(currentItem->flags() & ~Qt::ItemIsEditable);
	m_pTable->setItem(row, static_cast<int>(TableColumn::Current), currentItem);

	// 4: Delete (Red X Icon, Centered by Delegate)
	QTableWidgetItem* deleteItem = new QTableWidgetItem();
	deleteItem->setIcon(QIcon(":/resources/images2d/2d_deleteClassify.svg"));
	deleteItem->setFlags(deleteItem->flags() & ~Qt::ItemIsEditable);
	m_pTable->setItem(row, static_cast<int>(TableColumn::Delete), deleteItem);

	m_bIsUpdatingTable = false;

	if (data.isCurrent) {
		updateCurrentCategoryLabel();
	}
}

void Mx2dDimCategoryManagerDialog::updateCurrentCategoryLabel()
{
	m_pLblCurrentCategory->setText(tr("Current Category: ") + m_currentCategoryName);
}

QString Mx2dDimCategoryManagerDialog::generateUniqueCategoryName()
{
	int index = 1;
	while (true) {
		QString candidate = tr("Category %1").arg(index);
		if (!isCategoryNameExists(candidate)) {
			return candidate;
		}
		index++;
	}
}

bool Mx2dDimCategoryManagerDialog::isCategoryNameExists(const QString& name, int excludeRow)
{
	for (int i = 0; i < m_pTable->rowCount(); ++i) {
		if (i == excludeRow) continue;
		if (m_pTable->item(i, static_cast<int>(TableColumn::Name))->text() == name) {
			return true;
		}
	}
	return false;
}

void Mx2dDimCategoryManagerDialog::onNewCategoryClicked()
{
	m_pEditSearch->clear();
	DimCategoryData newCat;
	newCat.name = generateUniqueCategoryName();
	newCat.color = QColor(230, 81, 0); // Orange default
	newCat.isVisible = true;
	newCat.isCurrent = false;

	addCategoryToTable(newCat);
	m_pTable->scrollToBottom();
	m_categoryDatas = getCategoryDataFromTable();
    saveConfig(m_fileName);
	emit categoryDataChanged();
}

void Mx2dDimCategoryManagerDialog::onShowAllClicked()
{
	for (int i = 0; i < m_pTable->rowCount(); ++i) {
		QTableWidgetItem* showItem = m_pTable->item(i, static_cast<int>(TableColumn::Show));
		showItem->setData(Qt::UserRole, true);
		showItem->setIcon(QIcon(":/resources/images2d/2d_eyeOpen.svg"));
		QString name = m_pTable->item(i, static_cast<int>(TableColumn::Name))->text();
		m_pAnnoEditor->setAnnotationsVisable(name, true);
	}
	m_categoryDatas = getCategoryDataFromTable();
    saveConfig(m_fileName);
	emit categoryDataChanged();
}

void Mx2dDimCategoryManagerDialog::onHideAllClicked()
{
	for (int i = 0; i < m_pTable->rowCount(); ++i) {
		bool isCurrent = m_pTable->item(i, static_cast<int>(TableColumn::Current))->data(Qt::UserRole).toBool();
		if (!isCurrent) {
			QTableWidgetItem* showItem = m_pTable->item(i, static_cast<int>(TableColumn::Show));
			showItem->setData(Qt::UserRole, false);
			showItem->setIcon(QIcon(":/resources/images2d/2d_eyeClose.svg"));
			QString name = m_pTable->item(i, static_cast<int>(TableColumn::Name))->text();
			m_pAnnoEditor->setAnnotationsVisable(name, false);
		}
	}
	m_categoryDatas = getCategoryDataFromTable();
    saveConfig(m_fileName);
	emit categoryDataChanged();

	QMessageBox::information(this, tr("Information"),
		tr("'%1' is the current category and cannot be hidden. Other categories have been hidden.")
		.arg(m_currentCategoryName));
}

void Mx2dDimCategoryManagerDialog::onSearchTextChanged(const QString& text)
{
	for (int i = 0; i < m_pTable->rowCount(); ++i) {
		QString name = m_pTable->item(i, static_cast<int>(TableColumn::Name))->text();
		m_pTable->setRowHidden(i, !name.contains(text, Qt::CaseInsensitive));
	}
}

void Mx2dDimCategoryManagerDialog::onMoveUpClicked()
{
	int currentRow = m_pTable->currentRow();
	if (currentRow > 0) {
		swapRows(currentRow, currentRow - 1);
		m_pTable->selectRow(currentRow - 1);
	}
	m_categoryDatas = getCategoryDataFromTable();
    saveConfig(m_fileName);
	emit categoryDataChanged();
}

void Mx2dDimCategoryManagerDialog::onMoveDownClicked()
{
	int currentRow = m_pTable->currentRow();
	if (currentRow >= 0 && currentRow < m_pTable->rowCount() - 1) {
		swapRows(currentRow, currentRow + 1);
		m_pTable->selectRow(currentRow + 1);
	}
	m_categoryDatas = getCategoryDataFromTable();
	saveConfig(m_fileName);
	emit categoryDataChanged();
}

void Mx2dDimCategoryManagerDialog::swapRows(int row1, int row2)
{
	m_bIsUpdatingTable = true;
	for (int col = 0; col < m_pTable->columnCount(); ++col) {
		QTableWidgetItem* item1 = m_pTable->takeItem(row1, col);
		QTableWidgetItem* item2 = m_pTable->takeItem(row2, col);
		m_pTable->setItem(row1, col, item2);
		m_pTable->setItem(row2, col, item1);
	}
	m_bIsUpdatingTable = false;
}

QString Mx2dDimCategoryManagerDialog::getCategoryNameFromRow(int row)
{
    return m_pTable->item(row, static_cast<int>(TableColumn::Name))->text();
}

bool Mx2dDimCategoryManagerDialog::hasCategory(const QString& name)
{
    for (int i = 0; i < m_pTable->rowCount(); ++i) {
        if (getCategoryNameFromRow(i) == name) {
            return true;
        }
    }
    return false;
}

void Mx2dDimCategoryManagerDialog::loadMoreFromAnnotations()
{
	auto ids = m_pAnnoEditor->getAllAnnotations();
    for (auto id : ids) {
		McDbObjectPointer<Mx2dCustomAnnotation> spAnno(id, McDb::kForRead);
        if (!hasCategory(spAnno->category())) {
            DimCategoryData categoryData;
            categoryData.name = spAnno->category();
			McCmColor color = spAnno->color();
			categoryData.color = QColor(color.red(), color.green(), color.blue());
            categoryData.isVisible = true;
            categoryData.isCurrent = false;
            addCategoryToTable(categoryData);
            m_categoryDatas.append(categoryData);
			saveConfig(m_fileName);
        }
    }
}

QList<DimCategoryData> Mx2dDimCategoryManagerDialog::getCategoryDataFromTable()
{
    QList<DimCategoryData> categoryDatas;
    for (int i = 0; i < m_pTable->rowCount(); ++i) { 
        DimCategoryData categoryData;
        categoryData.name = getCategoryNameFromRow(i);
        categoryData.color = m_pTable->item(i, static_cast<int>(TableColumn::Color))->data(Qt::UserRole).value<QColor>();
        categoryData.isVisible = m_pTable->item(i, static_cast<int>(TableColumn::Show))->data(Qt::UserRole).toBool();
        categoryData.isCurrent = m_pTable->item(i, static_cast<int>(TableColumn::Current))->data(Qt::UserRole).toBool();
        categoryDatas.append(categoryData);
    }
    return categoryDatas;
}

DimCategoryData Mx2dDimCategoryManagerDialog::getCurrentCategoryData()
{
    for (int i = 0; i < m_pTable->rowCount(); ++i) {
        if (m_pTable->item(i, static_cast<int>(TableColumn::Current))->data(Qt::UserRole).toBool()) {
            return getCategoryDataFromTable().at(i);
        }
    }
	return {};
}

void Mx2dDimCategoryManagerDialog::onTableCellClicked(int row, int column)
{
	if(column != static_cast<int>(TableColumn::Color) && column != static_cast<int>(TableColumn::Show) && column != static_cast<int>(TableColumn::Delete))
		return;

	if (column == static_cast<int>(TableColumn::Color)) {
		QTableWidgetItem* colorItem = m_pTable->item(row, static_cast<int>(TableColumn::Color));
		QColor currentColor = colorItem->data(Qt::UserRole).value<QColor>();

		QColor newColor = QColorDialog::getColor(currentColor, this, tr("Select Color"));
		if (newColor.isValid()) {
			colorItem->setData(Qt::UserRole, newColor); // Delegate will auto-repaint with new color
		}
	}
	else if (column == static_cast<int>(TableColumn::Show)) {
		bool isCurrent = m_pTable->item(row, static_cast<int>(TableColumn::Current))->data(Qt::UserRole).toBool();
		if (isCurrent) {
			QMessageBox::warning(this, tr("Warning"), tr("The current category cannot be hidden."));
			return;
		}

		QTableWidgetItem* showItem = m_pTable->item(row, static_cast<int>(TableColumn::Show));
		bool isVisible = showItem->data(Qt::UserRole).toBool();
		isVisible = !isVisible;

		showItem->setData(Qt::UserRole, isVisible);
		showItem->setIcon(QIcon(isVisible ? ":/resources/images2d/2d_eyeOpen.svg" : ":/resources/images2d/2d_eyeClose.svg"));
		QString CategoryName = m_pTable->item(row, static_cast<int>(TableColumn::Name))->text();
		m_pAnnoEditor->setAnnotationsVisable(CategoryName, isVisible);
	}
	else if (column == static_cast<int>(TableColumn::Delete)) {
		bool isCurrent = m_pTable->item(row, static_cast<int>(TableColumn::Current))->data(Qt::UserRole).toBool();
		if (isCurrent) {
			QMessageBox::warning(this, tr("Warning"), tr("The current category cannot be deleted."));
			return;
		}

		QString name = m_pTable->item(row, static_cast<int>(TableColumn::Name))->text();
		if (m_pAnnoEditor->hasAnnotationInCategory(name))
		{
            QMessageBox::warning(this, tr("Warning"), tr("Please delete all annotations in category '%1' first.").arg(name));
            return;
		}
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(this, tr("Confirm Delete"),
			tr("Are you sure you want to delete category '%1'?").arg(name),
			QMessageBox::Yes | QMessageBox::No);

		if (reply == QMessageBox::Yes) {
			m_pTable->removeRow(row);
		}
	}
	m_categoryDatas = getCategoryDataFromTable();
    saveConfig(m_fileName);
	emit categoryDataChanged();
}

void Mx2dDimCategoryManagerDialog::onTableCellDoubleClicked(int row, int column)
{
	if (column == static_cast<int>(TableColumn::Current)) {
		
		setCurrentCategory(row);
	}
}

void Mx2dDimCategoryManagerDialog::onTableItemChanged(QTableWidgetItem* item)
{
	if (m_bIsUpdatingTable || !item) return;

	if (item->column() == static_cast<int>(TableColumn::Name)) {
		QString newName = item->text().trimmed();
		QString oldName = item->data(Qt::UserRole).toString();

		if (newName.isEmpty()) {
			QMessageBox::warning(this, tr("Warning"), tr("Category name cannot be empty."));
			m_bIsUpdatingTable = true;
			item->setText(oldName);
			m_bIsUpdatingTable = false;
			return;
		}

		if (newName != oldName && isCategoryNameExists(newName, item->row())) {
			QMessageBox::warning(this, tr("Warning"), tr("Category name already exists."));
			m_bIsUpdatingTable = true;
			item->setText(oldName);
			m_bIsUpdatingTable = false;
			return;
		}

		item->setData(Qt::UserRole, newName);

		bool isCurrent = m_pTable->item(item->row(), static_cast<int>(TableColumn::Current))->data(Qt::UserRole).toBool();
		if (isCurrent) {
			m_currentCategoryName = newName;
			updateCurrentCategoryLabel();
		}
		m_categoryDatas = getCategoryDataFromTable();
		saveConfig(m_fileName);
		emit categoryDataChanged();
		m_pAnnoEditor->modifyAnnotationsCategory(oldName, newName);
	}
}

void Mx2dDimCategoryManagerDialog::setCurrentCategory(int row)
{
	for (int i = 0; i < m_pTable->rowCount(); ++i) {
		QTableWidgetItem* currentItem = m_pTable->item(i, static_cast<int>(TableColumn::Current));
		if (i == row) {
			currentItem->setData(Qt::UserRole, true);
			currentItem->setIcon(QIcon(":/resources/images2d/2d_checkClassify.svg"));
			m_currentCategoryName = m_pTable->item(i, static_cast<int>(TableColumn::Name))->text();

			// Force show if it was hidden
			QTableWidgetItem* showItem = m_pTable->item(i, static_cast<int>(TableColumn::Show));
			if (!showItem->data(Qt::UserRole).toBool()) {
				showItem->setData(Qt::UserRole, true);
				showItem->setIcon(QIcon(":/resources/images2d/2d_eyeOpen.svg"));
			}
		}
		else {
			currentItem->setData(Qt::UserRole, false);
			currentItem->setIcon(QIcon()); // Clear icon
		}
	}
	updateCurrentCategoryLabel();
	m_categoryDatas = getCategoryDataFromTable();
	saveConfig(m_fileName);
	emit categoryDataChanged();
	m_pTable->selectRow(row);
}
