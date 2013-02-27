#include "proposals.h"
#include "templates.h"
#include "tools.h"
#include "core/project.h"
#include "const.h"

Proposals::Proposals()
{
    // Tab
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    QTabWidget *tabWidget = new QTabWidget();
    layout->addWidget(tabWidget);

    // Generic
    QGridLayout *gLayout;
    int row;

    // Top
    m_proposalsTable = new QTableWidget;
    m_proposalsTable->setColumnCount(6);
    m_proposalsTable->setHorizontalHeaderLabels(QString("%1,%2,%3,%4,%5,%6").arg("*").arg(tr("Reference")).arg(tr("Description")).arg(tr("Client")).arg(tr("Date")).arg(tr("Template")).split(","));
    m_proposalsTable->verticalHeader()->hide();
    m_proposalsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_proposalsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_proposalsTable->setSortingEnabled(true);
    m_proposalsTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_proposalsTable, SIGNAL(currentCellChanged(int,int,int,int)), this, SLOT(onProposalsCurrentCellChanged(int,int,int,int)));
    connect(m_proposalsTable, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(onProposalsCellDoubleClicked(int, int)));
    connect(m_proposalsTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onProposalsCustomContextMenuRequested(QPoint)));

    QWidget *top = new QWidget;
    gLayout = new QGridLayout();
    top->setLayout(gLayout);

    row = 0;
    gLayout->addWidget(new QLabel(tr("List of proposals:")), row++, 0);
    gLayout->addWidget(m_proposalsTable, row, 0);

    // Bottom
    m_itemsTable = new QTableWidget;
    m_itemsTable->setColumnCount(5);
    m_itemsTable->setHorizontalHeaderLabels(QString("%1,%2,%3,%4,%5").arg("#").arg(tr("Description")).arg(tr("Unit")).arg(tr("Price")).arg(tr("Amount")).split(","));
    m_itemsTable->verticalHeader()->hide();
    m_itemsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_itemsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_itemsTable->setSortingEnabled(true);
    m_itemsTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_itemsTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onItemsCustomContextMenuRequested(QPoint)));

    m_itemsLabel = new QLabel(tr("List of items of proposal '%1':").arg(""));

    QWidget *bottom = new QWidget;
    gLayout = new QGridLayout();
    bottom->setLayout(gLayout);

    row = 0;
    gLayout->addWidget(m_itemsLabel, row++, 0);
    gLayout->addWidget(m_itemsTable, row, 0);

    // Splitter
    QSplitter *vSplitter = new QSplitter(Qt::Vertical);
    tabWidget->addTab(vSplitter, tr("Proposals"));

    vSplitter->addWidget(top);
    vSplitter->addWidget(bottom);
    vSplitter->setStretchFactor(0, 30);
    vSplitter->setStretchFactor(1, 70);


    // Templates
    m_templates = new Templates();
    tabWidget->addTab(m_templates, tr("Templates"));

    // Statistics
    QWidget *statistics = new QWidget();
    tabWidget->addTab(statistics, tr("Statistics"));

    connect(g_project, SIGNAL(projectLoad()), this, SLOT(onProjectLoad()));
}

void Proposals::updateProposalsList()
{
    QString reference;
    QTableWidgetItem *referenceItem = m_proposalsTable->item(m_proposalsTable->currentRow(), PHEADER_REFERENCE);
    if(referenceItem)
        reference = referenceItem->text();

    MyTableWidgetItem *currentItem = NULL;

    m_proposalsTable->setSortingEnabled(false);
    m_proposalsTable->clearContents();
    m_proposalsTable->setRowCount(0);

    const QVector<Proposal*>& proposals = g_project->getProposals();
    for(QVector<Proposal*>::const_iterator it = proposals.constBegin(), end = proposals.constEnd(); it != end; ++it) {
        MyTableWidgetItem *item = addProposal(*it);
        if(reference == (*it)->getString("reference"))
            currentItem = item;
    }

    m_proposalsTable->setSortingEnabled(true);
    m_proposalsTable->resizeColumnsToContents();

    if(currentItem)
        m_proposalsTable->selectRow(m_proposalsTable->row(currentItem));
    else if(m_proposalsTable->rowCount() > 0)
        m_proposalsTable->selectRow(0);

    updateItemsList();
}

void Proposals::updateItemsList()
{
    ProposalItem *item = NULL;
    QTableWidgetItem *numberItem = m_itemsTable->item(m_itemsTable->currentRow(), IHEADER_NUMBER);
    if(numberItem)
        item = numberItem->data(Qt::UserRole).value<ProposalItem*>();

    m_itemsTable->setSortingEnabled(false);
    m_itemsTable->clearContents();
    m_itemsTable->setRowCount(0);

    Proposal *proposal = getCurrentProposal();
    if(!proposal) {
        m_itemsTable->setSortingEnabled(true);
        m_itemsTable->resizeColumnsToContents();
        return;
    }

    m_itemsLabel->setText(tr("List of items of proposal '%1':").arg(proposal->getString("reference")));

    MyTableWidgetItem *currentItem = NULL;

    const QVector<ProposalItem*>& items = proposal->getItems();
    for(int i = 0; i < items.size(); ++i) {
        MyTableWidgetItem *tableItem = addItem(items[i]);
        if(item == items[i])
            currentItem = tableItem;
    }

    m_itemsTable->setSortingEnabled(true);
    m_itemsTable->resizeColumnsToContents();

    if(currentItem)
        m_itemsTable->selectRow(m_itemsTable->row(currentItem));
    else if(m_itemsTable->rowCount() > 0)
        m_itemsTable->selectRow(0);
}

MyTableWidgetItem *Proposals::addProposal(Proposal *proposal)
{
    QLocale locale;

    MyTableWidgetItem *state = new MyTableWidgetItem();
    Proposal::State stateValue = (Proposal::State)proposal->getInt("state");
    if(stateValue == Proposal::STATE_PENDING) {
        state->setIcon(QIcon("resources/images/pending.png"));
        state->setToolTip(tr("Pending"));
    }
    else if(stateValue == Proposal::STATE_SENT) {
        state->setIcon(QIcon("resources/images/sent.png"));
        state->setToolTip(tr("Sent"));
    }
    else if(stateValue == Proposal::STATE_ACCEPTED) {
        state->setIcon(QIcon("resources/images/accepted.png"));
        state->setToolTip(tr("Accepted"));
    }
    else if(stateValue == Proposal::STATE_DECLINED) {
        state->setIcon(QIcon("resources/images/declined.png"));
        state->setToolTip(tr("Declined"));
    }
    state->setData(Qt::UserRole, proposal->getInt("state"));
    state->setFlags(state->flags() & ~Qt::ItemIsEditable);

    MyTableWidgetItem *reference = new MyTableWidgetItem();
    reference->setData(Qt::DisplayRole, proposal->getString("reference"));
    reference->setData(Qt::UserRole, QVariant::fromValue(proposal));
    reference->setFlags(reference->flags() & ~Qt::ItemIsEditable);

    MyTableWidgetItem *description = new MyTableWidgetItem();
    description->setData(Qt::DisplayRole, proposal->getString("description"));
    description->setFlags(description->flags() & ~Qt::ItemIsEditable);

    MyTableWidgetItem *client = new MyTableWidgetItem();
    client->setData(Qt::DisplayRole, proposal->getString("client"));
    client->setFlags(client->flags() & ~Qt::ItemIsEditable);

    MyTableWidgetItem *date = new MyTableWidgetItem();
    date->setData(Qt::DisplayRole, locale.toString(proposal->getDate("date"), "dd/MM/yyyy (dddd)"));
    date->setData(Qt::UserRole, proposal->getDate("date"));
    date->setFlags(date->flags() & ~Qt::ItemIsEditable);

    MyTableWidgetItem *tp = new MyTableWidgetItem();
    tp->setData(Qt::DisplayRole, proposal->getString("template"));
    tp->setFlags(tp->flags() & ~Qt::ItemIsEditable);

    m_proposalsTable->setRowCount(m_proposalsTable->rowCount()+1);
    m_proposalsTable->setItem(m_proposalsTable->rowCount()-1, PHEADER_STATE, state);
    m_proposalsTable->setItem(m_proposalsTable->rowCount()-1, PHEADER_REFERENCE, reference);
    m_proposalsTable->setItem(m_proposalsTable->rowCount()-1, PHEADER_DESCRIPTION, description);
    m_proposalsTable->setItem(m_proposalsTable->rowCount()-1, PHEADER_CLIENT, client);
    m_proposalsTable->setItem(m_proposalsTable->rowCount()-1, PHEADER_DATE, date);
    m_proposalsTable->setItem(m_proposalsTable->rowCount()-1, PHEADER_TEMPLATE, tp);

    return reference;
}

MyTableWidgetItem *Proposals::addItem(ProposalItem *item)
{
    int id = item->getId();

    MyTableWidgetItem *number = new MyTableWidgetItem();
    number->setData(Qt::DisplayRole, id+1);
    number->setData(Qt::UserRole, QVariant::fromValue(item));
    number->setFlags(number->flags() & ~Qt::ItemIsEditable);

    MyTableWidgetItem *description = new MyTableWidgetItem();
    description->setData(Qt::DisplayRole, item->get("description"));
    description->setFlags(description->flags() & ~Qt::ItemIsEditable);

    MyTableWidgetItem *unit = new MyTableWidgetItem();
    unit->setData(Qt::DisplayRole, item->get("unit"));
    unit->setFlags(unit->flags() & ~Qt::ItemIsEditable);

    MyTableWidgetItem *price = new MyTableWidgetItem();
    price->setData(Qt::DisplayRole, item->get("price"));
    price->setData(Qt::UserRole, item->get("price"));
    price->setFlags(price->flags() & ~Qt::ItemIsEditable);

    MyTableWidgetItem *amount = new MyTableWidgetItem();
    amount->setData(Qt::DisplayRole, item->get("amount"));
    amount->setData(Qt::UserRole, item->get("amount"));
    amount->setFlags(amount->flags() & ~Qt::ItemIsEditable);

    m_itemsTable->setRowCount(m_itemsTable->rowCount()+1);
    m_itemsTable->setItem(m_itemsTable->rowCount()-1, IHEADER_NUMBER, number);
    m_itemsTable->setItem(m_itemsTable->rowCount()-1, IHEADER_DESCRIPTION, description);
    m_itemsTable->setItem(m_itemsTable->rowCount()-1, IHEADER_UNIT, unit);
    m_itemsTable->setItem(m_itemsTable->rowCount()-1, IHEADER_PRICE, price);
    m_itemsTable->setItem(m_itemsTable->rowCount()-1, IHEADER_AMOUNT, amount);

    return number;
}

void Proposals::selectProposal(Thing *proposal)
{
    for(int r = 0; r < m_proposalsTable->rowCount(); ++r) {
        QTableWidgetItem *reference = m_proposalsTable->item(r, PHEADER_REFERENCE);
        if(reference->data(Qt::UserRole).value<Proposal*>() == proposal) {
            m_proposalsTable->selectRow(r);
            return;
        }
    }
}

Proposal *Proposals::getCurrentProposal()
{
    int currentRow = m_proposalsTable->currentRow();
    if(currentRow == -1)
        return NULL;

    QTableWidgetItem *reference = m_proposalsTable->item(currentRow, PHEADER_REFERENCE);
    return reference->data(Qt::UserRole).value<Proposal*>();
}

ProposalItem *Proposals::getCurrentItem()
{
    int currentRow = m_itemsTable->currentRow();
    if(currentRow == -1)
        return NULL;

    QTableWidgetItem *reference = m_itemsTable->item(currentRow, IHEADER_NUMBER);
    return reference->data(Qt::UserRole).value<ProposalItem*>();
}

void Proposals::onAddProposalClicked()
{
    QDialog dialog;
    dialog.setWindowTitle(tr("Add Proposal"));
    dialog.setMinimumWidth(DIALOG_MIN_WIDTH);

    QGridLayout *layout = new QGridLayout();
    dialog.setLayout(layout);

    int row = 0;
    layout->addWidget(new QLabel(tr("Reference:")), row, 0);
    QLineEdit *referenceLineEdit = new QLineEdit;
    layout->addWidget(referenceLineEdit, row++, 1);

    layout->addWidget(new QLabel(tr("Description:")), row, 0);
    QLineEdit *descriptionLineEdit = new QLineEdit;
    layout->addWidget(descriptionLineEdit, row++, 1);

    layout->addWidget(new QLabel(tr("Client:")), row, 0);
    QComboBox *client = new QComboBox();
    const QVector<Company*>& companies = g_project->getCompanies();
    for(int i = 0; i < companies.size(); ++i)
        client->addItem(companies[i]->getString("name"));
    client->model()->sort(0);
    layout->addWidget(client, row++, 1);

    layout->addWidget(new QLabel(tr("Date:")), row, 0);
    QCalendarWidget *calendar = new QCalendarWidget();
    calendar->setGridVisible(true);
    layout->addWidget(calendar, row++, 1);

    layout->addWidget(new QLabel(tr("Template:")), row, 0);
    QComboBox *tp = new QComboBox();
    const QVector<Template*>& templates = g_project->getTemplates();
    for(int i = 0; i < templates.size(); ++i)
        tp->addItem(templates[i]->getString("name"));
    tp->model()->sort(0);
    layout->addWidget(tp, row++, 1);

    layout->addLayout(Tools::createOkCancel(&dialog), row, 0, 1, 2);

    if(dialog.exec() == QDialog::Accepted) {
        Proposal *proposal = new Proposal;
        proposal->set("state", Proposal::STATE_PENDING);
        proposal->set("reference", referenceLineEdit->text());
        proposal->set("description", descriptionLineEdit->text());
        proposal->set("client", client->currentText());
        proposal->set("date", calendar->selectedDate());
        proposal->set("template", tp->currentText());
        if(g_project->addProposal(proposal)) {
            MyTableWidgetItem *item = addProposal(proposal);
            m_proposalsTable->setCurrentItem(item);
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

void Proposals::onAddItemClicked(int index)
{
    Proposal *proposal = getCurrentProposal();
    if(!proposal)
        return;

    QDialog dialog;
    dialog.setWindowTitle(tr("Add Item"));
    dialog.setMinimumWidth(DIALOG_MIN_WIDTH);

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
        item->set("description", descriptionLineEdit->text());
        item->set("unit", unitLineEdit->text());
        item->set("price", priceLineEdit->text().toDouble());
        item->set("amount", amountLineEdit->text().toInt());
        proposal->addItem(item, index);

        MyTableWidgetItem *addedItem = addItem(item);
        m_itemsTable->setCurrentItem(addedItem);

        // Ids must be adjusted
        updateItemsList();
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

    // Ids must be adjusted
    updateItemsList();
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
    dialog.setMinimumWidth(DIALOG_MIN_WIDTH);

    QGridLayout *layout = new QGridLayout();
    dialog.setLayout(layout);

    QComboBox *nState = NULL;
    QLineEdit *nReference = NULL;
    QLineEdit *nDescription = NULL;
    QComboBox *nClient = NULL;
    QCalendarWidget *nDate = NULL;
    QComboBox *nTemplate = NULL;

    int lrow = 0;
    if(column == PHEADER_STATE) {
        nState = new QComboBox();
        nState->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        nState->addItem(QIcon("resources/images/pending.png"), tr("Pending"));
        nState->addItem(QIcon("resources/images/sent.png"), tr("Sent"));
        nState->addItem(QIcon("resources/images/accepted.png"), tr("Accepted"));
        nState->addItem(QIcon("resources/images/declined.png"), tr("Declined"));
        nState->setCurrentIndex(proposal->getInt("state"));

        layout->addWidget(new QLabel(tr("State:")), lrow, 0);
        layout->addWidget(nState, lrow++, 1);
    }
    else if(column == PHEADER_REFERENCE) {
        nReference = new QLineEdit(proposal->getString("reference"));
        layout->addWidget(new QLabel(tr("Reference:")), lrow, 0);
        layout->addWidget(nReference, lrow++, 1);
    }
    else if(column == PHEADER_DESCRIPTION) {
        nDescription = new QLineEdit(proposal->getString("description"));
        layout->addWidget(new QLabel(tr("Description:")), lrow, 0);
        layout->addWidget(nDescription, lrow++, 1);
    }
    else if(column == PHEADER_CLIENT) {
        nClient = new QComboBox();
        nClient->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        const QVector<Company*>& companies = g_project->getCompanies();
        for(int i = 0; i < companies.size(); ++i)
            nClient->addItem(companies[i]->getString("name"));
        nClient->model()->sort(0);
        nClient->setCurrentIndex(nClient->findText(proposal->getString("client")));

        layout->addWidget(new QLabel(tr("Client:")), lrow, 0);
        layout->addWidget(nClient, lrow++, 1);
    }
    else if(column == PHEADER_DATE) {
        nDate = new QCalendarWidget();
        nDate->setGridVisible(true);
        nDate->setSelectedDate(proposal->getDate("date"));

        layout->addWidget(new QLabel(tr("Date:")), lrow, 0);
        layout->addWidget(nDate, lrow++, 1);
    }
    else if(column == PHEADER_TEMPLATE) {
        nTemplate = new QComboBox();
        nTemplate->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        const QVector<Template*>& templates = g_project->getTemplates();
        for(int i = 0; i < templates.size(); ++i)
            nTemplate->addItem(templates[i]->getString("name"));
        nTemplate->model()->sort(0);
        nTemplate->setCurrentIndex(nTemplate->findText(proposal->getString("template")));

        layout->addWidget(new QLabel(tr("Template:")), lrow, 0);
        layout->addWidget(nTemplate, lrow++, 1);
    }

    layout->addLayout(Tools::createOkCancel(&dialog), lrow, 0, 1, 2);

    if(dialog.exec() == QDialog::Accepted) {
        if(nState) {
            proposal->set("state", nState->currentIndex(), true);
        }
        else if(nReference) {
            if(g_project->getProposal(nReference->text()))
                qCritical() << "A proposal with this name already exists.";
            else
                proposal->set("reference", nReference->text(), true);
        }
        else if(nDescription) {
            proposal->set("description", nDescription->text(), true);
        }
        else if(nClient) {
            proposal->set("client", nClient->currentText(), true);
        }
        else if(nDate) {
            proposal->set("date", nDate->selectedDate(), true);
        }
        else if(nTemplate) {
            proposal->set("template", nTemplate->currentText(), true);
        }
        updateProposalsList();
    }
}

void Proposals::onProposalsCustomContextMenuRequested(QPoint pos)
{
    QTableWidgetItem *currentItem = m_proposalsTable->currentItem();
    QMenu menu(this);
    QAction *add = NULL;
    QAction *edit = NULL;
    QAction *remove = NULL;
    QAction *view = NULL;

    add = menu.addAction(tr("Add"));
    if(currentItem && m_proposalsTable->itemAt(pos) && currentItem->row() == m_proposalsTable->itemAt(pos)->row()) {
        edit = menu.addAction(tr("Edit"));
        remove = menu.addAction(tr("Remove"));
        view = menu.addAction(tr("View"));
    }

    QAction *ret = menu.exec(m_proposalsTable->viewport()->mapToGlobal(pos));
    if(add && ret == add) {
        onAddProposalClicked();
    }
    else if(edit && ret == edit) {
        onProposalsCellDoubleClicked(currentItem->row(), currentItem->column());
    }
    else if(remove && ret == remove) {
        onRemoveProposalClicked();
    }
    else if(view && ret == view) {
        Proposal *proposal = getCurrentProposal();
        m_templates->print(proposal);
    }
}

void Proposals::onItemsCustomContextMenuRequested(QPoint pos)
{
    QTableWidgetItem *currentItem = m_itemsTable->currentItem();
    QMenu menu(this);
    QAction *add = NULL;
    QAction *addBefore = NULL;
    QAction *addAfter = NULL;
    QAction *edit = NULL;
    QAction *remove = NULL;

    add = menu.addAction(tr("Add"));
    if(currentItem && m_itemsTable->itemAt(pos) && currentItem->row() == m_itemsTable->itemAt(pos)->row()) {
        addBefore = menu.addAction(tr("Add before #%1").arg(currentItem->row()+1));
        addAfter = menu.addAction(tr("Add after #%1").arg(currentItem->row()+1));
        edit = menu.addAction(tr("Edit"));
        remove = menu.addAction(tr("Remove"));
    }

    QAction *ret = menu.exec(m_itemsTable->viewport()->mapToGlobal(pos));
    if(add &&ret == add) {
        onAddItemClicked();
    }
    else if(addBefore && ret == addBefore) {
        onAddItemClicked(currentItem->row());
    }
    else if(addAfter && ret == addAfter) {
        onAddItemClicked(currentItem->row()+1);
    }
    else if(edit && ret == edit) {

    }
    else if(remove && ret == remove) {
        onRemoveItemClicked();
    }
}

void Proposals::onProjectLoad()
{
    updateProposalsList();
    m_proposalsTable->sortItems(PHEADER_DATE, Qt::DescendingOrder);
    m_itemsTable->sortItems(IHEADER_NUMBER);
}
