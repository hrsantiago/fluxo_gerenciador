#ifndef PROPOSALS_H
#define PROPOSALS_H

#include <QtGui>

class Proposals : public QWidget
{
    Q_OBJECT

public:
    Proposals();

private:
    QListWidget *m_proposalsList;
    QTableWidget *m_itemsTable;

private slots:
    void addProposal();
    void removeProposal();
    void addItem();
    void removeItem();

};

#endif // PROPOSALS_H
