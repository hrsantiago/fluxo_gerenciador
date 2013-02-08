#include "people.h"
#include "tools.h"
#include <QPushButton>
#include <QHBoxLayout>
#include <QSettings>
#include <QSplitter>
#include <QLabel>
#include <QDebug>

People::People(QWidget *parent) :
    QWidget(parent)
{
    // Left Widget
    QWidget *left = new QWidget;
    QGridLayout *gridLayout = new QGridLayout();
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
    m_layout = new QGridLayout();
    right->setLayout(m_layout);

    int row = 0;
    m_layout->addWidget(new QLabel(tr("Name:")), row++, 0);
    m_layout->addWidget(new QLabel(tr("Gender:")), row++, 0);
    m_layout->addWidget(new QLabel(tr("Telephone:")), row++, 0);
    m_layout->addWidget(new QLabel(tr("Email:")), row++, 0);
    m_layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), row++, 0, 1, 2);

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

    load();
}

void People::save()
{
    QSettings settings("people.ini", QSettings::IniFormat);
    settings.clear();

    settings.beginGroup("People");
    for(int i = 0; i < m_people.size(); ++i) {
        settings.beginGroup(QString("Person%1").arg(i));
        settings.setValue("name", m_people[i].lineEdit[Person::NAME]->text());
        settings.setValue("gender", m_people[i].comboBox[Person::GENDER]->currentIndex());
        settings.setValue("telephone", m_people[i].lineEdit[Person::TELEPHONE]->text());
        settings.setValue("email", m_people[i].lineEdit[Person::EMAIL]->text());
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
        person.lineEdit[Person::NAME]->setText(settings.value("name").toString());
        person.comboBox[Person::GENDER]->setCurrentIndex(settings.value("gender").toInt());
        person.lineEdit[Person::TELEPHONE]->setText(settings.value("telephone").toString());
        person.lineEdit[Person::EMAIL]->setText(settings.value("email").toString());
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
        m_list->addItem((*it).lineEdit[Person::NAME]->text());
    m_list->sortItems();

    if(m_list->count() == count)
        m_list->setCurrentIndex(currentIndex);
    else if(m_list->count() > 0)
        m_list->setCurrentRow(0);
}

void People::onListItemChanged(QListWidgetItem *current, QListWidgetItem *)
{
    for(int i = 0; i < m_people.size(); ++i) {
        if(current && current->text() == m_people[i].lineEdit[Person::NAME]->text()) {
            for(int j = 0; j < Person::LAST; ++j) {
                QWidget *widget = Tools::getLayoutWidget(m_layout, j, 1);
                if(widget) {
                    widget->hide();
                    widget->setParent(0);
                }
            }
            m_layout->addWidget(m_people[i].lineEdit[Person::NAME], 0, 1);
            m_layout->addWidget(m_people[i].comboBox[Person::GENDER], 1, 1);
            m_layout->addWidget(m_people[i].lineEdit[Person::TELEPHONE], 2, 1);
            m_layout->addWidget(m_people[i].lineEdit[Person::EMAIL], 3, 1);
            m_people[i].lineEdit[Person::NAME]->setVisible(true);
            m_people[i].comboBox[Person::GENDER]->setVisible(true);
            m_people[i].lineEdit[Person::TELEPHONE]->setVisible(true);
            m_people[i].lineEdit[Person::EMAIL]->setVisible(true);
        }
    }
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

    for(int i = 0; i < m_people.size(); ++i) {
        if(a[0] == m_people[i].lineEdit[Person::NAME]->text()) {
            qCritical() << tr("A person with this name already exists.");
            return;
        }
    }

    Person person;
    person.lineEdit[Person::NAME]->setText(a[0]);
    person.comboBox[Person::GENDER]->setCurrentIndex(a[1].split("|")[1].toInt());
    person.lineEdit[Person::TELEPHONE]->setText(a[2]);
    person.lineEdit[Person::EMAIL]->setText(a[3]);
    m_people.push_back(person);

    updateList();
}

void People::onRemoveButtonClicked()
{
    if(m_list->currentItem()) {
        if(Tools::requestYesNoFromUser(tr("Remove Person"), tr("Do you really want to remove this person?")) == "No")
            return;

        QListWidgetItem *currentItem = m_list->currentItem();
        for(int i = 0; i < m_people.size(); ++i) {
            if(currentItem->text() == m_people[i].lineEdit[Person::NAME]->text()) {
                m_people.erase(m_people.begin()+i);
                break;
            }
        }
        m_list->removeItemWidget(currentItem);

        updateList();
    }
}
