#ifndef PROPOSALS_H
#define PROPOSALS_H

#include "headers.h"
#include "core/project.h"
#include "templates.h"
#include "tools.h"

class Proposals : public QWidget
{
    Q_OBJECT

public:
    Proposals();

    enum ProposalsTableHeader {
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
        IHEADER_UNIT_PRICE,
        IHEADER_AMOUNT
    };

    void selectProposal(Thing *proposal);

private:
    void updateProposalsList();
    void updateItemsList();
    MyTableWidgetItem *addProposal(Thing *proposal);
    MyTableWidgetItem *addItem(QTableWidget *parent, Thing *item);
    Thing *getCurrentProposal();
    Thing *getCurrentItemList();
    Thing *getCurrentItem();
    QTableWidget *createItemsTable();

    Templates *m_templates;
    QTableWidget *m_proposalsTable;
    QTabWidget *m_itemListTabWidget;
    QWidget *m_proposalsBottomWidget;

private slots:
    void onAddProposalClicked();
    void onRemoveProposalClicked();
    void onAddItemClicked(int index = -1);
    void onRemoveItemClicked();
    void onRenameItemListClicked();

    void onProposalsCurrentCellChanged(int currentRow, int , int previousRow, int );
    void onProposalsCellDoubleClicked(int row, int column);
    void onProposalsCustomContextMenuRequested(QPoint pos);

    void onItemsCustomContextMenuRequested(QPoint pos);

    void onItemListAddClicked();
    void onItemListCloseRequested(int index);

    void onProjectLoad();

};

#endif // PROPOSALS_H
