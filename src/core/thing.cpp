#include "thing.h"
#include "project.h"
#include "event.h"

Thing::Thing()
{
}

Thing::~Thing()
{
    clearEvents();
}

void Thing::save(QSettings& settings)
{
    for(QMap<QString, QVariant>::iterator it = m_properties.begin(); it != m_properties.end(); ++it)
        settings.setValue(it.key(), it.value());

    settings.beginGroup("Events");
    for(int i = 0; i < m_events.size(); ++i) {
        settings.beginGroup(QString("Event%1").arg(i));
        m_events[i]->save(settings);
        settings.endGroup();
    }
    settings.endGroup();
}

void Thing::load(QSettings& settings)
{
    QStringList keys = settings.childKeys();
    for(QStringList::iterator it = keys.begin(), end = keys.end(); it != end; ++it)
        set(*it, settings.value(*it));

    settings.beginGroup("Events");
    QStringList events = settings.childGroups();
    for(QStringList::iterator it = events.begin(), end = events.end(); it != end; ++it) {
        Event *event = new Event();
        settings.beginGroup(*it);
        event->load(settings);
        settings.endGroup();
        m_events.push_back(event);
    }
    settings.endGroup();
}

void Thing::set(const QString& key, const QVariant& value, bool fromUser)
{
    if(!m_properties.contains(key) || m_properties[key] != value) {
        m_properties[key] = value;
        g_project->setSaved(false);
        if(fromUser)
            onSet(key, value);
    }
}

QVariant Thing::get(const QString& key, const QVariant& def)
{
    if(m_properties.contains(key))
        return m_properties[key];
    return def;
}

void Thing::addEvent(Event *event)
{
    m_events.push_back(event);
    g_project->setSaved(false);
}

void Thing::clearEvents()
{
    for(int i = 0; i < m_events.size(); ++i)
        delete m_events[i];
    m_events.clear();
    g_project->setSaved(false);
}
