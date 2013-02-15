#include "proposal.h"
#include "project.h"

Proposal::Proposal()
{
    m_state = STATE_PENDING;
}

void Proposal::setState(State state)
{
    if(state != m_state) {
        m_state = state;
        g_project->setSaved(false);
    }
}

void Proposal::setReference(const QString& reference)
{
    if(reference != m_reference) {
        m_reference = reference;
        g_project->setSaved(false);
    }
}

void Proposal::setDescription(const QString& description)
{
    if(description != m_description) {
        m_description = description;
        g_project->setSaved(false);
    }
}

void Proposal::setClient(const QString& client)
{
    if(client != m_client) {
        m_client = client;
        g_project->setSaved(false);
    }
}

void Proposal::setDate(const QDate& date)
{
    if(date != m_date) {
        m_date = date;
        g_project->setSaved(false);
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
    //g_project->setSaved(false);
}

void ProposalItem::setDescription(const QString& description)
{
    if(description != m_description) {
        m_description = description;
        g_project->setSaved(false);
    }
}

void ProposalItem::setUnit(const QString& unit)
{
    if(unit != m_unit) {
        m_unit = unit;
        g_project->setSaved(false);
    }
}

void ProposalItem::setPrice(float price)
{
    if(price != m_price) {
        m_price = price;
        g_project->setSaved(false);
    }
}

void ProposalItem::setAmount(int amount)
{
    if(amount != m_amount) {
        m_amount = amount;
        g_project->setSaved(false);
    }
}
