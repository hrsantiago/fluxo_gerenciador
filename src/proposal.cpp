#include "proposal.h"

Proposal::Proposal()
{
}

void Proposal::addItem(ProposalItem *item)
{
    item->setParent(this);
    m_items.push_back(item);
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
}
