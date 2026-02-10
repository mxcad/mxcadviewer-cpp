/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "Mx2dTextSearchDialog.h"
#include <QHeaderView>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QUrl>
#include "xlsxwriter.h"
#include "Mx2dSignalTransfer.h"
#include "MxUtils.h"
#include "Mx2dGuiDocument.h"
#include "Mx2dCustomAnnotation.h"

Mx2dTextSearchDialog::Mx2dTextSearchDialog(Mx2dGuiDocument* guiDoc)
	: QDialog(guiDoc), m_guiDoc(guiDoc)
{
	Qt::WindowFlags flags = Qt::Dialog;
	flags |= Qt::CustomizeWindowHint;
	flags |= Qt::WindowTitleHint;
	flags |= Qt::WindowCloseButtonHint;
	setWindowFlags(flags);

	initControls();
	initLayout();
	//onSearchResults();

	setWindowTitle(tr("Text Search"));

	adjustSize();
	setFixedSize(size());
}

Mx2dTextSearchDialog::~Mx2dTextSearchDialog()
{
}

void Mx2dTextSearchDialog::initControls()
{
	// Search content
	m_pSearchContentCombo = new QComboBox(this);
	m_pSearchContentCombo->setFixedWidth(400);
	m_pSearchContentCombo->setEditable(true);

	m_pSearchBtn = new QPushButton(tr("Search"), this);
	connect(m_pSearchBtn, &QPushButton::clicked, this, &Mx2dTextSearchDialog::onSearchClicked); // Execute text search command
	connect(&Mx2dSignalTransfer::getInstance(), &Mx2dSignalTransfer::signalTextSearched, this, &Mx2dTextSearchDialog::onSearchResults); // Display search results
	connect(&Mx2dSignalTransfer::getInstance(), &Mx2dSignalTransfer::signalTextSearchRect, this, [this](QWidget* tab, const Mx2d::Rect2D& rect) {
		if (tab != m_guiDoc)
			return;
		m_searchAreaRect = rect;
		});
	connect(&Mx2dSignalTransfer::getInstance(), &Mx2dSignalTransfer::signalTextSearchPoly, this, [this](QWidget* tab, const Mx2d::Point2DList& poly) {
		if (tab != m_guiDoc)
			return;
		if (Mx2d::isPolygonSelfIntersecting(poly))
		{
            m_searchAreaIrregular.clear();
            QMessageBox::information(this, tr("Prompt"), tr("The irregular area cannot be a self-intersecting polygon."));
			return;
		}
		m_searchAreaIrregular = poly;
		});

	// Search conditions
	m_pFullMatchCheck = new QCheckBox(tr("Exact match"), this);

	// Search area
	m_pWholePaperRadio = new QRadioButton(tr("Entire drawing"), this);
	m_pRectAreaRadio = new QRadioButton(tr("Rectangular area search"), this);
	m_pRectAreaRadio->setStyleSheet("QRadioButton {color: #BA9600;}");
	m_pIrregularAreaRadio = new QRadioButton(tr("Irregular area search"), this);
    m_pIrregularAreaRadio->setStyleSheet("QRadioButton {color: #BA9600;}");
	m_pWholePaperRadio->setChecked(true);
	// Search area button group
	m_pAreaButtonGroup = new QButtonGroup(this);
	m_pAreaButtonGroup->setExclusive(false);
	m_pAreaButtonGroup->addButton(m_pWholePaperRadio, 0);
	m_pAreaButtonGroup->addButton(m_pRectAreaRadio, 1);
	m_pAreaButtonGroup->addButton(m_pIrregularAreaRadio, 2);
	connect(m_pAreaButtonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, [this](int id) {
		auto checkButton = [this](int id) {
			for (auto* btn : m_pAreaButtonGroup->buttons()) {
				btn->setChecked(false);
			}
			m_pAreaButtonGroup->button(id)->setChecked(true);
		};
		if (id == 0)
		{
			checkButton(id);
			return;
		}
		checkButton(0);
		MxUtils::doAction([checkButton, id]() {
			checkButton(id);
			});
		});

	m_pSelectAreaBtn = new QPushButton(QIcon(":/resources/images2d/2d_textSearchRegion.svg"), tr("Select Area"), this);
	m_pSelectAreaBtn->setEnabled(false);
	connect(m_pSelectAreaBtn, &QPushButton::clicked, this, &Mx2dTextSearchDialog::onSelectAreaClicked);

	connect(m_pWholePaperRadio, &QRadioButton::toggled, this, &Mx2dTextSearchDialog::onSearchAreaChanged);
	connect(m_pRectAreaRadio, &QRadioButton::toggled, this, &Mx2dTextSearchDialog::onSearchAreaChanged);
	connect(m_pIrregularAreaRadio, &QRadioButton::toggled, this, &Mx2dTextSearchDialog::onSearchAreaChanged);

	// Search scope
	m_pPaperTextRadio = new QRadioButton(tr("Text in the drawing itself"), this);
	m_pAppTextRadio = new QRadioButton(tr("Text and measurement annotations added by this software"), this);
    m_pAppTextRadio->setStyleSheet("QRadioButton {color: #BA9600;}");
	m_pPaperTextRadio->setChecked(true);
	// Search scope button group
	m_pScopeButtonGroup = new QButtonGroup(this);
	m_pScopeButtonGroup->setExclusive(false);
	m_pScopeButtonGroup->addButton(m_pPaperTextRadio, 0);
	m_pScopeButtonGroup->addButton(m_pAppTextRadio, 1);
	connect(m_pScopeButtonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, [this](int id) {
		auto checkButton = [this](int id) {
			for (auto* btn : m_pScopeButtonGroup->buttons()) {
				btn->setChecked(false);
			}
			m_pScopeButtonGroup->button(id)->setChecked(true);
			};
		if (id == 0)
		{
			checkButton(id);
			return;
		}
		checkButton(0);
		MxUtils::doAction([checkButton, id]() {
			checkButton(id);
			});
		});

	// Result related controls
	m_pShowResultCheck = new QCheckBox(tr("Show results"), this);
	m_pShowResultCheck->setChecked(false);
	connect(m_pShowResultCheck, &QCheckBox::toggled, this, &Mx2dTextSearchDialog::onToggleShowResult);

	m_pResultCountLabel = new QLabel(tr("(Found 0)"), this);
	m_pMarkAllCheck = new QCheckBox(tr("Mark all results"), this);
	m_pMarkAllCheck->setChecked(true);

	// Table
	m_pResultTable = new QTableWidget(this);
	m_pResultTable->verticalHeader()->setVisible(false);
	m_pResultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_pResultTable->setSelectionBehavior(QAbstractItemView::SelectRows); // Set to select entire rows
	m_pResultTable->setSelectionMode(QAbstractItemView::SingleSelection); // Set to single selection
	m_pResultTable->setColumnCount(2);
	m_pResultTable->setHorizontalHeaderLabels(QStringList() << tr("No.") << tr("Text Content"));
	m_pResultTable->horizontalHeader()->setStretchLastSection(true);
	m_pResultTable->setColumnWidth(0, 50);
	// Connect table item change signal for handling manual clicks
	connect(m_pResultTable, &QTableWidget::currentItemChanged, this, &Mx2dTextSearchDialog::onCurrentItemChanged);
	// Table container resultPanel
	m_pResultPanel = new QWidget(this);
	QVBoxLayout* panelLayout = new QVBoxLayout(m_pResultPanel);
	panelLayout->setContentsMargins(0, 0, 0, 0);
	panelLayout->addWidget(m_pResultTable);
	m_pResultPanel->setLayout(panelLayout);
	m_pResultPanel->setVisible(false);
	m_pResultPanel->setMinimumHeight(300);

	// Bottom buttons
	m_pExportExcelBtn = new QPushButton(tr("Export to Excel"), this);
	m_pExportExcelBtn->setStyleSheet("QPushButton {color: #BA9600;}");
	m_pNextResultBtn = new QPushButton(tr("View Next"), this);
	m_pExportExcelBtn->setEnabled(false);
	m_pNextResultBtn->setEnabled(false);
	m_pCompleteBtn = new QPushButton(tr("Complete"), this);

	// Connect slot functions for buttons
	connect(m_pNextResultBtn, &QPushButton::clicked, this, &Mx2dTextSearchDialog::onNextResultClicked);
	connect(m_pExportExcelBtn, &QPushButton::clicked, this, [this]() {
		MxUtils::doAction([this]() {
			onExportToExcelClicked();
			});
		});
	connect(m_pCompleteBtn, &QPushButton::clicked, this, &QDialog::close);
}

void Mx2dTextSearchDialog::initLayout()
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	// Search content layout
	QHBoxLayout* searchContentLayout = new QHBoxLayout();
	searchContentLayout->addWidget(new QLabel(tr("Search content:"), this));
	searchContentLayout->addWidget(m_pSearchContentCombo);
	searchContentLayout->addStretch();
	searchContentLayout->addWidget(m_pSearchBtn);

	// Search conditions layout
	QHBoxLayout* searchConditionLayout = new QHBoxLayout();
	searchConditionLayout->addWidget(new QLabel(tr("Search conditions:"), this));
	searchConditionLayout->addWidget(m_pFullMatchCheck);
	searchConditionLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

	// Search area layout
	QHBoxLayout* searchAreaLayout = new QHBoxLayout();
	searchAreaLayout->addWidget(new QLabel(tr("Search area:"), this));
	searchAreaLayout->addWidget(m_pWholePaperRadio);
	searchAreaLayout->addWidget(m_pRectAreaRadio);
	searchAreaLayout->addWidget(m_pIrregularAreaRadio);
	searchAreaLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
	searchAreaLayout->addWidget(m_pSelectAreaBtn);

	// Search scope layout
	QHBoxLayout* searchScopeLayout = new QHBoxLayout();
	searchScopeLayout->addWidget(new QLabel(tr("Search scope:"), this));
	searchScopeLayout->addWidget(m_pPaperTextRadio);
	searchScopeLayout->addWidget(m_pAppTextRadio);
	searchScopeLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

	// Show results controls layout (not in resultPanel)
	QHBoxLayout* resultTopLayout = new QHBoxLayout();
	resultTopLayout->addWidget(m_pShowResultCheck);
	resultTopLayout->addWidget(m_pResultCountLabel);
	resultTopLayout->addStretch();
	resultTopLayout->addWidget(m_pMarkAllCheck);

	// Bottom buttons layout
	QHBoxLayout* bottomLayout = new QHBoxLayout();
	bottomLayout->addWidget(m_pExportExcelBtn);
	bottomLayout->addStretch();
	bottomLayout->addWidget(m_pNextResultBtn);
	bottomLayout->addWidget(m_pCompleteBtn);

	// Main layout assembly
	mainLayout->addLayout(searchContentLayout);
	mainLayout->addLayout(searchConditionLayout);
	mainLayout->addLayout(searchAreaLayout);
	mainLayout->addLayout(searchScopeLayout);
	mainLayout->addLayout(resultTopLayout);
	mainLayout->addWidget(m_pResultPanel);
	mainLayout->addLayout(bottomLayout);

	setLayout(mainLayout);
	adjustSize();
	resize(700, height());
}

void Mx2dTextSearchDialog::addToTable(int count, const QString& text, Mx2d::Extents ext2d)
{
	QTableWidgetItem* numberItem = new QTableWidgetItem(QString::number(count + 1));
	numberItem->setTextAlignment(Qt::AlignCenter);
	m_pResultTable->setItem(count, 0, numberItem);

	QTableWidgetItem* textItem = new QTableWidgetItem(text);

	textItem->setData(Qt::UserRole, ext2d.minX);
	textItem->setData(Qt::UserRole + 1, ext2d.minY);
	textItem->setData(Qt::UserRole + 2, ext2d.maxX);
	textItem->setData(Qt::UserRole + 3, ext2d.maxY);
	m_pResultTable->setItem(count, 1, textItem);
}

void Mx2dTextSearchDialog::updateSearchResult()
{
	int count = 0;
	m_pResultTable->setRowCount(0);

	for (const auto& pair : m_results) {
		const QString& currentText = pair.first;

		m_pResultTable->insertRow(count);
		addToTable(count, currentText, pair.second);
		count++;
	}

	m_pResultTable->setRowCount(count);
	m_pResultCountLabel->setText(QString(tr("(Found %1)")).arg(count));
	if (m_pResultTable->rowCount() > 0)
	{
		m_pResultTable->selectRow(0);
		m_pShowResultCheck->setChecked(true);
		m_pResultTable->setFocus();
		m_pExportExcelBtn->setEnabled(true);
		m_pNextResultBtn->setEnabled(true);
	}
	else {
		// Search completed, no results found
		m_pExportExcelBtn->setEnabled(false);
		m_pNextResultBtn->setEnabled(false);
		QMessageBox::information(this, tr("Prompt"), tr("Search completed, no results found"));
	}

	m_results.clear();
}

void Mx2dTextSearchDialog::closeEvent(QCloseEvent*)
{
	MxDrawApp::ClearVectorLine(m_guiDoc->cadView().GetOcxHandle());
	MxDrawApp::UpdateDisplay(m_guiDoc->cadView().GetOcxHandle());
}

int Mx2dTextSearchDialog::getCheckedButtonId(QButtonGroup* group)
{
	auto btns = group->buttons();
	for (auto & btn : btns)
	{
		if (btn->isChecked())
			return group->id(btn);
	}
    return -1;
}

Mx2d::TextInfoList Mx2dTextSearchDialog::searchText(const QString& text, bool isExactMatch, int searchArea, int searchScope, bool markAllResults)
{
	MrxDbgSelSet ss;
	MrxDbgRbList spFilter = Mx::mcutBuildList(RTDXF0, searchScope == 0 ? "TEXT,MTEXT,INSERT" : "AnnotationMark,RectCloudMark,RectMark,LineMark,EllipseMark,LeaderMark,TextMark,MTextMark,PolyAreaMark,RectAreaMark,CartesianCoordMark,AlignedDimMark,LinearDimMark,ContinuousMeasurementMark,RadiusDimMark,ArcLengthDimMark,CircleMeasurementMark,AngleMeasurementMark,ArcPolyAreaMark,HatchAreaMark,HatchArea2Mark,BatchMeasurementMark", 0);
	if (searchArea == 1)
	{
		McGePoint3d pt1(m_searchAreaRect.first.x, m_searchAreaRect.first.y, 0);
		McGePoint3d pt2(m_searchAreaRect.second.x, m_searchAreaRect.second.y, 0);

		if (ss.windowSelect(pt1, pt2, spFilter.data()) != MrxDbgSelSet::kSelected) 
		{
			qDebug() << "windowSelect failed!";
			qDebug() << QString("(%1, %2)").arg(pt1.x).arg(pt1.y);
			qDebug() << QString("(%1, %2)").arg(pt2.x).arg(pt2.y);
			return {};
		}
	}
	else if (searchArea == 2)
	{
		if (m_searchAreaIrregular.empty())
			return {};
		McGePoint3dArray ary;
		for (auto& pt2d : m_searchAreaIrregular)
		{
			ary.append(McGePoint3d(pt2d.x, pt2d.y, 0));
			//qDebug() << QString("(%1, %2)").arg(pt2d.x).arg(pt2d.y);
		}
		Mx2d::Extents ext2d = Mx2d::getPolygonGeomExtents(ary);
		MrxDbgSelSet::SelSetStatus status = ss.crossingSelect(ext2d.bottomRight3d(), ext2d.topLeft3d(), spFilter.data());
		qDebug() << "crossingSelect status:" << status;
		if (status != MrxDbgSelSet::kSelected)
		{
			return {};
		}
	}
	else
	{
		if (ss.allSelect(spFilter.data()) != MrxDbgSelSet::kSelected)
		{
			qDebug() << "allSelect failed!";
			return {};
		}
	}
	
	McDbObjectIdArray aryId;
	ss.asArray(aryId);

	qDebug() << "Selected entities:" << aryId.length();

	Mx2d::TextInfoList res;
	for (int i = 0; i < aryId.length(); i++)
	{
		McDbObjectPointer<McDbEntity> spEntity(aryId[i], McDb::kForRead);
		if (spEntity.openStatus() != Mcad::eOk)
			continue;
		if (spEntity->isKindOf(Mx2dCustomAnnotation::desc()))
		{
            Mx2dCustomAnnotation* pAnnotation = Mx2dCustomAnnotation::cast(spEntity.object());
			res.append(pAnnotation->findText(text, isExactMatch));
		}
		else if (spEntity->isA() == McDbText::desc())
		{
			McDbText* pText = McDbText::cast(spEntity.object());
			QString textStr = QString::fromLocal8Bit(pText->textString());
			McDbExtents ext;
			pText->getGeomExtents(ext, false);
			McGePoint3d minPt = ext.minPoint();
            McGePoint3d maxPt = ext.maxPoint();
			Mx2d::Extents ext2d{ minPt.x, minPt.y, maxPt.x,maxPt.y };

			if ((isExactMatch && (textStr != text)) || (!isExactMatch && !textStr.contains(text, Qt::CaseInsensitive)))
			{
				continue;
			}
			res.append({ textStr , ext2d });
		}
		else if (spEntity->isA() == McDbText::desc())
		{
            McDbMText* pMText = McDbMText::cast(spEntity.object());
            QString textStr = QString::fromLocal8Bit(pMText->contents());
            McDbExtents ext;
            pMText->getGeomExtents(ext, false);
            McGePoint3d minPt = ext.minPoint();
            McGePoint3d maxPt = ext.maxPoint();
            Mx2d::Extents ext2d{ minPt.x, minPt.y, maxPt.x,maxPt.y };
            if ((isExactMatch && (textStr != text)) || (!isExactMatch && !textStr.contains(text, Qt::CaseInsensitive)))
            {
                continue;
            }
            res.append({ textStr , ext2d });
		}
		else if (spEntity->isA() == McDbBlockReference::desc())
		{
            McDbBlockReference* pBlockRef = McDbBlockReference::cast(spEntity.object());
			McGeMatrix3d mat = pBlockRef->blockTransform(true);
			McDbObjectId blockId = pBlockRef->blockTableRecord();
			Mx2d::TextInfoList res2 = traverseTextInBlockTableRecord(blockId, mat);
			for (const auto& info : res2)
			{
				QString textStr = info.first;
				if ((isExactMatch && (textStr != text)) || (!isExactMatch && !textStr.contains(text, Qt::CaseInsensitive)))
				{
					continue;
				}
				res.append(info);
			}
		}
	}
	if (searchArea == 2)
	{
		Mx2d::TextInfoList resInPolygon;
		for (const auto& info : res)
		{
			Mx2d::Extents ext = info.second;
			if (Mx2d::isPointInPolygon(ext.center(), m_searchAreaIrregular) != 0)
				resInPolygon.append(info);
		}
        return resInPolygon;
	}
	return res;
}

Mx2d::TextInfoList Mx2dTextSearchDialog::traverseTextInBlockTableRecord(McDbObjectId btrId, const McGeMatrix3d& trsfMat)
{
	McDbBlockTableRecordPointer spBTR(btrId, McDb::kForRead);
	McDbBlockTableRecordIterator* pIterator = nullptr;
	Mcad::ErrorStatus es = spBTR->newIterator(pIterator);
	if (es != Mcad::eOk || pIterator == nullptr)
	{
		return {};
	}
	Mx2d::TextInfoList res;
	for (; !pIterator->done(); pIterator->step())
	{
		McDbEntity* pEntity = nullptr;
		es = pIterator->getEntity(pEntity, McDb::kForRead);
		if (es != Mcad::eOk || !pEntity)
		{
			continue;
		}
		if (pEntity->isA() == McDbText::desc())
		{
			McDbText* pText = McDbText::cast(pEntity);
			if (pText)
			{
				QString textStr = QString::fromLocal8Bit(pText->textString());
                McDbExtents ext;
                pText->getGeomExtents(ext, false);
                McGePoint3d minPt = ext.minPoint();
                McGePoint3d maxPt = ext.maxPoint();
                minPt.transformBy(trsfMat);
                maxPt.transformBy(trsfMat);
                Mx2d::Extents ext2d{ minPt.x, minPt.y, maxPt.x,maxPt.y };
                res.append({ textStr , ext2d });
			}
		}
		else if (pEntity->isA() == McDbMText::desc())
		{
			McDbMText* pMText = McDbMText::cast(pEntity);
			if (pMText)
			{
				QString textStr = QString::fromLocal8Bit(pMText->contents());
                McDbExtents ext;
                pMText->getGeomExtents(ext, false);
                McGePoint3d minPt = ext.minPoint();
                McGePoint3d maxPt = ext.maxPoint();
				minPt.transformBy(trsfMat);
				maxPt.transformBy(trsfMat);
                Mx2d::Extents ext2d{ minPt.x, minPt.y, maxPt.x,maxPt.y };
                res.append({ textStr , ext2d });
			}
		}
		else if (pEntity->isA() == McDbBlockReference::desc())
		{
			McDbBlockReference* pBlockRef = McDbBlockReference::cast(pEntity);
			McGeMatrix3d mat = pBlockRef->blockTransform(true);
			McDbObjectId blockId = pBlockRef->blockTableRecord();
			Mx2d::TextInfoList res2 = traverseTextInBlockTableRecord(blockId, mat);
			res.append(res2);
		}
		pEntity->close();
	}

	delete pIterator;
	pIterator = nullptr;
	return res;
}

void Mx2dTextSearchDialog::drawRectByCorner(const McGePoint3d& corner1, const McGePoint3d& corner2, MxColor color, bool isReserve)
{
	double minX = corner1.x < corner2.x ? corner1.x : corner2.x;
	double maxX = corner1.x < corner2.x ? corner2.x : corner1.x;
	double minY = corner1.y < corner2.y ? corner1.y : corner2.y;
	double maxY = corner1.y < corner2.y ? corner2.y : corner1.y;

	McGePoint3d pt1(minX, maxY, 0);
	McGePoint3d pt2(maxX, maxY, 0);
	McGePoint3d pt3(maxX, minY, 0);
	McGePoint3d pt4(minX, minY, 0);
	MxDrawApp::DrawVectorLine(pt1, pt2, color, isReserve);
	MxDrawApp::DrawVectorLine(pt2, pt3, color, isReserve);
	MxDrawApp::DrawVectorLine(pt3, pt4, color, isReserve);
	MxDrawApp::DrawVectorLine(pt4, pt1, color, isReserve);
}

void Mx2dTextSearchDialog::moveViewCenterTo(Mx2d::Extents ext2d)
{
	MxDrawApp::ClearVectorLine(m_guiDoc->cadView().GetOcxHandle());

	double minX = ext2d.minX;
	double minY = ext2d.minY;
	double maxX = ext2d.maxX;
	double maxY = ext2d.maxY;

	double width = maxX - minX;
	double height = maxY - minY;
	McGePoint3d centerPoint(ext2d.centerX(), ext2d.centerY(), 0);

	MxDrawApp::ZoomW(centerPoint.x - width * 1, centerPoint.y - height * 6, centerPoint.x + width * 1, centerPoint.y + height * 6);
	drawRectByCorner(McGePoint3d(minX, minY, 0), McGePoint3d(maxX, maxY, 0), 0xE65100, true);
	MxDrawApp::UpdateDisplay(m_guiDoc->cadView().GetOcxHandle());
}

// Call with id parameter
void Mx2dTextSearchDialog::onSearchResults(const Mx2d::TextInfoList& result)
{
	m_results = result;

	if (m_pWholePaperRadio->isChecked())
	{
		updateSearchResult();
	}
}


void Mx2dTextSearchDialog::onSearchClicked()
{
	MxDrawApp::ClearVectorLine(m_guiDoc->cadView().GetOcxHandle());
	MxDrawApp::UpdateDisplay(m_guiDoc->cadView().GetOcxHandle());
	QString text = m_pSearchContentCombo->currentText().trimmed();
	if (text.isEmpty())
	{
		m_pResultTable->setRowCount(0);
		m_pResultCountLabel->setText(QString(tr("(Found %1)")).arg(0));
		m_pExportExcelBtn->setEnabled(false);
		m_pNextResultBtn->setEnabled(false);
		QMessageBox::information(this, tr("Prompt"), tr("Please enter search content"));
		return;
	}
	m_results = searchText(text, m_pFullMatchCheck->isChecked(), getCheckedButtonId(m_pAreaButtonGroup), getCheckedButtonId(m_pScopeButtonGroup), m_pMarkAllCheck->isChecked());

	if (m_results.isEmpty())
	{
		m_pResultTable->setRowCount(0);
		m_pResultCountLabel->setText(QString(tr("(Found %1)")).arg(0));
		m_pExportExcelBtn->setEnabled(false);
		m_pNextResultBtn->setEnabled(false);
		QMessageBox::information(this, tr("Prompt"), tr("Search completed, no results found"));

		return;
	}
	updateSearchResult();
	m_pSearchContentCombo->insertItem(0, text);
}


void Mx2dTextSearchDialog::onSearchAreaChanged()
{
	m_pSelectAreaBtn->setEnabled(m_pRectAreaRadio->isChecked() || m_pIrregularAreaRadio->isChecked());
}

void Mx2dTextSearchDialog::onSelectAreaClicked()
{
	if (getCheckedButtonId(m_pAreaButtonGroup) == 1)
	{
		m_guiDoc->executeCommand("Mx_GetRectArea");
	}
	else if (getCheckedButtonId(m_pAreaButtonGroup) == 2)
	{
		m_guiDoc->executeCommand("Mx_GetPolyArea");
	}
}

void Mx2dTextSearchDialog::onToggleShowResult(bool checked)
{
	m_pResultPanel->setVisible(checked);

	adjustSize();
}

// "View Next" button slot function implementation
void Mx2dTextSearchDialog::onNextResultClicked()
{
	m_pResultTable->setFocus();
	int rowCount = m_pResultTable->rowCount();
	if (rowCount == 0)
	{
		return; // No content in table, return directly
	}

	int currentRow = m_pResultTable->currentRow();

	// If no row is currently selected (e.g., just loaded), default to first row
	if (currentRow < 0)
	{
		currentRow = 0;
		m_pResultTable->selectRow(currentRow);
	}

	// Check if already at last row
	if (currentRow >= rowCount - 1)
	{
		QMessageBox::information(this, tr("Prompt"), tr("Already at the last result"));
		return;
	}

	// Select next row
	int nextRow = currentRow + 1;
	m_pResultTable->selectRow(nextRow);
}

// Slot function for handling table current item changes
void Mx2dTextSearchDialog::onCurrentItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous)
{
	// Check if current item is valid
	if (!current)
	{
		return;
	}

	// Get current item's row number
	int currentRow = current->row();

	// Get the item in the second column (index 1) of that row
	QTableWidgetItem* textItem = m_pResultTable->item(currentRow, 1);

	// Ensure second column item exists, then print its content
	if (textItem)
	{
		double minX = textItem->data(Qt::UserRole).toDouble();
		double minY = textItem->data(Qt::UserRole + 1).toDouble();
		double maxX = textItem->data(Qt::UserRole + 2).toDouble();
		double maxY = textItem->data(Qt::UserRole + 3).toDouble();
		moveViewCenterTo({minX, minY, maxX, maxY});
	}
}

void Mx2dTextSearchDialog::onExportToExcelClicked()
{
	// 1. Check if table has data
	if (m_pResultTable->rowCount() == 0) {
		QMessageBox::information(this, tr("Prompt"), tr("No data in table to export."));
		return;
	}

	// 2. Get desktop path and build default filename
	QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
	QString defaultFilePath = desktopPath + "/" + tr("TextSearchResults.xlsx");

	// 3. Create file save dialog
	QFileDialog saveFileDlg(this, tr("Export to Excel"), defaultFilePath, tr("Excel files (*.xlsx);;All files (*)"));
	saveFileDlg.setDefaultSuffix("xlsx");
	saveFileDlg.setFileMode(QFileDialog::AnyFile);
	saveFileDlg.setAcceptMode(QFileDialog::AcceptSave);
	saveFileDlg.setOption(QFileDialog::DontConfirmOverwrite, false);

	if (saveFileDlg.exec() != QDialog::Accepted) {
		return;
	}

	// 4. Get selected file path
	QString filePath = saveFileDlg.selectedFiles().first();

	// 5. Create new workbook
	lxw_workbook* workbook = workbook_new(filePath.toUtf8().constData());
	if (!workbook) {
		QMessageBox::warning(this, tr("Error"), tr("Cannot create workbook"));
		return;
	}

	// 6. Create worksheet
	lxw_worksheet* worksheet = workbook_add_worksheet(workbook, NULL);
	if (!worksheet) {
		workbook_close(workbook);
		QMessageBox::warning(this, tr("Error"), tr("Cannot create worksheet"));
		return;
	}

	// 7. Create cell formats
	lxw_format* header_format = workbook_add_format(workbook);
	format_set_bold(header_format);
	format_set_align(header_format, LXW_ALIGN_CENTER);
	format_set_align(header_format, LXW_ALIGN_VERTICAL_CENTER);
	format_set_border(header_format, LXW_BORDER_THIN);

	lxw_format* cell_format = workbook_add_format(workbook);
	format_set_align(cell_format, LXW_ALIGN_CENTER);
	format_set_align(cell_format, LXW_ALIGN_VERTICAL_CENTER);
	format_set_border(cell_format, LXW_BORDER_THIN);
	format_set_text_wrap(cell_format);

	// 8. Write headers
	for (int column = 0; column < m_pResultTable->columnCount(); ++column) {
		QString header = m_pResultTable->horizontalHeaderItem(column)->text();
		worksheet_write_string(worksheet, 0, column, header.toUtf8().constData(), header_format);
	}

	// 9. Write table data
	for (int row = 0; row < m_pResultTable->rowCount(); ++row) {
		for (int column = 0; column < m_pResultTable->columnCount(); ++column) {
			QTableWidgetItem* item = m_pResultTable->item(row, column);
			QString text = item ? item->text() : "";
			worksheet_write_string(worksheet, row + 1, column, text.toUtf8().constData(), cell_format);
		}
	}

	// 10. Adjust column widths
	for (int column = 0; column < m_pResultTable->columnCount(); ++column) {
		worksheet_set_column(worksheet, column, column, column == 0 ? 15 : 40, NULL);
	}

	// 11. Save and close workbook
	lxw_error err = workbook_close(workbook);
	if (err != LXW_NO_ERROR) {
		QMessageBox::warning(this, tr("Error"), tr("Failed to save file, please check if file is being used by another program"));
		return;
	}

	// 12. Notify user of successful export and ask to open file
	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(this, tr("Prompt"),
		tr("Export to Excel successful! Open file now?"),
		QMessageBox::Yes | QMessageBox::No,
		QMessageBox::Yes);

	if (reply == QMessageBox::Yes) {
		QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
	}
}