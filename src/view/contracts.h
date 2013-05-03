#ifndef CONTRACTS_H
#define CONTRACTS_H

#include "headers.h"
#include "core/project.h"
#include "tools.h"
#include "itemlisttabwidget.h"

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
        PHEADER_START_DATE,
        PHEADER_END_DATE
    };

    void selectContract(Thing *contract);
    void updateContractsList();

private:
    MyTableWidgetItem *addContract(Thing *contract);
    Thing *getCurrentContract();

    QTableWidget *m_contractsTable;
    ItemListTabWidget *m_itemListTabWidget;

private slots:
    void onAddContractClicked();
    void onRemoveContractClicked();

    void onContractsCurrentCellChanged(int currentRow, int , int previousRow, int );
    void onContractsCellDoubleClicked(int row, int column);
    void onContractsCustomContextMenuRequested(QPoint pos);

    void onProjectLoad();
};

#endif // CONTRACTS_H
