#include "people.h"
#include "tools.h"
#include <QPushButton>
#include <QHBoxLayout>
#include <QSettings>

People::People(QWidget *parent) :
    QWidget(parent)
{
    m_layout = new QGridLayout();
    m_layout->setContentsMargins(0, 0, 0, 0);
    setLayout(m_layout);

    m_list = new QListWidget;
    connect(m_list, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(onListItemChanged(QListWidgetItem*,QListWidgetItem*)));
    m_layout->addWidget(m_list, 1, 0);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    m_layout->addLayout(buttonsLayout, 2, 0);

    QPushButton *addButton = new QPushButton(tr("Add"));
    connect(addButton, SIGNAL(clicked()), this, SLOT(onAddButtonClicked()));
    buttonsLayout->addWidget(addButton);

    QPushButton *removeButton = new QPushButton(tr("Remove"));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(onRemoveButtonClicked()));
    buttonsLayout->addWidget(removeButton);

    load();
}

void People::save()
{
    QSettings settings("people.ini", QSettings::IniFormat);
    settings.clear();

    settings.beginGroup("People");
    for(int i = 0; i < m_people.size(); ++i) {
        settings.beginGroup(QString("Person%1").arg(i));
        settings.setValue("name", m_people[i].name);
        settings.setValue("gender", m_people[i].gender);
        settings.setValue("telephone", m_people[i].telephone);
        settings.setValue("email", m_people[i].email);
        settings.endGroup();
    }
    settings.endGroup();
}

void People::load()
{
    QSettings settings("people.ini", QSettings::IniFormat);

    settings.beginGroup("People");
    QStringList childGroups = settings.childGroups();
    for(QStringList::iterator it = childGroups.begin(), end = childGroups.end(); it != end; ++it) {
        settings.beginGroup(*it);
        Person person;
        person.name = settings.value("name").toString();
        person.gender = settings.value("gender").toInt();
        person.telephone = settings.value("telephone").toString();
        person.email = settings.value("email").toString();
        m_people.push_back(person);
        settings.endGroup();
    }
    settings.endGroup();

    updateList();
}

void People::updateList()
{
    QModelIndex currentIndex = m_list->currentIndex();
    int count = m_list->count();

    m_list->clear();
    for(QVector<Person>::iterator it = m_people.begin(), end = m_people.end(); it != end; ++it)
        m_list->addItem((*it).name);

    m_list->sortItems();

    if(m_list->count() == count)
        m_list->setCurrentIndex(currentIndex);
    else if(m_list->count() > 0)
        m_list->setCurrentRow(0);
}

void People::onListItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{

}

void People::onAddButtonClicked()
{
    QMap<int, QString> defaultData;
    defaultData[0] = "QLabel|";
    defaultData[2] = QString("QComboBox|%1|%2").arg(tr("Male")).arg(tr("Female"));

    QStringList a = Tools::requestDataFromUser(tr("Add Person"), QString(tr("Personal and contact data,"
                                                                            "Name,"
                                                                            "Gender,"
                                                                            "Telephone,"
                                                                            "Email")).split(","), defaultData);
    if(!Tools::isRequestedDataValid(a))
        return;

    Person person;
    person.name = a[0];
    person.gender = a[1].split("|")[1].toInt();
    person.telephone = a[2];
    person.email = a[3];
    m_people.push_back(person);

    updateList();
}

void People::onRemoveButtonClicked()
{
    if(m_list->currentItem()) {
        if(Tools::requestYesNoFromUser(tr("Remove Person"), tr("Do you really want to remove this person?")) == "No")
            return;

        m_list->removeItemWidget(m_list->currentItem());
        delete m_list->currentItem();

        save();
    }
}
