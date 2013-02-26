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

    for(int i = 0; i < m_events.size(); ++i)
        delete m_events[i];
    m_events.clear();

    for(int i = 0; i < m_proposals.size(); ++i)
        delete m_proposals[i];
    m_proposals.clear();

    for(int i = 0; i < m_people.size(); ++i)
        delete m_people[i];
    m_people.clear();

    for(int i = 0; i < m_companies.size(); ++i)
        delete m_companies[i];
    m_companies.clear();

    for(int i = 0; i < m_templates.size(); ++i)
        delete m_templates[i];
    m_templates.clear();

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

    saveProposals(settings);
    savePeople(settings);
    saveCompanies(settings);
    saveTemplates(settings);

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

void Project::saveProposals(QSettings& settings)
{
    settings.beginGroup("Proposals");
    for(int i = 0; i < m_proposals.size(); ++i) {
        settings.beginGroup(QString("Proposal%1").arg(i));
        m_proposals[i]->save(settings);
        settings.endGroup();
    }
    settings.endGroup();
}

void Project::savePeople(QSettings& settings)
{
    settings.beginGroup("People");
    for(int i = 0; i < m_people.size(); ++i) {
        settings.beginGroup(QString("Person%1").arg(i));
        m_people[i]->save(settings);
        settings.endGroup();
    }
    settings.endGroup();
}

void Project::saveCompanies(QSettings& settings)
{
    settings.beginGroup("Companies");
    for(int i = 0; i < m_companies.size(); ++i) {
        settings.beginGroup(QString("Company%1").arg(i));
        m_companies[i]->save(settings);
        settings.endGroup();
    }
    settings.endGroup();
}

void Project::saveTemplates(QSettings& settings)
{
    settings.beginGroup("Templates");
    for(int i = 0; i < m_templates.size(); ++i) {
        settings.beginGroup(QString("Template%1").arg(i));
        m_templates[i]->save(settings);
        settings.endGroup();
    }
    settings.endGroup();
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

    loadProposals(settings);
    loadPeople(settings);
    loadCompanies(settings);
    loadTemplates(settings);

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

void Project::loadProposals(QSettings& settings)
{
    settings.beginGroup("Proposals");
    QStringList proposals = settings.childGroups();
    for(QStringList::iterator it = proposals.begin(), end = proposals.end(); it != end; ++it) {
        Proposal *proposal = new Proposal;
        settings.beginGroup(*it);
        proposal->load(settings);
        settings.endGroup();
        m_proposals.push_back(proposal);
    }
    settings.endGroup();
}

void Project::loadPeople(QSettings& settings)
{
    settings.beginGroup("People");
    QStringList people = settings.childGroups();
    for(QStringList::iterator it = people.begin(), end = people.end(); it != end; ++it) {
        settings.beginGroup(*it);
        Person *person = new Person;
        person->load(settings);
        m_people.push_back(person);
        settings.endGroup();
    }
    settings.endGroup();
}

void Project::loadCompanies(QSettings& settings)
{
    settings.beginGroup("Companies");
    QStringList companies = settings.childGroups();
    for(QStringList::iterator it = companies.begin(), end = companies.end(); it != end; ++it) {
        settings.beginGroup(*it);
        Company *company = new Company;
        company->load(settings);
        m_companies.push_back(company);
        settings.endGroup();
    }
    settings.endGroup();
}

void Project::loadTemplates(QSettings& settings)
{
    settings.beginGroup("Templates");
    QStringList companies = settings.childGroups();
    for(QStringList::iterator it = companies.begin(), end = companies.end(); it != end; ++it) {
        settings.beginGroup(*it);
        Template *tp = new Template;
        tp->load(settings);
        m_templates.push_back(tp);
        settings.endGroup();
    }
    settings.endGroup();
}

bool Project::addEvent(Event *event)
{
    for(int i = 0; i < m_events.size(); ++i) {
        if(m_events[i]->getString("description") == event->getString("description")) {
            delete event;
            return false;
        }
    }
    m_events.push_back(event);
    setSaved(false);
    return true;
}

Event *Project::getEvent(const QString& description)
{
    for(int i = 0; i < m_events.size(); ++i) {
        if(m_events[i]->getString("description") == description)
            return m_events[i];
    }
    return NULL;
}

QVector<Event*> Project::getEvents()
{
    QVector<Event*> events;

    events += (m_events);

    for(int i = 0; i < m_proposals.size(); ++i)
        events += m_proposals[i]->getEvents();

    for(int i = 0; i < m_people.size(); ++i)
        events += m_people[i]->getEvents();

    for(int i = 0; i < m_companies.size(); ++i)
        events += m_companies[i]->getEvents();

    for(int i = 0; i < m_templates.size(); ++i)
        events += m_templates[i]->getEvents();

    return events;
}

void Project::removeEvent(const QString& description)
{
    for(int i = 0; i < m_events.size(); ++i) {
        if(m_events[i]->getString("description") == description) {
            m_events.erase(m_events.begin()+i);
            setSaved(false);
            break;
        }
    }
}

bool Project::addProposal(Proposal *proposal)
{
    for(int i = 0; i < m_proposals.size(); ++i) {
        if(m_proposals[i]->getString("reference") == proposal->getString("reference")) {
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
        if(m_proposals[i]->getString("reference") == reference)
            return m_proposals[i];
    }
    return NULL;
}

void Project::removeProposal(const QString& reference)
{
    for(int i = 0; i < m_proposals.size(); ++i) {
        if(m_proposals[i]->getString("reference") == reference) {
            m_proposals.erase(m_proposals.begin()+i);
            setSaved(false);
            break;
        }
    }
}

bool Project::addPerson(Person *person)
{
    for(int i = 0; i < m_people.size(); ++i) {
        if(m_people[i]->getString("name") == person->getString("name")) {
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
        if(m_people[i]->getString("name") == name)
            return m_people[i];
    }
    return NULL;
}

void Project::removePerson(const QString& name)
{
    for(int i = 0; i < m_people.size(); ++i) {
        if(m_people[i]->getString("name") == name) {
            m_people.erase(m_people.begin()+i);
            setSaved(false);
            break;
        }
    }
}

bool Project::addCompany(Company *company)
{
    for(int i = 0; i < m_companies.size(); ++i) {
        if(m_companies[i]->getString("name") == company->getString("name")) {
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
        if(m_companies[i]->getString("name") == name)
            return m_companies[i];
    }
    return NULL;
}

bool Project::removeCompany(const QString& name)
{
    if(name == m_companyName) {
        qCritical() << tr("It's not possible to remove this company.");
        return false;
    }
    for(int i = 0; i < m_companies.size(); ++i) {
        if(m_companies[i]->getString("name") == name) {
            m_companies.erase(m_companies.begin()+i);
            setSaved(false);
            break;
        }
    }
    return true;
}

bool Project::addTemplate(Template *tp)
{
    for(int i = 0; i < m_templates.size(); ++i) {
        if(m_templates[i]->getString("name") == tp->getString("name")) {
            delete tp;
            return false;
        }
    }
    m_templates.push_back(tp);
    setSaved(false);
    return true;
}

Template *Project::getTemplate(const QString& name)
{
    for(int i = 0; i < m_templates.size(); ++i) {
        if(m_templates[i]->getString("name") == name)
            return m_templates[i];
    }
    return NULL;
}

bool Project::removeTemplate(const QString& name)
{
    for(int i = 0; i < m_templates.size(); ++i) {
        if(m_templates[i]->getString("name") == name) {
            m_templates.erase(m_templates.begin()+i);
            setSaved(false);
            break;
        }
    }
    return true;
}

void Project::setSaved(bool saved)
{
    if(!m_isLoading) {
        m_isSaved = saved;
        emit projectUpdate();
    }
}
