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

    for(QMap<QString, QVector<Thing*> >::Iterator it = m_things.begin(); it != m_things.end(); ++it) {
        for(int i = 0; i < it.value().size(); ++i)
            delete it.value()[i];
        it.value().clear();
    }
    m_things.clear();

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

    for(QMap<QString, QVector<Thing*> >::Iterator it = m_things.begin(); it != m_things.end(); ++it) {
        settings.beginGroup(it.key());
        for(int i = 0; i < it.value().size(); ++i) {
            settings.beginGroup(it.key() + QString::number(i));
            it.value()[i]->save(settings);
            settings.endGroup();
        }
        settings.endGroup();
    }

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

    QStringList thingsGroups = settings.childGroups();
    for(int i = 0; i < thingsGroups.size(); ++i) {
        settings.beginGroup(thingsGroups[i]);
        QStringList things = settings.childGroups();
        for(int j = 0; j < things.size(); ++j) {
            settings.beginGroup(things[j]);

            Thing *thing = createThing(thingsGroups[i]);
            if(thing) {
                thing->load(settings);
                m_things[thingsGroups[i]].push_back(thing);
            }

            settings.endGroup();
        }
        settings.endGroup();
    }

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

Thing *Project::createThing(const QString& name)
{
    Thing *thing = NULL;
    if(name == "Event")
        thing = new Event;
    else if(name == "Contract")
        thing = new Contract;
    else if(name == "Proposal")
        thing = new Proposal;
    else if(name == "Person")
        thing = new Person;
    else if(name == "Company")
        thing = new Company;
    else if(name == "Template")
        thing = new Template;
    return thing;
}

bool Project::addThing(Thing *thing)
{
    QString mainKey = thing->getMainKey();
    QVector<Thing*>& things = m_things[thing->getName()];
    for(int i = 0; i < things.size(); ++i) {
        if(things[i]->get(mainKey) == thing->get(mainKey)) {
            delete thing;
            return false;
        }
    }
    things.push_back(thing);
    setSaved(false);
    return true;
}

Thing *Project::getThing(const QString& name, const QVariant& mainValue)
{
    QVector<Thing*> things = m_things[name];
    for(int i = 0; i < things.size(); ++i) {
        Thing *thing = things[i];
        if(thing->get(thing->getMainKey()) == mainValue)
            return thing;
    }
    return NULL;
}

const QVector<Thing*>& Project::getThings(const QString& name)
{
    if(name == "Event") {
        // TODO: gather events from everywhere
        return m_things[name];
    }
    else
        return m_things[name];
}

bool Project::removeThing(const QString& name, const QVariant& mainValue)
{
    QVector<Thing*>& things = m_things[name];
    for(int i = 0; i < things.size(); ++i) {
        Thing *thing = things[i];
        if(thing->get(thing->getMainKey()) == mainValue) {
            delete thing;
            things.erase(things.begin()+i);
            setSaved(false);
            return true;
        }
    }
    return false;
}

void Project::setSaved(bool saved)
{
    if(!m_isLoading) {
        m_isSaved = saved;
        emit projectUpdate();
    }
}
