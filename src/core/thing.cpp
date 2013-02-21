#include "thing.h"
#include "project.h"

Thing::Thing()
{
}

void Thing::save(QSettings& settings)
{
    for(QMap<QString, QVariant>::iterator it = m_properties.begin(); it != m_properties.end(); ++it)
        settings.setValue(it.key(), it.value());
}

void Thing::load(QSettings& settings)
{
    QStringList keys = settings.childKeys();
    for(QStringList::iterator it = keys.begin(), end = keys.end(); it != end; ++it)
        set(*it, settings.value(*it));
}

void Thing::set(const QString& key, const QVariant& value)
{
    if(!m_properties.contains(key) || m_properties[key] != value) {
        m_properties[key] = value;
        g_project->setSaved(false);
    }
}

QVariant Thing::get(const QString& key, const QVariant& def)
{
    if(m_properties.contains(key))
        return m_properties[key];
    return def;
}
