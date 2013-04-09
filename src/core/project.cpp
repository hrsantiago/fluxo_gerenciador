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

    for(int i = 0; i < m_things.size(); ++i)
        delete m_things[i];
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

    settings.beginGroup("Things");
    for(int i = 0; i < m_things.size(); ++i) {
        settings.beginGroup(QString("Thing%1").arg(i));
        m_things[i]->save(settings);
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

    settings.beginGroup("Things");
    QStringList things = settings.childGroups();
    for(int i = 0; i < things.size(); ++i) {
        settings.beginGroup(things[i]);
        Thing *thing = new Thing(settings.value("type").toString());
        thing->load(settings);
        m_things.push_back(thing);
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

bool Project::addThing(Thing *thing)
{
    QString mainKey = thing->getString("mainKey");
    for(int i = 0; i < m_things.size(); ++i) {
        if(m_things[i]->getString("type") == thing->getString("type") && m_things[i]->get(mainKey) == thing->get(mainKey)) {
            delete thing;
            return false;
        }
    }
    m_things.push_back(thing);
    setSaved(false);
    return true;
}

Thing *Project::getThing(const QString& type, const QVariant& mainValue)
{
    for(int i = 0; i < m_things.size(); ++i) {
        Thing *thing = m_things[i];
        if(thing->getString("type") == type && thing->get(thing->getString("mainKey")) == mainValue)
            return thing;
    }
    return NULL;
}

const QVector<Thing*> Project::getThings(const QString& type)
{
    QVector<Thing*> things;
    for(int i = 0; i < m_things.size(); ++i) {
        Thing *thing = m_things[i];
        if(thing->getString("type") == type)
            things.push_back(thing);
        things += thing->getChildren(type);
    }
    return things;
}

bool Project::removeThing(const QString& type, const QVariant& mainValue)
{
    for(int i = 0; i < m_things.size(); ++i) {
        Thing *thing = m_things[i];
        if(thing->getString("type") == type && thing->get(thing->getString("mainKey")) == mainValue) {
            delete thing;
            m_things.erase(m_things.begin()+i);
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
