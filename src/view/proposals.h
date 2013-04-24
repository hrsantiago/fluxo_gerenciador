#ifndef PROPOSALS_H
#define PROPOSALS_H

#include "headers.h"
#include "core/project.h"
#include "templates.h"
#include "tools.h"
#include "itemlisttabwidget.h"

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

    void selectProposal(Thing *proposal);

private:
    void updateProposalsList();
    MyTableWidgetItem *addProposal(Thing *proposal);
    Thing *getCurrentProposal();

    Templates *m_templates;
    QTableWidget *m_proposalsTable;
    ItemListTabWidget *m_itemListTabWidget;

private slots:
    void onAddProposalClicked();
    void onRemoveProposalClicked();

    void onProposalsCurrentCellChanged(int currentRow, int , int previousRow, int );
    void onProposalsCellDoubleClicked(int row, int column);
    void onProposalsCustomContextMenuRequested(QPoint pos);

    void onProjectLoad();
};

#endif // PROPOSALS_H
