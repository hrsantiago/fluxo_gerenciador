#include "project.h"

Project *g_project = NULL;

Project::Project()
{
    g_project = this;
    m_isLoading = false;
    clean();
}

Project::~Project()
{
    clean();
}

void Project::create(const QString& companyName)
{
    setName(companyName);
    emit projectLoad();
}

void Project::clean()
{
    m_companyName = QString();
    m_filename = QString();
    m_isSaved = true;

    for(int i = 0; i < m_proposals.size(); ++i)
        delete m_proposals[i];
    m_proposals.clear();

    for(int i = 0; i < m_people.size(); ++i)
        delete m_people[i];
    m_people.clear();

    for(int i = 0; i < m_companies.size(); ++i)
        delete m_companies[i];
    m_companies.clear();

    emit projectUpdate();
}

void Project::save(bool backup)
{
    QString filename = m_filename;
    if(backup)
        filename = QFileInfo(QFileInfo(QSettings().fileName()).absolutePath(), "project.fm").absoluteFilePath();

    QSettings settings(filename, QSettings::IniFormat);
    settings.clear();

    settings.beginGroup("Project");
    settings.setValue("name", m_companyName);

    settings.beginGroup("Proposals");
    for(int i = 0; i < m_proposals.size(); ++i) {
        Proposal *proposal = m_proposals[i];
        settings.beginGroup(QString("Proposal%1").arg(i));
        settings.setValue("reference", m_proposals[i]->getReference());
        settings.setValue("description", m_proposals[i]->getDescription());
        settings.setValue("client", m_proposals[i]->getClient());
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

    settings.beginGroup("Companies");
    for(int i = 0; i < m_companies.size(); ++i) {
        settings.beginGroup(QString("Company%1").arg(i));
        settings.setValue("name", m_companies[i]->getName());
        settings.setValue("address", m_companies[i]->getAddress());
        settings.setValue("city", m_companies[i]->getCity());
        settings.setValue("state", m_companies[i]->getState());
        settings.setValue("zipcode", m_companies[i]->getZipcode());
        settings.setValue("telephone", m_companies[i]->getTelephone());
        settings.endGroup();
    }
    settings.endGroup();

    settings.endGroup();

    if(backup) {
        settings.beginGroup("Backup");
        settings.setValue("saved", isSaved());
        settings.setValue("filename", m_filename);
        settings.endGroup();
    }
    else
        setSaved(true);
}

void Project::load(bool backup)
{
    QString filename = m_filename;
    clean();
    m_filename = filename;

    if(backup)
        filename = QFileInfo(QFileInfo(QSettings().fileName()).absolutePath(), "project.fm").absoluteFilePath();

    m_isLoading = true;
    QSettings settings(filename, QSettings::IniFormat);

    settings.beginGroup("Project");
    m_companyName = settings.value("name").toString();

    settings.beginGroup("Proposals");
    QStringList proposals = settings.childGroups();
    for(QStringList::iterator it = proposals.begin(), end = proposals.end(); it != end; ++it) {
        Proposal *proposal = new Proposal;
        settings.beginGroup(*it);
        proposal->setReference(settings.value("reference").toString());
        proposal->setDescription(settings.value("description").toString());
        proposal->setClient(settings.value("client").toString());
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
    QStringList people = settings.childGroups();
    for(QStringList::iterator it = people.begin(), end = people.end(); it != end; ++it) {
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

    settings.beginGroup("Companies");
    QStringList companies = settings.childGroups();
    for(QStringList::iterator it = companies.begin(), end = companies.end(); it != end; ++it) {
        settings.beginGroup(*it);
        Company *company = new Company;
        company->setName(settings.value("name").toString());
        company->setAddress(settings.value("address").toString());
        company->setCity(settings.value("city").toString());
        company->setState(settings.value("state").toString());
        company->setZipcode(settings.value("zipcode").toString());
        company->setTelephone(settings.value("telephone").toString());
        m_companies.push_back(company);
        settings.endGroup();
    }
    settings.endGroup();

    settings.endGroup();

    m_isLoading = false;

    if(backup) {
        settings.beginGroup("Backup");
        setSaved(settings.value("saved").toBool());
        m_filename = settings.value("filename").toString();
        settings.endGroup();
    }
    else
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

bool Project::addCompany(Company *company)
{
    for(int i = 0; i < m_companies.size(); ++i) {
        if(m_companies[i]->getName() == company->getName()) {
            delete company;
            return false;
        }
    }
    m_companies.push_back(company);
    setSaved(false);
    return true;
}

Company *Project::getCompany(const QString& name)
{
    for(int i = 0; i < m_companies.size(); ++i) {
        if(m_companies[i]->getName() == name)
            return m_companies[i];
    }
    return NULL;
}

void Project::removeCompany(const QString& name)
{
    if(name == m_companyName) {
        qWarning() << tr("Can't remove the project's company.");
        return;
    }
    for(int i = 0; i < m_companies.size(); ++i) {
        if(m_companies[i]->getName() == name) {
            m_companies.erase(m_companies.begin()+i);
            setSaved(false);
            break;
        }
    }
}

void Project::setSaved(bool saved)
{
    if(!m_isLoading) {
        m_isSaved = saved;
        emit projectUpdate();
    }
}
