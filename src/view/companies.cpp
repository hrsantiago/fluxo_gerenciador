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
    company->setName(a[0]);
    company->setAddress(a[1]);
    company->setCity(a[2]);
    company->setState(a[3]);
    company->setZipcode(a[4]);
    company->setTelephone(a[5]);

    if(!g_project->addCompany(company)) {
        qCritical() << tr("A company with this name already exists.");
        return QString();
    }

    updateList();
    return company->getName();
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
    const QVector<Company*>& companies = g_project->getCompanies();
    for(QVector<Company*>::const_iterator it = companies.constBegin(), end = companies.constEnd(); it != end; ++it) {
        QListWidgetItem *item = new QListWidgetItem((*it)->getName());
        m_list->addItem(item);

        if((*it)->getName() == currentName)
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
    Company *company = g_project->getCompany(name);
    if(company) {
        company->setName(m_nameWidget->text());
        company->setAddress(m_addressWidget->text());
        company->setCity(m_cityWidget->text());
        company->setState(m_stateWidget->text());
        company->setZipcode(m_zipcodeWidget->text());
        company->setTelephone(m_telephoneWidget->text());
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
        Company *company = g_project->getCompany(name);
        disconnectWidgets();
        m_nameWidget->setText(company->getName());
        m_addressWidget->setText(company->getAddress());
        m_cityWidget->setText(company->getCity());
        m_stateWidget->setText(company->getState());
        m_zipcodeWidget->setText(company->getZipcode());
        m_telephoneWidget->setText(company->getTelephone());
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

        if(g_project->removeCompany(currentItem->text())) {
            m_list->takeItem(m_list->row(currentItem));
            delete currentItem;
        }
    }
}

void Companies::onProjectLoad()
{
    updateList();
}