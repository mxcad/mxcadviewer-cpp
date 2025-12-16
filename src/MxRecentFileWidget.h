/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <QWidget>
#include "MxRecentFile.h"
#include <QList>

QT_BEGIN_NAMESPACE
class QGridLayout;
class QStackedWidget;
class QScrollArea;
class QLineEdit;
class QPushButton;
class QLabel;
QT_END_NAMESPACE

class MxRecentFileItem;

class MxRecentFileWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MxRecentFileWidget(QWidget* parent = nullptr);
    ~MxRecentFileWidget() override;

protected:
    void resizeEvent(QResizeEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    void setupUi();

    QStackedWidget* m_stackedWidget;
    QScrollArea* scrollArea;
    QWidget* scrollAreaWidgetContents;
    QLineEdit* lineEdit_search;
    QPushButton* customComboBox;
    QLabel* label_reversed;


    QGridLayout* m_pGridLayout;
    QList<MxRecentFileItem*> m_listRecentFileItems;

public:
    enum class SortType { Date, Name, Type, Size };

private:

    SortType m_currentSortType = SortType::Date;

    bool m_isReverse = false;

    QString m_sSearchFilter = "";

private:

    void sortItemsByName(QList<MxRecentFileItem*>& items);

    void sortItemsByDate(QList<MxRecentFileItem*>& items);

    void sortItemsByType(QList<MxRecentFileItem*>& items);

    void sortItemsBySize(QList<MxRecentFileItem*>& items);

private:
    void updateItems();
    void sortItems(SortType sortType);
    void reverseItems();
    void resortItems();
    void updateLayout();

private slots:
    void onSortItems(int index);
    void onSortItems(const QString& item);
    void onUpdateItems();
    void onUpdateLayout(const QString& filter);
};