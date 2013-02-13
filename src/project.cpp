#include "project.h"
#include <QSettings>

Project *g_project = NULL;

Project::Project()
{
    g_project = this;
    reset();
}

void Project::reset()
{
    m_name = QString();
    m_filename = QString();
    m_isSaved = true;
}

void Project::save()
{
    QSettings settings(m_filename, QSettings::IniFormat);
    settings.clear();

    settings.beginGroup("Project");
    settings.setValue("name", m_name);

    settings.beginGroup("Proposals");
    for(int i = 0; i < m_proposals.size(); ++i) {
        Proposal *proposal = m_proposals[i];
        settings.beginGroup(QString("Proposal%1").arg(i));
        settings.setValue("reference", m_proposals[i]->getReference());
        settings.setValue("description", m_proposals[i]->getDescription());
        settings.setValue("date", m_proposals[i]->getDate());

        const QVector<ProposalItem*>& items = proposal->getItems();
        for(int j = 0; j < items.size(); ++j) {
            ProposalItem *item = items[j];
            settings.beginGroup(QString("ProposalItem%1").arg(j));
            settings.setValue("description", item->getDescription());
            settings.setValue("unit", item->getUnit());
            settings.setValue("price", item->getPrice());
            settings.setValue("amount", item->getAmount());
            settings.endGroup();
        }
        settings.endGroup();
    }
    settings.endGroup();

    settings.beginGroup("People");
    for(int i = 0; i < m_people.size(); ++i) {
        settings.beginGroup(QString("Person%1").arg(i));
        settings.setValue("name", m_people[i]->getName());
        settings.setValue("gender", m_people[i]->getGender());
        settings.setValue("telephone", m_people[i]->getTelephone());
        settings.setValue("email", m_people[i]->getEmail());
        settings.endGroup();
    }
    settings.endGroup();

    settings.endGroup();

    setSaved(true);
}

void Project::load()
{
    QSettings settings(m_filename, QSettings::IniFormat);

    settings.beginGroup("Project");
    m_name = settings.value("name").toString();

    settings.beginGroup("Proposals");
    QStringList proposals = settings.childGroups();
    for(QStringList::iterator it = proposals.begin(), end = proposals.end(); it != end; ++it) {
        Proposal *proposal = new Proposal;
        settings.beginGroup(*it);
        proposal->setReference(settings.value("reference").toString());
        proposal->setDescription(settings.value("description").toString());
        proposal->setDate(settings.value("date").toDate());

        QStringList items = settings.childGroups();
        for(QStringList::iterator it = items.begin(), end = items.end(); it != end; ++it) {
            ProposalItem *item = new ProposalItem;
            settings.beginGroup(*it);
            item->setDescription(settings.value("description").toString());
            item->setUnit(settings.value("unit").toString());
            item->setPrice(settings.value("price").toFloat());
            item->setAmount(settings.value("amount").toInt());
            settings.endGroup();
            proposal->addItem(item);
        }
        settings.endGroup();
        m_proposals.push_back(proposal);
    }
    settings.endGroup();

    settings.beginGroup("People");
    QStringList childGroups = settings.childGroups();
    for(QStringList::iterator it = childGroups.begin(), end = childGroups.end(); it != end; ++it) {
        settings.beginGroup(*it);
        Person *person = new Person;
        person->setName(settings.value("name").toString());
        person->setGender((Person::Gender)settings.value("gender").toInt());
        person->setTelephone(settings.value("telephone").toString());
        person->setEmail(settings.value("email").toString());
        m_people.push_back(person);
        settings.endGroup();
    }
    settings.endGroup();

    settings.endGroup();

    setSaved(true);
    emit projectLoad();
}

bool Project::addProposal(Proposal *proposal)
{
    for(int i = 0; i < m_proposals.size(); ++i) {
        if(m_proposals[i]->getReference() == proposal->getReference()) {
            delete proposal;
            return false;
        }
    }
    m_proposals.push_back(proposal);
    setSaved(false);
    return true;
}

Proposal *Project::getProposal(const QString& reference)
{
    for(int i = 0; i < m_proposals.size(); ++i) {
        if(m_proposals[i]->getReference() == reference)
            return m_proposals[i];
    }
    return NULL;
}

void Project::removeProposal(const QString& reference)
{
    for(int i = 0; i < m_proposals.size(); ++i) {
        if(m_proposals[i]->getReference() == reference) {
            m_proposals.erase(m_proposals.begin()+i);
            setSaved(false);
            break;
        }
    }
}

bool Project::addPerson(Person *person)
{
    for(int i = 0; i < m_people.size(); ++i) {
        if(m_people[i]->getName() == person->getName()) {
            delete person;
            return false;
        }
    }
    m_people.push_back(person);
    setSaved(false);
    return true;
}

Person *Project::getPerson(const QString& name)
{
    for(int i = 0; i < m_people.size(); ++i) {
        if(m_people[i]->getName() == name)
            return m_people[i];
    }
    return NULL;
}

void Project::removePerson(const QString& name)
{
    for(int i = 0; i < m_people.size(); ++i) {
        if(m_people[i]->getName() == name) {
            m_people.erase(m_people.begin()+i);
            setSaved(false);
            break;
        }
    }
}
