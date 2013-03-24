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
        PHEADER_DATE,
        PHEADER_TEMPLATE
    };

    enum ItemsTableHeader {
        IHEADER_NUMBER,
        IHEADER_DESCRIPTION,
        IHEADER_UNIT,
        IHEADER_PRICE,
        IHEADER_AMOUNT
    };

    void selectContract(Thing *contract);

private:
    void updateContractsList();
    void updateItemsList();
    MyTableWidgetItem *addContract(Contract *contract);
    MyTableWidgetItem *addItem(ContractItem *item);
    Contract *getCurrentContract();
    ContractItem *getCurrentItem();

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
