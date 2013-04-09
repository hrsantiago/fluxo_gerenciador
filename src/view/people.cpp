#include "people.h"
#include "core/project.h"
#include "tools.h"

People::People(QWidget *parent) :
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
    m_genderWidget = new QComboBox();
    m_genderWidget->addItem(tr("Male"));
    m_genderWidget->addItem(tr("Female"));
    m_telephoneWidget = new QLineEdit();
    m_emailWidget = new QLineEdit();
    connectWidgets();

    int row = 0;
    gridLayout->addWidget(new QLabel(tr("Name:")), row, 0);
    gridLayout->addWidget(m_nameWidget, row++, 1);
    gridLayout->addWidget(new QLabel(tr("Gender:")), row, 0);
    gridLayout->addWidget(m_genderWidget, row++, 1);
    gridLayout->addWidget(new QLabel(tr("Telephone:")), row, 0);
    gridLayout->addWidget(m_telephoneWidget, row++, 1);
    gridLayout->addWidget(new QLabel(tr("Email:")), row, 0);
    gridLayout->addWidget(m_emailWidget, row++, 1);
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

void People::connectWidgets()
{
    connect(m_nameWidget, SIGNAL(textChanged(QString)), this, SLOT(onWidgetChanged()));
    connect(m_genderWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(onWidgetChanged()));
    connect(m_telephoneWidget, SIGNAL(textChanged(QString)), this, SLOT(onWidgetChanged()));
    connect(m_emailWidget, SIGNAL(textChanged(QString)), this, SLOT(onWidgetChanged()));
}

void People::disconnectWidgets()
{
    disconnect(m_nameWidget, SIGNAL(textChanged(QString)), this, SLOT(onWidgetChanged()));
    disconnect(m_genderWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(onWidgetChanged()));
    disconnect(m_telephoneWidget, SIGNAL(textChanged(QString)), this, SLOT(onWidgetChanged()));
    disconnect(m_emailWidget, SIGNAL(textChanged(QString)), this, SLOT(onWidgetChanged()));
}

void People::updateList()
{
    QString currentName;
    if(m_list->currentItem())
        currentName = m_list->currentItem()->text();

    QListWidgetItem *currentItem = NULL;

    disconnect(m_list, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(onListItemChanged(QListWidgetItem*,QListWidgetItem*)));
    m_list->clear();
    const QVector<Thing*>& people = g_project->getThings("Person");
    for(QVector<Thing*>::const_iterator it = people.constBegin(), end = people.constEnd(); it != end; ++it) {
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

void People::updatePerson(const QString& name)
{
    Thing *person = g_project->getThing("Person", name);
    if(person) {
        person->set("name", m_nameWidget->text());
        person->set("gender", m_genderWidget->currentIndex());
        person->set("telephone", m_telephoneWidget->text());
        person->set("email", m_emailWidget->text());
    }
}

void People::onWidgetChanged()
{
    if(m_list->currentItem()) {
        QString name = m_list->currentItem()->text();
        updatePerson(name);

        if(sender() && sender() == m_nameWidget) {
            disconnect(m_list, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(onListItemChanged(QListWidgetItem*,QListWidgetItem*)));
            m_list->currentItem()->setText(m_nameWidget->text());
            m_list->sortItems();
            connect(m_list, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(onListItemChanged(QListWidgetItem*,QListWidgetItem*)));
        }
    }
    else {
        qCritical() << tr("You need to create a person before editing.");
    }
}

void People::onListItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if(previous) {
        QString name = previous->text();
        updatePerson(name);
    }
    if(current) {
        QString name = current->text();
        Thing *person = g_project->getThing("Person", name);
        disconnectWidgets();
        m_nameWidget->setText(person->getString("name"));
        m_genderWidget->setCurrentIndex(person->getInt("gender"));
        m_telephoneWidget->setText(person->getString("telephone"));
        m_emailWidget->setText(person->getString("email"));
        connectWidgets();
    }
}

void People::onAddButtonClicked()
{
    QMap<int, QString> defaultData;
    defaultData[0] = "QLabel|";
    defaultData[2] = QString("QComboBox|%1|%2").arg(tr("Male")).arg(tr("Female"));

    QStringList a = Tools::requestDataFromUser(tr("Add Person"), tr("Personal and contact data,"
                                                                    "Name,"
                                                                    "Gender,"
                                                                    "Telephone,"
                                                                    "Email").split(","), defaultData);
    if(!Tools::isRequestedDataValid(a))
        return;

    Thing *person = new Thing("Person");
    person->set("name", a[0]);
    person->set("gender", a[1].split("|")[1].toInt());
    person->set("telephone", a[2]);
    person->set("email", a[3]);

    if(g_project->addThing(person))
        updateList();
    else
        qCritical() << tr("A person with this name already exists.");
}

void People::onRemoveButtonClicked()
{
    QListWidgetItem *currentItem = m_list->currentItem();
    if(currentItem) {
        if(Tools::requestYesNoFromUser(tr("Remove Person"), tr("Do you really want to remove this person?")) == "No")
            return;

        g_project->removeThing("Person", currentItem->text());
        m_list->takeItem(m_list->row(currentItem));
        delete currentItem;
    }
}

void People::onProjectLoad()
{
    updateList();
}
