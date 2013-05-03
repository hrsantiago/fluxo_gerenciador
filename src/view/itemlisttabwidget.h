#ifndef ITEMLIST_H
#define ITEMLIST_H

#include "headers.h"
#include "tools.h"
#include "core/project.h"

struct ExtraItem
{
    ExtraItem(const QString& hName, const QString& tName) {
        headerName = hName;
        thingName = tName;
    }

    QString headerName;
    QString thingName;
};

class ItemListTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    ItemListTabWidget(const QVector<ExtraItem*>& extraItems = QVector<ExtraItem*>(), QWidget *parent = 0);

    enum ItemsTableHeader {
        IHEADER_NUMBER,
        IHEADER_DESCRIPTION,
        IHEADER_UNIT,
        IHEADER_UNIT_PRICE,
        IHEADER_AMOUNT,
        IHEADER_LAST
    };

    void setParentThing(Thing *thing) { m_parentThing = thing; }

    void updateItemsList();
    void updateItemListIds(QTableWidget *tableWidget);
    Thing *getCurrentItemList();
    Thing *getCurrentItem();
    QString getTableHTML(Thing *itemList);

private:
    QTableWidget *createItemsTable();
    MyTableWidgetItem *addItem(QTableWidget *parent, Thing *item);

    Thing *m_parentThing;
    QVector<ExtraItem*> m_extraItems;
    
private slots:
    void onItemListAddClicked();
    void onItemListCloseRequested(int index);
    void onItemListTabMoved(int from, int to);
    void onRenameItemListClicked();

    void onAddItemClicked(int index = -1);
    void onRemoveItemClicked();
    void onItemsCellDoubleClicked(int row, int column);
    void onItemsCustomContextMenuRequested(QPoint pos);

};

#endif // ITEMLIST_H
