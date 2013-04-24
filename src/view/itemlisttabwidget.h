#ifndef ITEMLIST_H
#define ITEMLIST_H

#include "headers.h"
#include "tools.h"
#include "core/project.h"

class ItemListTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    ItemListTabWidget(QWidget *parent = 0);

    enum ItemsTableHeader {
        IHEADER_NUMBER,
        IHEADER_DESCRIPTION,
        IHEADER_UNIT,
        IHEADER_UNIT_PRICE,
        IHEADER_AMOUNT
    };

    void setParentThing(Thing *thing) { m_parentThing = thing; }

    void updateItemsList();
    Thing *getCurrentItemList();
    Thing *getCurrentItem();

private:
    QTableWidget *createItemsTable();
    MyTableWidgetItem *addItem(QTableWidget *parent, Thing *item);

    Thing *m_parentThing;
    
private slots:
    void onItemListAddClicked();
    void onItemListCloseRequested(int index);
    void onRenameItemListClicked();

    void onAddItemClicked(int index = -1);
    void onRemoveItemClicked();
    void onItemsCustomContextMenuRequested(QPoint pos);

};

#endif // ITEMLIST_H
