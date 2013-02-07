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
    settings.endGroup();

    setSaved(true);
}

void Project::load()
{
    QSettings settings(m_filename, QSettings::IniFormat);

    settings.beginGroup("Project");
    m_name = settings.value("name").toString();
    settings.endGroup();

    setSaved(true);
}
