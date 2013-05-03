#include "contracts.h"
#include "tools.h"
#include "core/project.h"
#include "const.h"

Contracts::Contracts()
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
    QStringList headerLabels = QString("%1,%2,%3,%4,%5,%6").arg("*").arg(tr("Reference")).arg(tr("Description")).arg(tr("Client")).arg(tr("Start Date")).arg(tr("End Date")).split(",");
    m_contractsTable = new QTableWidget;
    m_contractsTable->setColumnCount(headerLabels.size());
    m_contractsTable->setHorizontalHeaderLabels(headerLabels);
    m_contractsTable->verticalHeader()->hide();
    m_contractsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_contractsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_contractsTable->setSortingEnabled(true);
    m_contractsTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_contractsTable, SIGNAL(currentCellChanged(int,int,int,int)), this, SLOT(onContractsCurrentCellChanged(int,int,int,int)));
    connect(m_contractsTable, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(onContractsCellDoubleClicked(int, int)));
    connect(m_contractsTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onContractsCustomContextMenuRequested(QPoint)));

    QWidget *top = new QWidget;
    gLayout = new QGridLayout();
    top->setLayout(gLayout);

    row = 0;
    gLayout->addWidget(new QLabel(tr("List of contracts:")), row++, 0);
    gLayout->addWidget(m_contractsTable, row, 0);

    // Bottom
    QVector<ExtraItem*> extraItems;
    ExtraItem *extraItem = new ExtraItem(QString(tr("Done")), QString("amount_done"));
    extraItems.push_back(extraItem);
    m_itemListTabWidget = new ItemListTabWidget(extraItems);

    QWidget *bottom = new QWidget;
    gLayout = new QGridLayout();
    bottom->setLayout(gLayout);

    row = 0;
    gLayout->addWidget(m_itemListTabWidget, row, 0);

    // Splitter
    QSplitter *vSplitter = new QSplitter(Qt::Vertical);
    tabWidget->addTab(vSplitter, tr("Contracts"));

    vSplitter->addWidget(top);
    vSplitter->addWidget(bottom);
    vSplitter->setStretchFactor(0, 15);
    vSplitter->setStretchFactor(1, 85);

    // Templates
    QWidget *invoice = new QWidget();
    tabWidget->addTab(invoice, tr("Invoices"));

    // Statistics
    QWidget *statistics = new QWidget();
    tabWidget->addTab(statistics, tr("Statistics"));

    connect(g_project, SIGNAL(projectLoad()), this, SLOT(onProjectLoad()));
}

void Contracts::updateContractsList()
{
    QString reference;
    QTableWidgetItem *referenceItem = m_contractsTable->item(m_contractsTable->currentRow(), PHEADER_REFERENCE);
    if(referenceItem)
        reference = referenceItem->text();

    MyTableWidgetItem *currentItem = NULL;

    m_contractsTable->setSortingEnabled(false);
    m_contractsTable->clearContents();
    m_contractsTable->setRowCount(0);

    const QVector<Thing*>& contracts = g_project->getThings("Contract");
    for(QVector<Thing*>::const_iterator it = contracts.constBegin(), end = contracts.constEnd(); it != end; ++it) {
        MyTableWidgetItem *item = addContract(*it);
        if(reference == (*it)->getString("reference"))
            currentItem = item;
    }

    m_contractsTable->setSortingEnabled(true);
    m_contractsTable->resizeColumnsToContents();

    m_itemListTabWidget->setParentThing(getCurrentContract());
    if(currentItem)
        m_contractsTable->selectRow(m_contractsTable->row(currentItem));
    else if(m_contractsTable->rowCount() > 0) {
        m_contractsTable->selectRow(0);
        m_itemListTabWidget->updateItemsList();
    }
    else
        m_itemListTabWidget->updateItemsList();
}

MyTableWidgetItem *Contracts::addContract(Thing *contract)
{
    QLocale locale;

    MyTableWidgetItem *state = new MyTableWidgetItem();
    ContractState stateValue = (ContractState)contract->getInt("state");
    if(stateValue == STATE_IN_PROGRESS) {
        state->setIcon(QIcon("resources/images/process.png"));
        state->setToolTip(tr("Pending"));
    }
    else if(stateValue == STATE_COMPLETED) {
        state->setIcon(QIcon("resources/images/accepted.png"));
        state->setToolTip(tr("Accepted"));
    }
    else if(stateValue == STATE_CANCELED) {
        state->setIcon(QIcon("resources/images/declined.png"));
        state->setToolTip(tr("Declined"));
    }
    state->setData(Qt::UserRole, contract->getInt("state"));
    state->setFlags(state->flags() & ~Qt::ItemIsEditable);

    MyTableWidgetItem *reference = new MyTableWidgetItem();
    reference->setData(Qt::DisplayRole, contract->getString("reference"));
    reference->setData(Qt::UserRole, QVariant::fromValue(contract));
    reference->setFlags(reference->flags() & ~Qt::ItemIsEditable);

    MyTableWidgetItem *description = new MyTableWidgetItem();
    description->setData(Qt::DisplayRole, contract->getString("description"));
    description->setFlags(description->flags() & ~Qt::ItemIsEditable);

    MyTableWidgetItem *client = new MyTableWidgetItem();
    client->setData(Qt::DisplayRole, contract->getString("client"));
    client->setFlags(client->flags() & ~Qt::ItemIsEditable);

    MyTableWidgetItem *dateStart = new MyTableWidgetItem();
    dateStart->setData(Qt::DisplayRole, locale.toString(contract->getDate("date_start"), "dd/MM/yyyy (dddd)"));
    dateStart->setData(Qt::UserRole, contract->getDate("date_start"));
    dateStart->setFlags(dateStart->flags() & ~Qt::ItemIsEditable);

    MyTableWidgetItem *dateEnd = new MyTableWidgetItem();
    dateEnd->setData(Qt::DisplayRole, locale.toString(contract->getDate("date_end"), "dd/MM/yyyy (dddd)"));
    dateEnd->setData(Qt::UserRole, contract->getDate("date_end"));
    dateEnd->setFlags(dateEnd->flags() & ~Qt::ItemIsEditable);

    m_contractsTable->setRowCount(m_contractsTable->rowCount()+1);
    m_contractsTable->setItem(m_contractsTable->rowCount()-1, PHEADER_STATE, state);
    m_contractsTable->setItem(m_contractsTable->rowCount()-1, PHEADER_REFERENCE, reference);
    m_contractsTable->setItem(m_contractsTable->rowCount()-1, PHEADER_DESCRIPTION, description);
    m_contractsTable->setItem(m_contractsTable->rowCount()-1, PHEADER_CLIENT, client);
    m_contractsTable->setItem(m_contractsTable->rowCount()-1, PHEADER_START_DATE, dateStart);
    m_contractsTable->setItem(m_contractsTable->rowCount()-1, PHEADER_END_DATE, dateEnd);

    return reference;
}

void Contracts::selectContract(Thing *contract)
{
    for(int r = 0; r < m_contractsTable->rowCount(); ++r) {
        QTableWidgetItem *reference = m_contractsTable->item(r, PHEADER_REFERENCE);
        if(reference->data(Qt::UserRole).value<Thing*>() == contract) {
            m_contractsTable->selectRow(r);
            return;
        }
    }
}

Thing *Contracts::getCurrentContract()
{
    int currentRow = m_contractsTable->currentRow();
    if(currentRow == -1)
        return NULL;

    QTableWidgetItem *reference = m_contractsTable->item(currentRow, PHEADER_REFERENCE);
    return reference->data(Qt::UserRole).value<Thing*>();
}

void Contracts::onAddContractClicked()
{
    QDialog dialog;
    dialog.setWindowTitle(tr("Add Contract"));
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
    const QVector<Thing*>& companies = g_project->getThings("Company");
    for(int i = 0; i < companies.size(); ++i)
        client->addItem(companies[i]->getString("name"));
    client->model()->sort(0);
    layout->addWidget(client, row++, 1);

    layout->addWidget(new QLabel(tr("Start Date:")), row, 0);
    QCalendarWidget *calendar = new QCalendarWidget();
    calendar->setGridVisible(true);
    layout->addWidget(calendar, row++, 1);

    layout->addLayout(Tools::createOkCancel(&dialog), row, 0, 1, 2);

    if(dialog.exec() == QDialog::Accepted) {
        Thing *contract = new Thing("Contract");
        contract->set("state", STATE_PENDING);
        contract->set("reference", referenceLineEdit->text());
        contract->set("description", descriptionLineEdit->text());
        contract->set("client", client->currentText());
        contract->set("date_start", calendar->selectedDate());
        contract->set("date_end", calendar->selectedDate().addMonths(1));

        Thing *itemList = new Thing("ItemList");
        itemList->set("name", tr("Item List %1").arg(1));
        contract->addChild(itemList);

        if(g_project->addThing(contract)) {
            MyTableWidgetItem *item = addContract(contract);
            m_contractsTable->setCurrentItem(item);
            m_contractsTable->resizeColumnsToContents();
        }
        else
            qCritical() << tr("A contract with this reference already exists.");
    }
}

void Contracts::onRemoveContractClicked()
{
    int currentRow = m_contractsTable->currentRow();
    if(currentRow == -1)
        return;

    QTableWidgetItem *reference = m_contractsTable->item(currentRow, PHEADER_REFERENCE);

    if(Tools::requestYesNoFromUser(tr("Remove Contract"), tr("Do you really want to remove contract %1?").arg(reference->text())) == "No")
        return;

    g_project->removeThing("Contract", reference->text());
    m_contractsTable->removeRow(currentRow);
}

void Contracts::onContractsCurrentCellChanged(int currentRow, int, int previousRow, int)
{
    if(currentRow != previousRow) {
        m_itemListTabWidget->setParentThing(getCurrentContract());
        m_itemListTabWidget->updateItemsList();
    }
}

void Contracts::onContractsCellDoubleClicked(int row, int column)
{
    QTableWidgetItem *reference = m_contractsTable->item(row, PHEADER_REFERENCE);
    Thing *contract = g_project->getThing("Contract", reference->text());

    QDialog dialog;
    dialog.setWindowTitle(tr("Edit Contract %1").arg(reference->text()));
    dialog.setMinimumWidth(DIALOG_MIN_WIDTH);

    QGridLayout *layout = new QGridLayout();
    dialog.setLayout(layout);

    QComboBox *nState = NULL;
    QLineEdit *nReference = NULL;
    QLineEdit *nDescription = NULL;
    QComboBox *nClient = NULL;
    QCalendarWidget *nStartDate = NULL;
    QCalendarWidget *nEndDate = NULL;

    int lrow = 0;
    if(column == PHEADER_STATE) {
        nState = new QComboBox();
        nState->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        nState->addItem(QIcon("resources/images/pending.png"), tr("In Progress"));
        nState->addItem(QIcon("resources/images/accepted.png"), tr("Completed"));
        nState->addItem(QIcon("resources/images/declined.png"), tr("Canceled"));
        nState->setCurrentIndex(contract->getInt("state"));

        layout->addWidget(new QLabel(tr("State:")), lrow, 0);
        layout->addWidget(nState, lrow++, 1);
    }
    else if(column == PHEADER_REFERENCE) {
        nReference = new QLineEdit(contract->getString("reference"));
        nReference->selectAll();
        layout->addWidget(new QLabel(tr("Reference:")), lrow, 0);
        layout->addWidget(nReference, lrow++, 1);
    }
    else if(column == PHEADER_DESCRIPTION) {
        nDescription = new QLineEdit(contract->getString("description"));
        nDescription->selectAll();
        layout->addWidget(new QLabel(tr("Description:")), lrow, 0);
        layout->addWidget(nDescription, lrow++, 1);
    }
    else if(column == PHEADER_CLIENT) {
        nClient = new QComboBox();
        nClient->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        const QVector<Thing*>& companies = g_project->getThings("Company");
        for(int i = 0; i < companies.size(); ++i)
            nClient->addItem(companies[i]->getString("name"));
        nClient->model()->sort(0);
        nClient->setCurrentIndex(nClient->findText(contract->getString("client")));

        layout->addWidget(new QLabel(tr("Client:")), lrow, 0);
        layout->addWidget(nClient, lrow++, 1);
    }
    else if(column == PHEADER_START_DATE) {
        nStartDate = new QCalendarWidget();
        nStartDate->setGridVisible(true);
        nStartDate->setSelectedDate(contract->getDate("date_start"));

        layout->addWidget(new QLabel(tr("Start Date:")), lrow, 0);
        layout->addWidget(nStartDate, lrow++, 1);
    }
    else if(column == PHEADER_END_DATE) {
        nEndDate = new QCalendarWidget();
        nEndDate->setGridVisible(true);
        nEndDate->setSelectedDate(contract->getDate("date_end"));

        layout->addWidget(new QLabel(tr("End Date:")), lrow, 0);
        layout->addWidget(nEndDate, lrow++, 1);
    }

    layout->addLayout(Tools::createOkCancel(&dialog), lrow, 0, 1, 2);

    if(dialog.exec() == QDialog::Accepted) {
        if(nState) {
            contract->set("state", nState->currentIndex(), true);
        }
        else if(nReference) {
            if(g_project->getThing("Contract", nReference->text()))
                qCritical() << "A contract with this name already exists.";
            else
                contract->set("reference", nReference->text(), true);
        }
        else if(nDescription) {
            contract->set("description", nDescription->text(), true);
        }
        else if(nClient) {
            contract->set("client", nClient->currentText(), true);
        }
        else if(nStartDate) {
            contract->set("date_start", nStartDate->selectedDate(), true);
        }
        else if(nEndDate) {
            contract->set("date_end", nEndDate->selectedDate(), true);
        }
        updateContractsList();
    }
}

void Contracts::onContractsCustomContextMenuRequested(QPoint pos)
{
    QTableWidgetItem *currentItem = m_contractsTable->currentItem();
    QMenu menu(this);
    QAction *add = NULL;
    QAction *edit = NULL;
    QAction *remove = NULL;

    add = menu.addAction(tr("Add"));
    if(currentItem && m_contractsTable->itemAt(pos) && currentItem->row() == m_contractsTable->itemAt(pos)->row()) {
        edit = menu.addAction(tr("Edit"));
        remove = menu.addAction(tr("Remove"));
    }

    QAction *ret = menu.exec(m_contractsTable->viewport()->mapToGlobal(pos));
    if(add && ret == add) {
        onAddContractClicked();
    }
    else if(edit && ret == edit) {
        onContractsCellDoubleClicked(currentItem->row(), currentItem->column());
    }
    else if(remove && ret == remove) {
        onRemoveContractClicked();
    }
}

void Contracts::onProjectLoad()
{
    updateContractsList();
    m_contractsTable->sortItems(PHEADER_START_DATE, Qt::DescendingOrder);
    m_contractsTable->selectRow(0);
}
