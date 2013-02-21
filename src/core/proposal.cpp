#include "proposal.h"
#include "project.h"

Proposal::Proposal()
{
    set("state", STATE_PENDING);
}

void Proposal::save(QSettings& settings)
{
    Thing::save(settings);

    for(int i = 0; i < m_items.size(); ++i) {
        settings.beginGroup(QString("ProposalItem%1").arg(i));
        m_items[i]->save(settings);
        settings.endGroup();
    }
}

void Proposal::load(QSettings& settings)
{
    Thing::load(settings);

    QStringList items = settings.childGroups();
    for(QStringList::iterator it = items.begin(), end = items.end(); it != end; ++it) {
        ProposalItem *item = new ProposalItem;
        settings.beginGroup(*it);
        item->load(settings);
        settings.endGroup();
        addItem(item);
    }
}

void Proposal::addItem(ProposalItem *item)
{
    item->setParent(this);
    m_items.push_back(item);
    g_project->setSaved(false);
}

int Proposal::getItemId(ProposalItem *item)
{
    for(int i = 0; i < m_items.size(); ++i) {
        if(m_items[i] == item)
            return i;
    }
    return -1;
}

void Proposal::removeItem(int id)
{
    m_items.erase(m_items.begin()+id);
    g_project->setSaved(false);
}

void ProposalItem::setParent(Proposal *parent)
{
    m_parent = parent;
}
