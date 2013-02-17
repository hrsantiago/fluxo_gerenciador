#include "template.h"
#include "project.h"

Template::Template()
{
}

void Template::setName(const QString& name)
{
    if(name != m_name) {
        m_name = name;
        g_project->setSaved(false);
    }
}

void Template::setDescription(const QString& description)
{
    if(description != m_description) {
        m_description = description;
        g_project->setSaved(false);
    }
}

void Template::setHeader(const QString& header)
{
    if(header != m_header) {
        m_header = header;
        g_project->setSaved(false);
    }
}

void Template::setFooter(const QString& footer)
{
    if(footer != m_footer) {
        m_footer = footer;
        g_project->setSaved(false);
    }
}

void Template::setBase(const QString& base)
{
    if(base != m_base) {
        m_base = base;
        g_project->setSaved(false);
    }
}
