#ifndef CONTRACTS_H
#define CONTRACTS_H

#include "headers.h"
#include "core/project.h"
#include "tools.h"

class Contracts : public QWidget
{
    Q_OBJECT

public:
    Contracts();

    enum ContractsTableHeader {
        PHEADER_STATE,
        PHEADER_REFERENCE,
        PHEADER_DESCRIPTION,
        PHEADER_CLIENT,
        PHEADER_DATE
    };

    enum ItemsTableHeader {
        IHEADER_NUMBER,
        IHEADER_DESCRIPTION,
        IHEADER_UNIT,
        IHEADER_UNIT_PRICE,
        IHEADER_AMOUNT
    };

    void selectContract(Thing *contract);
    void updateContractsList();

private:
    void updateItemsList();
    MyTableWidgetItem *addContract(Thing *contract);
    MyTableWidgetItem *addItem(Thing *item);
    Thing *getCurrentContract();
    Thing *getCurrentItem();

    QTableWidget *m_contractsTable;
    QTableWidget *m_itemsTable;
    QLabel *m_itemsLabel;

private slots:
    void onAddContractClicked();
    void onRemoveContractClicked();
    void onAddItemClicked(int index = -1);
    void onRemoveItemClicked();

    void onContractsCurrentCellChanged(int currentRow, int , int previousRow, int );
    void onContractsCellDoubleClicked(int row, int column);
    void onContractsCustomContextMenuRequested(QPoint pos);

    void onItemsCustomContextMenuRequested(QPoint pos);

    void onProjectLoad();

};

#endif // CONTRACTS_H
