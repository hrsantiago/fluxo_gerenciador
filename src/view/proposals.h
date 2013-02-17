#ifndef PROPOSALS_H
#define PROPOSALS_H

#include <QtGui>
#include "core/project.h"
#include "templates.h"

class MyTableWidgetItem : public QTableWidgetItem
{
public:
    MyTableWidgetItem(int type = Type) : QTableWidgetItem(type) {}
    ~MyTableWidgetItem () {}

    bool operator<(const QTableWidgetItem& other) const
    {
        Q_ASSERT(tableWidget());
        if(data(Qt::UserRole).type() == QVariant::Int && other.data(Qt::UserRole).type() == QVariant::Int)
            return data(Qt::UserRole).toInt() < other.data(Qt::UserRole).toInt();
        else if(data(Qt::UserRole).type() == QVariant::Double && other.data(Qt::UserRole).type() == QVariant::Double)
            return data(Qt::UserRole).toDouble() < other.data(Qt::UserRole).toDouble();
        else if(data(Qt::UserRole).type() == QVariant::Date && other.data(Qt::UserRole).type() == QVariant::Date)
            return data(Qt::UserRole).toDate() < other.data(Qt::UserRole).toDate();

        return data(Qt::DisplayRole).toString().toLower() < other.data(Qt::DisplayRole).toString().toLower();
    }
};

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
        IHEADER_PRICE,
        IHEADER_AMOUNT
    };

private:
    void updateProposalsList();
    void updateItemsList();
    MyTableWidgetItem *addProposal(Proposal *proposal);
    MyTableWidgetItem *addItem(ProposalItem *item);
    Proposal *getCurrentProposal();

    Templates *m_templates;
    QTableWidget *m_proposalsTable;
    QTableWidget *m_itemsTable;
    QLabel *m_itemsLabel;

private slots:
    void onAddProposalClicked();
    void onRemoveProposalClicked();
    void onAddItemClicked();
    void onRemoveItemClicked();

    void onProposalsCurrentCellChanged(int currentRow, int , int previousRow, int );
    void onProposalsCellDoubleClicked(int row, int column);
    void onProposalsCustomContextMenuRequested(QPoint pos);

    void onItemsCustomContextMenuRequested(QPoint pos);

    void onProjectLoad();

};

#endif // PROPOSALS_H