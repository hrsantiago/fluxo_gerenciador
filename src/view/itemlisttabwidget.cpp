#include "itemlisttabwidget.h"
#include "const.h"

ItemListTabWidget::ItemListTabWidget(QWidget *parent) :
    QTabWidget(parent)
{
    m_parentThing = NULL;

    setTabsClosable(true);

    QIcon icon = QIcon(QPixmap("resources/images/add.png").scaled(QSize(16, 16), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    QPushButton *addItemListButton = new QPushButton(icon, "");
    setCornerWidget(addItemListButton);

    connect(addItemListButton, SIGNAL(clicked()), this, SLOT(onItemListAddClicked()));
    connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(onItemListCloseRequested(int)));
}

Thing *ItemListTabWidget::getCurrentItemList()
{
    if(!m_parentThing)
        return NULL;

    QString name = tabText(currentIndex());
    return m_parentThing->getChild("ItemList", name);
}

Thing *ItemListTabWidget::getCurrentItem()
{
    QTableWidget *tableWidget = qobject_cast<QTableWidget*>(currentWidget());
    int currentRow = tableWidget->currentRow();
    if(currentRow == -1)
        return NULL;

    QTableWidgetItem *reference = tableWidget->item(currentRow, IHEADER_NUMBER);
    return reference->data(Qt::UserRole).value<Thing*>();
}

void ItemListTabWidget::updateItemsList()
{
    int cIndex = currentIndex();
    clear();

    if(!m_parentThing)
        return;

    const QVector<Thing*>& itemListVector = m_parentThing->getChildren("ItemList");
    for(int i = 0; i < itemListVector.size(); ++i) {
        Thing *child = itemListVector[i];

        QTableWidget *tableWidget = createItemsTable();
        insertTab(0, tableWidget, child->getString(child->getString("mainKey")));

        tableWidget->setSortingEnabled(false);
        const QVector<Thing*>& itemsVector = child->getChildren("Item");
        for(int j = 0; j < itemsVector.size(); ++j)
            addItem(tableWidget, itemsVector[j]);

        tableWidget->setSortingEnabled(true);
        tableWidget->resizeColumnsToContents();
        tableWidget->sortItems(IHEADER_NUMBER, Qt::AscendingOrder);
        if(tableWidget->rowCount() > 0)
            tableWidget->selectRow(0);
    }

    if(cIndex >= 0 && cIndex < count())
        setCurrentIndex(cIndex);
    else if(count() > 0)
        setCurrentIndex(0);
}

QTableWidget *ItemListTabWidget::createItemsTable()
{
    QTableWidget *itemsTable = new QTableWidget;
    itemsTable->setColumnCount(5);
    itemsTable->setHorizontalHeaderLabels(QString("%1,%2,%3,%4,%5").arg("#").arg(tr("Description")).arg(tr("Unit")).arg(tr("Unit Price")).arg(tr("Amount")).split(","));
    itemsTable->verticalHeader()->hide();
    itemsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    itemsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    itemsTable->setSortingEnabled(true);
    itemsTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(itemsTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onItemsCustomContextMenuRequested(QPoint)));
    return itemsTable;
}

MyTableWidgetItem *ItemListTabWidget::addItem(QTableWidget *parent, Thing *item)
{
    int id = item->getParent()->getChildIndex(item);

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
    price->setData(Qt::DisplayRole, item->get("unit price"));
    price->setData(Qt::UserRole, item->get("unit price"));
    price->setFlags(price->flags() & ~Qt::ItemIsEditable);

    MyTableWidgetItem *amount = new MyTableWidgetItem();
    amount->setData(Qt::DisplayRole, item->get("amount"));
    amount->setData(Qt::UserRole, item->get("amount"));
    amount->setFlags(amount->flags() & ~Qt::ItemIsEditable);

    parent->setRowCount(parent->rowCount()+1);
    parent->setItem(parent->rowCount()-1, IHEADER_NUMBER, number);
    parent->setItem(parent->rowCount()-1, IHEADER_DESCRIPTION, description);
    parent->setItem(parent->rowCount()-1, IHEADER_UNIT, unit);
    parent->setItem(parent->rowCount()-1, IHEADER_UNIT_PRICE, price);
    parent->setItem(parent->rowCount()-1, IHEADER_AMOUNT, amount);

    return number;
}

void ItemListTabWidget::onItemListAddClicked()
{
    if(!m_parentThing)
        return;

    QString name;
    for(int i = 0; ; ++i) {
        Thing *itemList = new Thing("ItemList");
        name = tr("Item List %1").arg(i+1);
        itemList->set("name", name);
        if(m_parentThing->addChild(itemList))
            break;
    }

    int index = insertTab(0, createItemsTable(), name);
    setCurrentIndex(index);
}

void ItemListTabWidget::onItemListCloseRequested(int index)
{
    if(count() == 1) {
        qCritical() << tr("You must have at least 1 item list.");
        return;
    }

    QString name = tabText(index);
    if(Tools::requestYesNoFromUser(tr("Remove Item List"), tr("Do you really want to remove item list '%1'?").arg(name)) == "No")
        return;

    if(!m_parentThing)
        return;

    Thing *itemList = m_parentThing->getChild("ItemList", name);
    m_parentThing->removeChild(itemList);
    removeTab(index);
}

void ItemListTabWidget::onRenameItemListClicked()
{
    QString name = tabText(currentIndex());

    QStringList a = Tools::requestDataFromUser(tr("Rename '%1'").arg(name), tr("Name").split(";"));
    if(!Tools::isRequestedDataValid(a))
        return;

    Thing *itemList = getCurrentItemList();

    if(itemList->getParent()->getChild("ItemList", a[0])) {
        qCritical() << tr("An item list with this name already exists.");
        return;
    }

    itemList->set("name", a[0]);
    setTabText(currentIndex(), a[0]);
}

void ItemListTabWidget::onAddItemClicked(int index)
{
    if(!m_parentThing)
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
        Thing *item = new Thing("Item");
        item->set("description", descriptionLineEdit->text());
        item->set("unit", unitLineEdit->text());
        item->set("unit price", priceLineEdit->text().toDouble());
        item->set("amount", amountLineEdit->text().toInt());

        Thing *itemList = getCurrentItemList();
        if(!itemList->addChild(item, index)) {
            qCritical() << tr("An item with this description already exist.");
            return;
        }

        QTableWidget *tableWidget = qobject_cast<QTableWidget*>(currentWidget());
        tableWidget->setSortingEnabled(false);

        MyTableWidgetItem *addedItem = addItem(tableWidget, item);
        tableWidget->setCurrentItem(addedItem);

        for(int i = 0; i < tableWidget->rowCount(); ++i) {
            QTableWidgetItem *item = tableWidget->item(i, IHEADER_NUMBER);
            Thing *thing = item->data(Qt::UserRole).value<Thing*>();
            item->setData(Qt::DisplayRole, thing->getParent()->getChildIndex(thing)+1);
        }
        tableWidget->setSortingEnabled(true);
    }
}

void ItemListTabWidget::onRemoveItemClicked()
{
    QTableWidget *tableWidget = qobject_cast<QTableWidget*>(currentWidget());
    int currentRow = tableWidget->currentRow();
    if(currentRow == -1)
        return;

    if(Tools::requestYesNoFromUser(tr("Remove Item"), tr("Do you really want to remove item #%1?").arg(currentRow+1)) == "No")
        return;

    Thing *item = getCurrentItem();
    item->getParent()->removeChild(item);

    // Ids must be adjusted
    tableWidget->removeRow(currentRow);
    updateItemsList();
}

void ItemListTabWidget::onItemsCustomContextMenuRequested(QPoint pos)
{
    QTableWidget *tableWidget = qobject_cast<QTableWidget*>(sender());
    QTableWidgetItem *currentItem = tableWidget->currentItem();
    QMenu menu(this);
    QAction *add = NULL;
    QAction *addBefore = NULL;
    QAction *addAfter = NULL;
    QAction *edit = NULL;
    QAction *remove = NULL;

    int id = 0;
    add = menu.addAction(tr("Add"));
    if(currentItem && tableWidget->itemAt(pos) && currentItem->row() == tableWidget->itemAt(pos)->row()) {
        id = tableWidget->item(currentItem->row(), IHEADER_NUMBER)->data(Qt::DisplayRole).toInt();
        addBefore = menu.addAction(tr("Add before #%1").arg(id));
        addAfter = menu.addAction(tr("Add after #%1").arg(id));
        edit = menu.addAction(tr("Edit"));
        remove = menu.addAction(tr("Remove"));
    }

    menu.addSeparator();

    QString itemListName = tabText(currentIndex());
    QAction *renameItemList = menu.addAction(tr("Rename '%1'").arg(itemListName));

    QAction *ret = menu.exec(tableWidget->viewport()->mapToGlobal(pos));
    if(add && ret == add) {
        onAddItemClicked();
    }
    else if(addBefore && ret == addBefore) {
        onAddItemClicked(id-1);
    }
    else if(addAfter && ret == addAfter) {
        onAddItemClicked(id);
    }
    else if(edit && ret == edit) {

    }
    else if(remove && ret == remove) {
        onRemoveItemClicked();
    }
    else if(renameItemList && ret == renameItemList) {
        onRenameItemListClicked();
    }
}
