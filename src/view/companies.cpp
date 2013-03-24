#include "companies.h"
#include "core/project.h"
#include "tools.h"

Companies::Companies(QWidget *parent) :
    QWidget(parent)
{
    QGridLayout *gridLayout;

    // Left Widget
    QWidget *left = new QWidget;
    gridLayout = new QGridLayout();
    gridLayout->setContentsMargins(0, 0, 0, 0);
    left->setLayout(gridLayout);

    m_list = new QListWidget;
    connect(m_list, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(onListItemChanged(QListWidgetItem*,QListWidgetItem*)));
    gridLayout->addWidget(m_list, 1, 0);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    gridLayout->addLayout(buttonsLayout, 2, 0);

    QPushButton *addButton = new QPushButton(tr("Add"));
    connect(addButton, SIGNAL(clicked()), this, SLOT(onAddButtonClicked()));
    buttonsLayout->addWidget(addButton);

    QPushButton *removeButton = new QPushButton(tr("Remove"));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(onRemoveButtonClicked()));
    buttonsLayout->addWidget(removeButton);

    // Right Widget
    QWidget *right = new QWidget;
    gridLayout = new QGridLayout();
    right->setLayout(gridLayout);

    m_nameWidget = new QLineEdit();
    m_addressWidget = new QLineEdit();
    m_cityWidget = new QLineEdit();
    m_stateWidget = new QLineEdit();
    m_zipcodeWidget = new QLineEdit();
    m_telephoneWidget = new QLineEdit();
    connectWidgets();

    int row = 0;
    gridLayout->addWidget(new QLabel(tr("Name:")), row, 0);
    gridLayout->addWidget(m_nameWidget, row++, 1);
    gridLayout->addWidget(new QLabel(tr("Address:")), row, 0);
    gridLayout->addWidget(m_addressWidget, row++, 1);
    gridLayout->addWidget(new QLabel(tr("City:")), row, 0);
    gridLayout->addWidget(m_cityWidget, row++, 1);
    gridLayout->addWidget(new QLabel(tr("State:")), row, 0);
    gridLayout->addWidget(m_stateWidget, row++, 1);
    gridLayout->addWidget(new QLabel(tr("Zipcode:")), row, 0);
    gridLayout->addWidget(m_zipcodeWidget, row++, 1);
    gridLayout->addWidget(new QLabel(tr("Telephone:")), row, 0);
    gridLayout->addWidget(m_telephoneWidget, row++, 1);
    gridLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), row++, 0, 1, 2);

    // Splitter
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    QSplitter *hSplitter = new QSplitter(Qt::Horizontal);
    layout->addWidget(hSplitter);

    hSplitter->addWidget(left);
    hSplitter->addWidget(right);
    hSplitter->setStretchFactor(0, 30);
    hSplitter->setStretchFactor(1, 70);

    connect(g_project, SIGNAL(projectLoad()), this, SLOT(onProjectLoad()));
}

QString Companies::createCompany()
{
    QMap<int, QString> defaultData;
    defaultData[0] = "QLabel|";

    QStringList a = Tools::requestDataFromUser(tr("Add Company"), tr("Location and contact data,"
                                                                     "Name,"
                                                                     "Address,"
                                                                     "City,"
                                                                     "State,"
                                                                     "Zipcode,"
                                                                     "Telephone").split(","), defaultData);
    if(!Tools::isRequestedDataValid(a))
        return QString();

    Company *company = new Company;
    company->set("name", a[0]);
    company->set("address", a[1]);
    company->set("city", a[2]);
    company->set("state", a[3]);
    company->set("zipcode", a[4]);
    company->set("telephone", a[5]);

    if(!g_project->addThing(company)) {
        qCritical() << tr("A company with this name already exists.");
        return QString();
    }

    updateList();
    return company->getString("name");
}

void Companies::connectWidgets()
{
    connect(m_nameWidget, SIGNAL(textChanged(QString)), this, SLOT(onWidgetChanged()));
    connect(m_addressWidget, SIGNAL(textChanged(QString)), this, SLOT(onWidgetChanged()));
    connect(m_cityWidget, SIGNAL(textChanged(QString)), this, SLOT(onWidgetChanged()));
    connect(m_stateWidget, SIGNAL(textChanged(QString)), this, SLOT(onWidgetChanged()));
    connect(m_zipcodeWidget, SIGNAL(textChanged(QString)), this, SLOT(onWidgetChanged()));
    connect(m_telephoneWidget, SIGNAL(textChanged(QString)), this, SLOT(onWidgetChanged()));
}

void Companies::disconnectWidgets()
{
    disconnect(m_nameWidget, SIGNAL(textChanged(QString)), this, SLOT(onWidgetChanged()));
    disconnect(m_addressWidget, SIGNAL(textChanged(QString)), this, SLOT(onWidgetChanged()));
    disconnect(m_cityWidget, SIGNAL(textChanged(QString)), this, SLOT(onWidgetChanged()));
    disconnect(m_stateWidget, SIGNAL(textChanged(QString)), this, SLOT(onWidgetChanged()));
    disconnect(m_zipcodeWidget, SIGNAL(textChanged(QString)), this, SLOT(onWidgetChanged()));
    disconnect(m_telephoneWidget, SIGNAL(textChanged(QString)), this, SLOT(onWidgetChanged()));
}

void Companies::updateList()
{
    QString currentName;
    if(m_list->currentItem())
        currentName = m_list->currentItem()->text();

    QListWidgetItem *currentItem = NULL;

    disconnect(m_list, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(onListItemChanged(QListWidgetItem*,QListWidgetItem*)));
    m_list->clear();
    const QVector<Thing*>& companies = g_project->getThings("Company");
    for(QVector<Thing*>::const_iterator it = companies.constBegin(), end = companies.constEnd(); it != end; ++it) {
        QListWidgetItem *item = new QListWidgetItem((*it)->getString("name"));
        m_list->addItem(item);

        if((*it)->getString("name") == currentName)
            currentItem = item;
    }
    m_list->sortItems();
    connect(m_list, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(onListItemChanged(QListWidgetItem*,QListWidgetItem*)));

    if(currentItem)
        m_list->setCurrentItem(currentItem);
    else if(m_list->count() > 0)
        m_list->setCurrentRow(0);
}

void Companies::updateCompany(const QString& name)
{
    Company *company = (Company*)g_project->getThing("Company", name);
    if(company) {
        company->set("name", m_nameWidget->text());
        company->set("address", m_addressWidget->text());
        company->set("city", m_cityWidget->text());
        company->set("state", m_stateWidget->text());
        company->set("zipcode", m_zipcodeWidget->text());
        company->set("telephone", m_telephoneWidget->text());
    }
}

void Companies::onWidgetChanged()
{
    if(m_list->currentItem()) {
        QString name = m_list->currentItem()->text();
        updateCompany(name);

        if(sender() && sender() == m_nameWidget) {
            disconnect(m_list, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(onListItemChanged(QListWidgetItem*,QListWidgetItem*)));
            m_list->currentItem()->setText(m_nameWidget->text());
            m_list->sortItems();
            connect(m_list, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(onListItemChanged(QListWidgetItem*,QListWidgetItem*)));
        }
    }
    else {
        qCritical() << tr("You need to create a company before editing.");
    }
}

void Companies::onListItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if(previous) {
        QString name = previous->text();
        updateCompany(name);
    }
    if(current) {
        QString name = current->text();
        Company *company = (Company*)g_project->getThing("Company", name);
        disconnectWidgets();
        m_nameWidget->setText(company->getString("name"));
        m_addressWidget->setText(company->getString("address"));
        m_cityWidget->setText(company->getString("city"));
        m_stateWidget->setText(company->getString("state"));
        m_zipcodeWidget->setText(company->getString("zipcode"));
        m_telephoneWidget->setText(company->getString("telephone"));
        connectWidgets();
    }
}

void Companies::onAddButtonClicked()
{
    createCompany();
}

void Companies::onRemoveButtonClicked()
{
    QListWidgetItem *currentItem = m_list->currentItem();
    if(currentItem) {
        if(Tools::requestYesNoFromUser(tr("Remove Company"), tr("Do you really want to remove this company?")) == "No")
            return;

        if(g_project->removeThing("Company", currentItem->text())) {
            m_list->takeItem(m_list->row(currentItem));
            delete currentItem;
        }
    }
}

void Companies::onProjectLoad()
{
    updateList();
}
