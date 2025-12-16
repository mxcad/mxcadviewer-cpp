/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#include "MxRecentFileWidget.h"
#include "MxRecentFileItem.h"
#include "MxRecentFileManager.h"
#include "MxMenu.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QStackedWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QSpacerItem>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QEvent>
#include <QFileInfo>
#include <QDebug>
#include <algorithm>

MxRecentFileWidget::MxRecentFileWidget(QWidget* parent)
    : QWidget(parent)
{
    setupUi();


    MxMenu* dropMenu = new MxMenu(customComboBox);
    connect(customComboBox, &QPushButton::clicked, [this, dropMenu]() { dropMenu->showAt(customComboBox); });
    connect(dropMenu, &MxMenu::itemSelected, customComboBox, &QPushButton::setText);
    connect(dropMenu, &MxMenu::itemSelected, this, QOverload<const QString&>::of(&MxRecentFileWidget::onSortItems));

    QStringList items = { tr("Date"), tr("Name"), tr("Type"), tr("Size") };
    for (const QString& item : items) {
        dropMenu->addItem(item);
    }


    QLabel* searchIcon = new QLabel(lineEdit_search);
    searchIcon->setGeometry(0, 0, 24, 24);
    searchIcon->setScaledContents(true);
    searchIcon->move(10, 8);
    searchIcon->setPixmap(QPixmap(":/resources/images/search.svg").scaled(24, 24, Qt::KeepAspectRatio));

    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("background-color: transparent;");

    m_pGridLayout = new QGridLayout(scrollAreaWidgetContents);
    m_pGridLayout->setHorizontalSpacing(10);
    m_pGridLayout->setVerticalSpacing(10);
    m_pGridLayout->setContentsMargins(0, 0, 0, 0);
    m_pGridLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    scrollAreaWidgetContents->setLayout(m_pGridLayout);

    QList<MxRecentFile> recentFiles = MxRecentFileManager::instance()->getRecentFiles();

    for (int i = 0; i < recentFiles.size(); i++) {
        MxRecentFile recentFile = recentFiles[i];
        MxRecentFileItem* widget = new MxRecentFileItem(recentFile, scrollAreaWidgetContents);
        m_listRecentFileItems.append(widget);
    }

    connect(MxRecentFileManager::instance(), &MxRecentFileManager::recentFilesChanged, this, &MxRecentFileWidget::onUpdateItems);
    connect(lineEdit_search, &QLineEdit::textChanged, this, &MxRecentFileWidget::onUpdateLayout);
    label_reversed->installEventFilter(this);

    sortItems(m_currentSortType);
    updateLayout();
}

MxRecentFileWidget::~MxRecentFileWidget()
{
    qDeleteAll(m_listRecentFileItems);
}

void MxRecentFileWidget::setupUi()
{
    if (this->objectName().isEmpty())
        this->setObjectName(QString::fromUtf8("RecentFile"));
    this->resize(1041, 938);

    auto* verticalLayout_3 = new QVBoxLayout(this);
    verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
    verticalLayout_3->addItem(new QSpacerItem(20, 50, QSizePolicy::Minimum, QSizePolicy::Fixed));

    auto* horizontalLayout_2 = new QHBoxLayout();
    horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
    horizontalLayout_2->addItem(new QSpacerItem(40, 20, QSizePolicy::Fixed, QSizePolicy::Minimum));

    auto* label_3 = new QLabel(this);
    label_3->setObjectName(QString::fromUtf8("label_3"));
    label_3->setFixedSize(32, 32);
    label_3->setPixmap(QPixmap(":/resources/images/recent-white.svg"));
    label_3->setScaledContents(true);
    horizontalLayout_2->addWidget(label_3);

    auto* label_2 = new QLabel(this);
    label_2->setObjectName(QString::fromUtf8("label_2"));
    label_2->setStyleSheet("QLabel { color: #FFFFFF; font-size: 20px; }");
    label_2->setText(tr("Recent Files"));
    horizontalLayout_2->addWidget(label_2);

    horizontalLayout_2->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    horizontalLayout_2->addItem(new QSpacerItem(100, 20, QSizePolicy::Fixed, QSizePolicy::Minimum));

    lineEdit_search = new QLineEdit(this);
    lineEdit_search->setObjectName(QString::fromUtf8("lineEdit_search"));
    lineEdit_search->setFixedSize(300, 40);
    lineEdit_search->setStyleSheet(
        "QLineEdit {"
        "    border: none; background-color: rgba(196, 196, 196, 0.2);"
        "    padding-left: 40px; font-size: 18px; border-radius: 10px;"
        "}"
        "QLineEdit[placeholderText] { color: #ffffff; }"
    );
    lineEdit_search->setPlaceholderText(tr("Search recent files by name"));
    horizontalLayout_2->addWidget(lineEdit_search);

    horizontalLayout_2->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    auto* label_4 = new QLabel(this);
    label_4->setObjectName(QString::fromUtf8("label_4"));
    label_4->setStyleSheet("QLabel { color: #ffffff; font-size: 20px; }");
    label_4->setText(tr("Sort by:"));
    horizontalLayout_2->addWidget(label_4);

    customComboBox = new QPushButton(this);
    customComboBox->setObjectName(QString::fromUtf8("customComboBox"));
    customComboBox->setStyleSheet(
        "QPushButton {"
        "	color: #fefefe; background: rgba(196, 196, 196, 0.2);"
        "	border: none; border-radius: 5px;"
        "	padding: 8px 35px 8px 15px; min-width: 80px;"
        "	text-align: left; font-size: 20px;"
        "}"
    );
    customComboBox->setText(tr("Date"));
    horizontalLayout_2->addWidget(customComboBox);

    label_reversed = new QLabel(this);
    label_reversed->setObjectName(QString::fromUtf8("label_reversed"));
    label_reversed->setFixedSize(32, 32);
    label_reversed->setCursor(Qt::PointingHandCursor);
    label_reversed->setStyleSheet(
        "QLabel:hover { background-color: rgba(200, 200, 200, 0.7); }"
        "QLabel:pressed { background-color: rgba(100, 100, 100, 0.7); }"
    );
    label_reversed->setPixmap(QPixmap(":/resources/images/sort.svg"));
    label_reversed->setScaledContents(true);
    label_reversed->setToolTip(tr("Reverse Sort Order"));
    horizontalLayout_2->addWidget(label_reversed);

    horizontalLayout_2->addItem(new QSpacerItem(40, 20, QSizePolicy::Fixed, QSizePolicy::Minimum));
    verticalLayout_3->addLayout(horizontalLayout_2);

    verticalLayout_3->addItem(new QSpacerItem(20, 50, QSizePolicy::Minimum, QSizePolicy::Fixed));

    m_stackedWidget = new QStackedWidget(this);
    m_stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));

    QWidget* page = new QWidget();
    page->setObjectName(QString::fromUtf8("page"));
    auto* verticalLayout = new QVBoxLayout(page);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));

    scrollArea = new QScrollArea(page);
    scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
    scrollArea->setWidgetResizable(true);

    scrollAreaWidgetContents = new QWidget();
    scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
    scrollAreaWidgetContents->setGeometry(QRect(0, 0, 1003, 740)); // Example geometry

    scrollArea->setWidget(scrollAreaWidgetContents);
    verticalLayout->addWidget(scrollArea);
    m_stackedWidget->addWidget(page);

    QWidget* page_2 = new QWidget();
    page_2->setObjectName(QString::fromUtf8("page_2"));
    auto* verticalLayout_2 = new QVBoxLayout(page_2);
    verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));

    verticalLayout_2->addItem(new QSpacerItem(20, 54, QSizePolicy::Minimum, QSizePolicy::Expanding));

    auto* horizontalLayout_3 = new QHBoxLayout();
    horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
    horizontalLayout_3->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    auto* label_5 = new QLabel(page_2);
    label_5->setObjectName(QString::fromUtf8("label_5"));
    label_5->setFixedSize(200, 114);
    label_5->setPixmap(QPixmap(":/resources/images/noRecentFiles.png"));
    horizontalLayout_3->addWidget(label_5);

    horizontalLayout_3->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    verticalLayout_2->addLayout(horizontalLayout_3);

    auto* horizontalLayout_4 = new QHBoxLayout();
    horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
    horizontalLayout_4->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    auto* label_6 = new QLabel(page_2);
    label_6->setObjectName(QString::fromUtf8("label_6"));
    label_6->setStyleSheet("QLabel { color: #ffffff; font-size: 16px; font-weight: bold; }");
    label_6->setText(tr("No matching recent file records found."));
    horizontalLayout_4->addWidget(label_6);

    horizontalLayout_4->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    verticalLayout_2->addLayout(horizontalLayout_4);

    verticalLayout_2->addItem(new QSpacerItem(20, 28, QSizePolicy::Minimum, QSizePolicy::Expanding));
    m_stackedWidget->addWidget(page_2);

    verticalLayout_3->addWidget(m_stackedWidget);
    m_stackedWidget->setCurrentIndex(0);

    // Set object names for stylesheet targeting if necessary
    this->setObjectName("RecentFile");
}



void MxRecentFileWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    updateLayout();
}

bool MxRecentFileWidget::eventFilter(QObject* watched, QEvent* event)
{
    // Listen for mouse release events on ui->label_reversed
    if (watched == label_reversed && event->type() == QEvent::MouseButtonRelease) {
        reverseItems();
        updateLayout();
        return true;
    }
    return QWidget::eventFilter(watched, event);
}

void MxRecentFileWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    QLinearGradient gradient(0, 0, width(), height());
    gradient.setColorAt(0, QColor("#52B99E"));
    gradient.setColorAt(1, QColor("#24707C"));
    painter.fillRect(rect(), gradient);
    event->accept();
}

void MxRecentFileWidget::sortItemsByName(QList<MxRecentFileItem*>& items)
{
    std::sort(items.begin(), items.end(), [](MxRecentFileItem* a, MxRecentFileItem* b) {
        QFileInfo aInfo(a->getRecentFile().filePath);
        QFileInfo bInfo(b->getRecentFile().filePath);
        return aInfo.fileName().toLower() < bInfo.fileName().toLower();
        });
}

void MxRecentFileWidget::sortItemsByDate(QList<MxRecentFileItem*>& items)
{
    std::sort(items.begin(), items.end(), [](MxRecentFileItem* a, MxRecentFileItem* b) {
        return a->getRecentFile().lastOpened > b->getRecentFile().lastOpened;
        });
}

void MxRecentFileWidget::sortItemsByType(QList<MxRecentFileItem*>& items)
{
    std::sort(items.begin(), items.end(), [](MxRecentFileItem* a, MxRecentFileItem* b) {
        return a->getRecentFile().fileType < b->getRecentFile().fileType;
        });
}

void MxRecentFileWidget::sortItemsBySize(QList<MxRecentFileItem*>& items)
{
    std::sort(items.begin(), items.end(), [](MxRecentFileItem* a, MxRecentFileItem* b) {
        return a->getRecentFile().fileSize > b->getRecentFile().fileSize;
        });
}

void MxRecentFileWidget::updateItems()
{
    qDeleteAll(m_listRecentFileItems);
    m_listRecentFileItems.clear();

    QList<MxRecentFile> recentFiles = MxRecentFileManager::instance()->getRecentFiles();
    for (const MxRecentFile& recentFile : recentFiles) {
        MxRecentFileItem* item = new MxRecentFileItem(recentFile, scrollAreaWidgetContents);
        m_listRecentFileItems.append(item);
    }
}

void MxRecentFileWidget::sortItems(SortType sortType)
{
    switch (sortType) {
    case SortType::Name:
        sortItemsByName(m_listRecentFileItems);
        break;
    case SortType::Date:
        sortItemsByDate(m_listRecentFileItems);
        break;
    case SortType::Type:
        sortItemsByType(m_listRecentFileItems);
        break;
    case SortType::Size:
        sortItemsBySize(m_listRecentFileItems);
        break;
    default:
        break;
    }
    m_currentSortType = sortType;
    m_isReverse = false;
}

void MxRecentFileWidget::reverseItems()
{
    std::reverse(m_listRecentFileItems.begin(), m_listRecentFileItems.end());
    m_isReverse = !m_isReverse;
}

void MxRecentFileWidget::resortItems()
{
    sortItems(m_currentSortType);
    if (m_isReverse) {
        reverseItems();
    }
}

void MxRecentFileWidget::updateLayout()
{
    // Clear existing layout (but do not delete widgets)
    QLayoutItem* item;
    while ((item = m_pGridLayout->takeAt(0)) != nullptr) {
        delete item; // Delete the layout item, not the widget
    }

    const int imageWidth = 255;
    const int horizontalSpacing = 10;

    // Calculate how many items can fit per row
    int availableWidth = scrollArea->width() - m_pGridLayout->contentsMargins().left() - m_pGridLayout->contentsMargins().right();
    int itemsPerRow = qMax(1, (availableWidth + horizontalSpacing) / (imageWidth + horizontalSpacing));

    // List of items to be displayed
    QList<MxRecentFileItem*> validItems;
    for (int i = 0; i < m_listRecentFileItems.size(); ++i) {
        auto item = m_listRecentFileItems[i];
        if (item->contains(m_sSearchFilter)) {
            validItems.append(item);
        }
        else {
            item->hide();
        }
    }

    for (int i = 0; i < validItems.size(); ++i) {
        int row = i / itemsPerRow;
        int col = i % itemsPerRow;
        auto item = validItems[i];
        item->show();
        m_pGridLayout->addWidget(item, row, col);
    }

    if (m_pGridLayout->count() == 0) {
        m_stackedWidget->setCurrentIndex(1);
    }
    else {
        m_stackedWidget->setCurrentIndex(0);
    }
}

void MxRecentFileWidget::onSortItems(int index)
{
    SortType sortType = static_cast<SortType>(index);
    sortItems(sortType);
    updateLayout();
}

void MxRecentFileWidget::onSortItems(const QString& item)
{
    SortType sortType = SortType::Date;
    if (item == tr("Name")) {
        sortType = SortType::Name;
    }
    else if (item == tr("Date")) {
        sortType = SortType::Date;
    }
    else if (item == tr("Type")) {
        sortType = SortType::Type;
    }
    else if (item == tr("Size")) {
        sortType = SortType::Size;
    }
    sortItems(sortType);
    updateLayout();
}

void MxRecentFileWidget::onUpdateItems()
{
    updateItems();
    resortItems();
    updateLayout();
}

void MxRecentFileWidget::onUpdateLayout(const QString& filter)
{
    m_sSearchFilter = filter;
    updateLayout();
}