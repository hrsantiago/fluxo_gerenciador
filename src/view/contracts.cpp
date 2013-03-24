#include "contracts.h"
#include "tools.h"
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
    m_contractsTable = new QTableWidget;
    m_contractsTable->setColumnCount(6);
    m_contractsTable->setHorizontalHeaderLabels(QString("%1,%2,%3,%4,%5,%6").arg("*").arg(tr("Reference")).arg(tr("Description")).arg(tr("Client")).arg(tr("Date")).arg(tr("Template")).split(","));
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
    m_itemsTable = new QTableWidget;
    m_itemsTable->setColumnCount(5);
    m_itemsTable->setHorizontalHeaderLabels(QString("%1,%2,%3,%4,%5").arg("#").arg(tr("Description")).arg(tr("Unit")).arg(tr("Price")).arg(tr("Amount")).split(","));
    m_itemsTable->verticalHeader()->hide();
    m_itemsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_itemsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_itemsTable->setSortingEnabled(true);
    m_itemsTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_itemsTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onItemsCustomContextMenuRequested(QPoint)));

    m_itemsLabel = new QLabel(tr("List of items of contract '%1':").arg(""));

    QWidget *bottom = new QWidget;
    gLayout = new QGridLayout();
    bottom->setLayout(gLayout);

    row = 0;
    gLayout->addWidget(m_itemsLabel, row++, 0);
    gLayout->addWidget(m_itemsTable, row, 0);

    // Splitter
    QSplitter *vSplitter = new QSplitter(Qt::Vertical);
    tabWidget->addTab(vSplitter, tr("Contracts"));

    vSplitter->addWidget(top);
    vSplitter->addWidget(bottom);
    vSplitter->setStretchFactor(0, 30);
    vSplitter->setStretchFactor(1, 70);

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
        MyTableWidgetItem *item = addContract((Contract*)*it);
        if(reference == (*it)->getString("reference"))
            currentItem = item;
    }

    m_contractsTable->setSortingEnabled(true);
    m_contractsTable->resizeColumnsToContents();

    if(currentItem)
        m_contractsTable->selectRow(m_contractsTable->row(currentItem));
    else if(m_contractsTable->rowCount() > 0)
        m_contractsTable->selectRow(0);

    updateItemsList();
}

void Contracts::updateItemsList()
{
    ContractItem *item = NULL;
    QTableWidgetItem *numberItem = m_itemsTable->item(m_itemsTable->currentRow(), IHEADER_NUMBER);
    if(numberItem)
        item = numberItem->data(Qt::UserRole).value<ContractItem*>();

    m_itemsTable->setSortingEnabled(false);
    m_itemsTable->clearContents();
    m_itemsTable->setRowCount(0);

    Contract *contract = getCurrentContract();
    if(!contract) {
        m_itemsTable->setSortingEnabled(true);
        m_itemsTable->resizeColumnsToContents();
        return;
    }

    m_itemsLabel->setText(tr("List of items of contract '%1':").arg(contract->getString("reference")));

    MyTableWidgetItem *currentItem = NULL;

    const QVector<ContractItem*>& items = contract->getItems();
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

MyTableWidgetItem *Contracts::addContract(Contract *contract)
{
    QLocale locale;

    MyTableWidgetItem *state = new MyTableWidgetItem();
    Contract::State stateValue = (Contract::State)contract->getInt("state");
    if(stateValue == Contract::STATE_PENDING) {
        state->setIcon(QIcon("resources/images/pending.png"));
        state->setToolTip(tr("Pending"));
    }
    else if(stateValue == Contract::STATE_SENT) {
        state->setIcon(QIcon("resources/images/sent.png"));
        state->setToolTip(tr("Sent"));
    }
    else if(stateValue == Contract::STATE_ACCEPTED) {
        state->setIcon(QIcon("resources/images/accepted.png"));
        state->setToolTip(tr("Accepted"));
    }
    else if(stateValue == Contract::STATE_DECLINED) {
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

    MyTableWidgetItem *date = new MyTableWidgetItem();
    date->setData(Qt::DisplayRole, locale.toString(contract->getDate("date"), "dd/MM/yyyy (dddd)"));
    date->setData(Qt::UserRole, contract->getDate("date"));
    date->setFlags(date->flags() & ~Qt::ItemIsEditable);

    MyTableWidgetItem *tp = new MyTableWidgetItem();
    tp->setData(Qt::DisplayRole, contract->getString("template"));
    tp->setFlags(tp->flags() & ~Qt::ItemIsEditable);

    m_contractsTable->setRowCount(m_contractsTable->rowCount()+1);
    m_contractsTable->setItem(m_contractsTable->rowCount()-1, PHEADER_STATE, state);
    m_contractsTable->setItem(m_contractsTable->rowCount()-1, PHEADER_REFERENCE, reference);
    m_contractsTable->setItem(m_contractsTable->rowCount()-1, PHEADER_DESCRIPTION, description);
    m_contractsTable->setItem(m_contractsTable->rowCount()-1, PHEADER_CLIENT, client);
    m_contractsTable->setItem(m_contractsTable->rowCount()-1, PHEADER_DATE, date);
    m_contractsTable->setItem(m_contractsTable->rowCount()-1, PHEADER_TEMPLATE, tp);

    return reference;
}

MyTableWidgetItem *Contracts::addItem(ContractItem *item)
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

void Contracts::selectContract(Thing *contract)
{
    for(int r = 0; r < m_contractsTable->rowCount(); ++r) {
        QTableWidgetItem *reference = m_contractsTable->item(r, PHEADER_REFERENCE);
        if(reference->data(Qt::UserRole).value<Contract*>() == contract) {
            m_contractsTable->selectRow(r);
            return;
        }
    }
}

Contract *Contracts::getCurrentContract()
{
    int currentRow = m_contractsTable->currentRow();
    if(currentRow == -1)
        return NULL;

    QTableWidgetItem *reference = m_contractsTable->item(currentRow, PHEADER_REFERENCE);
    return reference->data(Qt::UserRole).value<Contract*>();
}

ContractItem *Contracts::getCurrentItem()
{
    int currentRow = m_itemsTable->currentRow();
    if(currentRow == -1)
        return NULL;

    QTableWidgetItem *reference = m_itemsTable->item(currentRow, IHEADER_NUMBER);
    return reference->data(Qt::UserRole).value<ContractItem*>();
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

    layout->addWidget(new QLabel(tr("Date:")), row, 0);
    QCalendarWidget *calendar = new QCalendarWidget();
    calendar->setGridVisible(true);
    layout->addWidget(calendar, row++, 1);

    layout->addWidget(new QLabel(tr("Template:")), row, 0);
    QComboBox *tp = new QComboBox();
    const QVector<Thing*>& templates = g_project->getThings("Template");
    for(int i = 0; i < templates.size(); ++i)
        tp->addItem(templates[i]->getString("name"));
    tp->model()->sort(0);
    layout->addWidget(tp, row++, 1);

    layout->addLayout(Tools::createOkCancel(&dialog), row, 0, 1, 2);

    if(dialog.exec() == QDialog::Accepted) {
        Contract *contract = new Contract;
        contract->set("state", Contract::STATE_PENDING);
        contract->set("reference", referenceLineEdit->text());
        contract->set("description", descriptionLineEdit->text());
        contract->set("client", client->currentText());
        contract->set("date", calendar->selectedDate());
        contract->set("template", tp->currentText());
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

void Contracts::onAddItemClicked(int index)
{
    Contract *contract = getCurrentContract();
    if(!contract)
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
        ContractItem *item = new ContractItem;
        item->set("description", descriptionLineEdit->text());
        item->set("unit", unitLineEdit->text());
        item->set("price", priceLineEdit->text().toDouble());
        item->set("amount", amountLineEdit->text().toInt());
        contract->addItem(item, index);

        MyTableWidgetItem *addedItem = addItem(item);
        m_itemsTable->setCurrentItem(addedItem);

        // Ids must be adjusted
        updateItemsList();
    }
}

void Contracts::onRemoveItemClicked()
{
    int currentRow = m_itemsTable->currentRow();
    if(currentRow == -1)
        return;

    if(Tools::requestYesNoFromUser(tr("Remove Item"), tr("Do you really want to remove item #%1?").arg(currentRow+1)) == "No")
        return;

    Contract *contract = getCurrentContract();
    contract->removeItem(currentRow);

    // Ids must be adjusted
    updateItemsList();
}

void Contracts::onContractsCurrentCellChanged(int currentRow, int, int previousRow, int)
{
    if(currentRow != previousRow)
        updateItemsList();
}

void Contracts::onContractsCellDoubleClicked(int row, int column)
{
    QTableWidgetItem *reference = m_contractsTable->item(row, PHEADER_REFERENCE);
    Contract *contract = (Contract*)g_project->getThing("Contract", reference->text());

    QDialog dialog;
    dialog.setWindowTitle(tr("Edit Contract %1").arg(reference->text()));
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
        nState->setCurrentIndex(contract->getInt("state"));

        layout->addWidget(new QLabel(tr("State:")), lrow, 0);
        layout->addWidget(nState, lrow++, 1);
    }
    else if(column == PHEADER_REFERENCE) {
        nReference = new QLineEdit(contract->getString("reference"));
        layout->addWidget(new QLabel(tr("Reference:")), lrow, 0);
        layout->addWidget(nReference, lrow++, 1);
    }
    else if(column == PHEADER_DESCRIPTION) {
        nDescription = new QLineEdit(contract->getString("description"));
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
    else if(column == PHEADER_DATE) {
        nDate = new QCalendarWidget();
        nDate->setGridVisible(true);
        nDate->setSelectedDate(contract->getDate("date"));

        layout->addWidget(new QLabel(tr("Date:")), lrow, 0);
        layout->addWidget(nDate, lrow++, 1);
    }
    else if(column == PHEADER_TEMPLATE) {
        nTemplate = new QComboBox();
        nTemplate->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        const QVector<Thing*>& templates = g_project->getThings("Template");
        for(int i = 0; i < templates.size(); ++i)
            nTemplate->addItem(templates[i]->getString("name"));
        nTemplate->model()->sort(0);
        nTemplate->setCurrentIndex(nTemplate->findText(contract->getString("template")));

        layout->addWidget(new QLabel(tr("Template:")), lrow, 0);
        layout->addWidget(nTemplate, lrow++, 1);
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
        else if(nDate) {
            contract->set("date", nDate->selectedDate(), true);
        }
        else if(nTemplate) {
            contract->set("template", nTemplate->currentText(), true);
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

void Contracts::onItemsCustomContextMenuRequested(QPoint pos)
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

void Contracts::onProjectLoad()
{
    updateContractsList();
    m_contractsTable->sortItems(PHEADER_DATE, Qt::DescendingOrder);
    m_itemsTable->sortItems(IHEADER_NUMBER);
}
