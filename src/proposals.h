#ifndef PROPOSALS_H
#define PROPOSALS_H

#include <QtGui>

class Proposals : public QWidget
{
    Q_OBJECT

public:
    Proposals();

private:
    void updateProposalsList();
    void updateItemsList();

    QTableWidget *m_proposalsTable;
    QTableWidget *m_itemsTable;

private slots:
    void addProposal();
    void removeProposal();
    void addItem();
    void removeItem();

    void onProjectLoad();

};

#endif // PROPOSALS_H
