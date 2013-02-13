#include "proposals.h"
#include "tools.h"
#include "project.h"
#include <QtGui>

Proposals::Proposals()
{
    QGridLayout *gLayout;
    int row;

    QPushButton *addProposal = new QPushButton("+");
    connect(addProposal, SIGNAL(clicked()), this, SLOT(onAddProposalClicked()));

    QPushButton *removeProposal = new QPushButton("-");
    connect(removeProposal, SIGNAL(clicked()), this, SLOT(onRemoveProposalClicked()));

    QPushButton *addItem = new QPushButton("+");
    connect(addItem, SIGNAL(clicked()), this, SLOT(onAddItemClicked()));

    QPushButton *removeItem = new QPushButton("-");
    connect(removeItem, SIGNAL(clicked()), this, SLOT(onRemoveItemClicked()));

    m_proposalsTable = new QTableWidget;
    m_proposalsTable->setColumnCount(4);
    m_proposalsTable->setHorizontalHeaderLabels(QString("%1,%2,%3,%4").arg(tr("Reference")).arg(tr("Description")).arg("Client").arg(tr("Date")).split(","));
    m_proposalsTable->verticalHeader()->hide();
    m_proposalsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_proposalsTable->setSortingEnabled(true);
    connect(m_proposalsTable, SIGNAL(currentCellChanged(int,int,int,int)), this, SLOT(onProposalsCurrentCellChanged(int,int,int,int)));
    connect(m_proposalsTable, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(onProposalsCellDoubleClicked(int, int)));

    m_itemsTable = new QTableWidget;
    m_itemsTable->setColumnCount(5);
    m_itemsTable->setHorizontalHeaderLabels(QString("%1,%2,%3,%4,%5").arg(tr("#")).arg(tr("Description")).arg("Unit").arg(tr("Price")).arg("Amount").split(","));
    m_itemsTable->verticalHeader()->hide();
    m_itemsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_itemsTable->setSortingEnabled(true);

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
    m_proposalsTable->setRowCount(0);
    for(QVector<Proposal*>::const_iterator it = proposals.constBegin(), end = proposals.constEnd(); it != end; ++it) {
        addProposal(*it);
    }
    m_proposalsTable->sortItems(PHEADER_DATE, Qt::DescendingOrder);
    m_proposalsTable->resizeColumnsToContents();

    if(m_proposalsTable->rowCount() == rowCount)
        m_proposalsTable->selectRow(currentRow);
    else if(m_proposalsTable->rowCount() > 0)
        m_proposalsTable->selectRow(0);

    updateItemsList();
}

void Proposals::updateItemsList()
{
    Proposal *proposal = getCurrentProposal();
    if(!proposal)
        return;

    const QVector<ProposalItem*>& items = proposal->getItems();

    int currentRow = m_itemsTable->currentRow();
    int rowCount = m_itemsTable->rowCount();

    m_itemsTable->clearContents();
    m_itemsTable->setRowCount(0);
    for(int i = 0; i < items.size(); ++i)
        addItem(items[i]);
    m_itemsTable->sortItems(IHEADER_NUMBER);
    m_itemsTable->resizeColumnsToContents();

    if(m_itemsTable->rowCount() == rowCount)
        m_itemsTable->selectRow(currentRow);
    else if(m_itemsTable->rowCount() > 0)
        m_itemsTable->selectRow(0);
}

void Proposals::addProposal(Proposal *proposal)
{
    QLocale locale;

    MyTableWidgetItem *reference = new MyTableWidgetItem();
    reference->setData(Qt::DisplayRole, proposal->getReference());
    reference->setFlags(reference->flags() & ~Qt::ItemIsEditable);

    MyTableWidgetItem *description = new MyTableWidgetItem();
    description->setData(Qt::DisplayRole, proposal->getDescription());
    description->setFlags(description->flags() & ~Qt::ItemIsEditable);

    MyTableWidgetItem *date = new MyTableWidgetItem();
    date->setData(Qt::DisplayRole, locale.toString(proposal->getDate(), "dd/MM/yyyy (dddd)"));
    date->setData(Qt::UserRole, proposal->getDate());
    date->setFlags(date->flags() & ~Qt::ItemIsEditable);

    m_proposalsTable->setRowCount(m_proposalsTable->rowCount()+1);
    m_proposalsTable->setItem(m_proposalsTable->rowCount()-1, PHEADER_REFERENCE, reference);
    m_proposalsTable->setItem(m_proposalsTable->rowCount()-1, PHEADER_DESCRIPTION, description);
    m_proposalsTable->setItem(m_proposalsTable->rowCount()-1, PHEADER_DATE, date);
}

void Proposals::addItem(ProposalItem *item)
{
    int id = item->getId();

    MyTableWidgetItem *number = new MyTableWidgetItem();
    number->setData(Qt::DisplayRole, id+1);
    number->setData(Qt::UserRole, id+1);
    number->setFlags(number->flags() & ~Qt::ItemIsEditable);

    MyTableWidgetItem *description = new MyTableWidgetItem();
    description->setData(Qt::DisplayRole, item->getDescription());
    description->setFlags(description->flags() & ~Qt::ItemIsEditable);

    MyTableWidgetItem *unit = new MyTableWidgetItem();
    unit->setData(Qt::DisplayRole, item->getUnit());
    unit->setFlags(unit->flags() & ~Qt::ItemIsEditable);

    MyTableWidgetItem *price = new MyTableWidgetItem();
    price->setData(Qt::DisplayRole, item->getPrice());
    price->setData(Qt::UserRole, item->getPrice());
    price->setFlags(price->flags() & ~Qt::ItemIsEditable);

    MyTableWidgetItem *amount = new MyTableWidgetItem();
    amount->setData(Qt::DisplayRole, item->getAmount());
    amount->setData(Qt::UserRole, item->getAmount());
    amount->setFlags(amount->flags() & ~Qt::ItemIsEditable);

    m_itemsTable->setRowCount(m_itemsTable->rowCount()+1);
    m_itemsTable->setItem(m_itemsTable->rowCount()-1, IHEADER_NUMBER, number);
    m_itemsTable->setItem(m_itemsTable->rowCount()-1, IHEADER_DESCRIPTION, description);
    m_itemsTable->setItem(m_itemsTable->rowCount()-1, IHEADER_UNIT, unit);
    m_itemsTable->setItem(m_itemsTable->rowCount()-1, IHEADER_PRICE, price);
    m_itemsTable->setItem(m_itemsTable->rowCount()-1, IHEADER_AMOUNT, amount);
}

Proposal *Proposals::getCurrentProposal()
{
    int currentRow = m_proposalsTable->currentRow();
    if(currentRow == -1)
        return NULL;

    QTableWidgetItem *reference = m_proposalsTable->item(currentRow, PHEADER_REFERENCE);
    return g_project->getProposal(reference->text());
}

void Proposals::onAddProposalClicked()
{
    QDialog dialog;
    dialog.setWindowTitle(tr("Add Proposal"));

    QGridLayout *layout = new QGridLayout();
    dialog.setLayout(layout);

    int row = 0;
    layout->addWidget(new QLabel(tr("Reference:")), row, 0);
    QLineEdit *referenceLineEdit = new QLineEdit;
    layout->addWidget(referenceLineEdit, row++, 1);

    layout->addWidget(new QLabel(tr("Description:")), row, 0);
    QLineEdit *descriptionLineEdit = new QLineEdit;
    layout->addWidget(descriptionLineEdit, row++, 1);

    layout->addWidget(new QLabel(tr("Date:")), row, 0);
    QCalendarWidget *calendar = new QCalendarWidget();
    calendar->setGridVisible(true);
    layout->addWidget(calendar, row++, 1);

    layout->addLayout(Tools::createOkCancel(&dialog), row, 0, 1, 2);

    if(dialog.exec() == QDialog::Accepted) {
        Proposal *proposal = new Proposal;
        proposal->setReference(referenceLineEdit->text());
        proposal->setDescription(descriptionLineEdit->text());
        proposal->setDate(calendar->selectedDate());
        if(g_project->addProposal(proposal)) {
            addProposal(proposal);
            m_proposalsTable->resizeColumnsToContents();
        }
        else
            qCritical() << tr("A proposal with this reference already exists.");
    }
}

void Proposals::onRemoveProposalClicked()
{
    int currentRow = m_proposalsTable->currentRow();
    if(currentRow == -1)
        return;

    QTableWidgetItem *reference = m_proposalsTable->item(currentRow, PHEADER_REFERENCE);

    if(Tools::requestYesNoFromUser(tr("Remove Proposal"), tr("Do you really want to remove proposal %1?").arg(reference->text())) == "No")
        return;

    g_project->removeProposal(reference->text());
    m_proposalsTable->removeRow(currentRow);
}

void Proposals::onAddItemClicked()
{
    Proposal *proposal = getCurrentProposal();
    if(!proposal)
        return;

    QDialog dialog;
    dialog.setWindowTitle(tr("Add Item"));

    QGridLayout *layout = new QGridLayout();
    dialog.setLayout(layout);

    int row = 0;
    layout->addWidget(new QLabel(tr("Description:")), row, 0);
    QLineEdit *descriptionLineEdit = new QLineEdit;
    layout->addWidget(descriptionLineEdit, row++, 1);

    layout->addWidget(new QLabel(tr("Unit:")), row, 0);
    QLineEdit *unitLineEdit = new QLineEdit;
    layout->addWidget(unitLineEdit, row++, 1);

    layout->addWidget(new QLabel(tr("Price:")), row, 0);
    QLineEdit *priceLineEdit = new QLineEdit;
    layout->addWidget(priceLineEdit, row++, 1);

    layout->addWidget(new QLabel(tr("Amount:")), row, 0);
    QLineEdit *amountLineEdit = new QLineEdit;
    layout->addWidget(amountLineEdit, row++, 1);

    layout->addLayout(Tools::createOkCancel(&dialog), row, 0, 1, 2);

    if(dialog.exec() == QDialog::Accepted) {
        ProposalItem *item = new ProposalItem;
        item->setDescription(descriptionLineEdit->text());
        item->setUnit(unitLineEdit->text());
        item->setPrice(priceLineEdit->text().toFloat());
        item->setAmount(amountLineEdit->text().toInt());
        proposal->addItem(item);

        addItem(item);
        m_itemsTable->resizeColumnsToContents();
    }
}

void Proposals::onRemoveItemClicked()
{
    int currentRow = m_itemsTable->currentRow();
    if(currentRow == -1)
        return;

    if(Tools::requestYesNoFromUser(tr("Remove Item"), tr("Do you really want to remove item #%1?").arg(currentRow+1)) == "No")
        return;

    Proposal *proposal = getCurrentProposal();
    proposal->removeItem(currentRow);

    m_itemsTable->removeRow(currentRow);
}

void Proposals::onProposalsCurrentCellChanged(int currentRow, int, int previousRow, int)
{
    if(currentRow != previousRow)
        updateItemsList();
}

void Proposals::onProposalsCellDoubleClicked(int row, int column)
{
    QTableWidgetItem *reference = m_proposalsTable->item(row, PHEADER_REFERENCE);
    Proposal *proposal = g_project->getProposal(reference->text());

    QDialog dialog;
    dialog.setWindowTitle(tr("Edit Proposal %1").arg(reference->text()));

    QGridLayout *layout = new QGridLayout();
    dialog.setLayout(layout);

    QLineEdit *nReference = NULL;
    QLineEdit *nDescription = NULL;
    QCalendarWidget *nDate = NULL;

    int lrow = 0;
    if(column == PHEADER_REFERENCE) {
        nReference = new QLineEdit(proposal->getReference());
        layout->addWidget(new QLabel(tr("Reference:")), lrow, 0);
        layout->addWidget(nReference, lrow++, 1);
    }
    else if(column == PHEADER_DESCRIPTION) {
        nDescription = new QLineEdit(proposal->getDescription());
        layout->addWidget(new QLabel(tr("Description:")), lrow, 0);
        layout->addWidget(nDescription, lrow++, 1);
    }
    else if(column == PHEADER_DATE) {
        nDate = new QCalendarWidget();
        nDate->setGridVisible(true);
        nDate->setSelectedDate(proposal->getDate());
        layout->addWidget(new QLabel(tr("Date:")), lrow, 0);
        layout->addWidget(nDate, lrow++, 1);
    }

    layout->addLayout(Tools::createOkCancel(&dialog), lrow, 0, 1, 2);

    if(dialog.exec() == QDialog::Accepted) {
        if(nReference) {
            if(g_project->getProposal(nReference->text()))
                qCritical() << "A proposal with this name already exists.";
            else
                proposal->setReference(nReference->text());
        }
        else if(nDescription) {
            proposal->setDescription(nDescription->text());
        }
        else if(nDate) {
            proposal->setDate(nDate->selectedDate());
        }
        updateProposalsList();
    }
}

void Proposals::onProjectLoad()
{
    updateProposalsList();
}
