#include "proposals.h"
#include "tools.h"
#include "project.h"
#include <QtGui>

Proposals::Proposals()
{
    QGridLayout *gLayout;
    int row;

    QPushButton *addProposal = new QPushButton("+");
    connect(addProposal, SIGNAL(clicked()), this, SLOT(addProposal()));

    QPushButton *removeProposal = new QPushButton("-");
    connect(removeProposal, SIGNAL(clicked()), this, SLOT(removeProposal()));

    QPushButton *addItem = new QPushButton("+");
    connect(addItem, SIGNAL(clicked()), this, SLOT(addItem()));

    QPushButton *removeItem = new QPushButton("-");
    connect(removeItem, SIGNAL(clicked()), this, SLOT(removeItem()));

    m_proposalsTable = new QTableWidget;
    m_proposalsTable->setColumnCount(3);
    m_proposalsTable->setHorizontalHeaderLabels(QString("%1,%2,%3").arg(tr("Description")).arg(tr("Start")).arg(tr("End")).split(","));
    m_proposalsTable->verticalHeader()->hide();
    m_proposalsTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    m_itemsTable = new QTableWidget;
    m_itemsTable->setColumnCount(4);
    m_itemsTable->setHorizontalHeaderLabels(QString("%1,%2,%3,%4").arg(tr("Description")).arg(tr("Value")).arg(tr("Start")).arg(tr("End")).split(","));
    m_itemsTable->verticalHeader()->hide();

    // Top
    QWidget *top = new QWidget;
    gLayout = new QGridLayout();
    top->setLayout(gLayout);

    row = 0;
    gLayout->addWidget(new QLabel(tr("List of proposals:")), row++, 0, 1, 2);
    gLayout->addWidget(m_proposalsTable, row, 0, 3, 1);
    gLayout->addWidget(addProposal, row++, 1);
    gLayout->addWidget(removeProposal, row++, 1);
    gLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), row++, 1);

    // Bottom
    QWidget *bottom = new QWidget;
    gLayout = new QGridLayout();
    bottom->setLayout(gLayout);

    row = 0;
    gLayout->addWidget(new QLabel(tr("List of items:")), row++, 0, 1, 2);
    gLayout->addWidget(m_itemsTable, row, 0, 3, 1);
    gLayout->addWidget(addItem, row++, 1);
    gLayout->addWidget(removeItem, row++, 1);
    gLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), row++, 1);

    // Splitter
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    QSplitter *vSplitter = new QSplitter(Qt::Vertical);
    layout->addWidget(vSplitter);

    vSplitter->addWidget(top);
    vSplitter->addWidget(bottom);
    vSplitter->setStretchFactor(0, 30);
    vSplitter->setStretchFactor(1, 70);
}

void Proposals::updateProposalsList()
{
    const QVector<Proposal*>& proposals = g_project->getProposals();

    int currentRow = m_proposalsTable->currentRow();
    int rowCount = m_proposalsTable->rowCount();

    m_proposalsTable->clearContents();
    for(QVector<Proposal*>::const_iterator it = proposals.constBegin(), end = proposals.constEnd(); it != end; ++it) {
        m_proposalsTable->setRowCount(m_proposalsTable->rowCount()+1);
        m_proposalsTable->setCellWidget(m_proposalsTable->rowCount()-1, 0, new QLabel((*it)->description));
        m_proposalsTable->setCellWidget(m_proposalsTable->rowCount()-1, 1, new QLabel((*it)->start.toString("dd/MM/yyyy (dddd)")));
        m_proposalsTable->setCellWidget(m_proposalsTable->rowCount()-1, 2, new QLabel((*it)->end.toString("dd/MM/yyyy (dddd)")));
    }
    m_proposalsTable->sortItems(0);
    m_proposalsTable->resizeColumnsToContents();

    if(m_proposalsTable->rowCount() == rowCount)
        m_proposalsTable->selectRow(currentRow);
    else if(m_proposalsTable->rowCount() > 0)
        m_proposalsTable->selectRow(0);
}

void Proposals::updateItemsList()
{

}

void Proposals::addProposal()
{
    QDialog dialog;
    dialog.setWindowTitle(tr("Add Proposal"));

    QGridLayout *layout = new QGridLayout();
    dialog.setLayout(layout);

    int row = 0;
    layout->addWidget(new QLabel(tr("Description:")), row, 0, 1, 2);
    QLineEdit *descriptionLineEdit = new QLineEdit;
    layout->addWidget(descriptionLineEdit, row++, 1, 1, 2);

    layout->addWidget(new QLabel(tr("Start:")), row, 0);
    layout->addWidget(new QLabel(tr("Finish:")), row++, 1);
    QCalendarWidget *startCalendar = new QCalendarWidget();
    startCalendar->setGridVisible(true);
    QCalendarWidget *endCalendar = new QCalendarWidget();
    endCalendar->setGridVisible(true);

    layout->addWidget(startCalendar, row, 0);
    layout->addWidget(endCalendar, row++, 1);

    QHBoxLayout *hLayout = new QHBoxLayout();
    layout->addLayout(hLayout, row, 0, 1, 2);

    QPushButton *ok = new QPushButton(tr("Ok"));
    connect(ok, SIGNAL(clicked()), &dialog, SLOT(accept()));
    hLayout->addWidget(ok);
    QPushButton *cancel = new QPushButton(tr("Cancel"));
    connect(cancel, SIGNAL(clicked()), &dialog, SLOT(reject()));
    hLayout->addWidget(cancel);

    if(dialog.exec() == QDialog::Accepted) {
        Proposal *proposal = new Proposal;
        proposal->description = descriptionLineEdit->text();
        proposal->start = startCalendar->selectedDate();
        proposal->end = endCalendar->selectedDate();
        g_project->addProposal(proposal);

        m_proposalsTable->setRowCount(m_proposalsTable->rowCount()+1);
        m_proposalsTable->setCellWidget(m_proposalsTable->rowCount()-1, 0, new QLabel(proposal->description));
        m_proposalsTable->setCellWidget(m_proposalsTable->rowCount()-1, 1, new QLabel(proposal->start.toString("dd/MM/yyyy (dddd)")));
        m_proposalsTable->setCellWidget(m_proposalsTable->rowCount()-1, 2, new QLabel(proposal->end.toString("dd/MM/yyyy (dddd)")));
        m_proposalsTable->resizeColumnsToContents();
    }
}

void Proposals::removeProposal()
{
    if(m_proposalsTable->currentItem()) {
        if(Tools::requestYesNoFromUser(tr("Remove Proposal"), tr("Do you really want to remove this proposal?")) == "No")
            return;

        m_proposalsTable->removeRow(m_proposalsTable->currentRow());

        updateProposalsList();
    }
}

void Proposals::addItem()
{

}

void Proposals::removeItem()
{
    if(m_proposalsTable->currentItem()) {
        if(Tools::requestYesNoFromUser(tr("Remove Item"), tr("Do you really want to remove this item?")) == "No")
            return;

        m_itemsTable->removeRow(m_itemsTable->currentRow());

        updateItemsList();
    }
}

void Proposals::onProjectLoad()
{
    updateProposalsList();
}
