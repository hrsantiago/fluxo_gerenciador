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
        settings.setValue("description", m_proposals[i]->description);
        settings.setValue("start", m_proposals[i]->start);
        settings.setValue("end", m_proposals[i]->end);

        for(int j = 0; j < proposal->items.size(); ++j) {
            ProposalItem *item = proposal->items[j];
            settings.beginGroup(QString("ProposalItem%1").arg(j));
            settings.setValue("value", item->value);
            settings.setValue("start", item->start);
            settings.setValue("end", item->end);
            settings.endGroup();
        }
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
        proposal->description = settings.value("description").toString();
        proposal->start = settings.value("start").toDate();
        proposal->end = settings.value("end").toDate();

        QStringList items = settings.childGroups();
        for(QStringList::iterator it = items.begin(), end = items.end(); it != end; ++it) {
            ProposalItem *item = new ProposalItem;
            settings.beginGroup(*it);
            item->value = settings.value("value").toString();
            item->start = settings.value("start").toDate();
            item->end = settings.value("end").toDate();
            settings.endGroup();
            proposal->items.push_back(item);
        }
        settings.endGroup();
        m_proposals.push_back(proposal);
    }
    settings.endGroup();

    settings.endGroup();

    setSaved(true);
    emit projectLoad();
}

void Project::addProposal(Proposal *proposal)
{
    m_proposals.push_back(proposal);
    setSaved(false);
}

void Project::removeProposal()
{
    setSaved(false);
}
