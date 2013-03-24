#include "contract.h"
#include "project.h"

Contract::Contract()
{
}

Contract::~Contract()
{
    for(int i = 0; i < m_items.size(); ++i)
        delete m_items[i];
    m_items.clear();
}

void Contract::save(QSettings& settings)
{
    Thing::save(settings);

    settings.beginGroup("ProposalItems");
    for(int i = 0; i < m_items.size(); ++i) {
        settings.beginGroup(QString("ProposalItem%1").arg(i));
        m_items[i]->save(settings);
        settings.endGroup();
    }
    settings.endGroup();
}

void Contract::load(QSettings& settings)
{
    Thing::load(settings);

    settings.beginGroup("ProposalItems");
    QStringList items = settings.childGroups();
    for(QStringList::iterator it = items.begin(), end = items.end(); it != end; ++it) {
        ContractItem *item = new ContractItem;
        settings.beginGroup(*it);
        item->load(settings);
        settings.endGroup();
        addItem(item);
    }
    settings.endGroup();
}

void Contract::addItem(ContractItem *item, int index)
{
    if(index == -1)
        index = m_items.size();
    m_items.insert(m_items.begin()+index, item);
    item->setParent(this);
    g_project->setSaved(false);
}

int Contract::getItemId(ContractItem *item)
{
    for(int i = 0; i < m_items.size(); ++i) {
        if(m_items[i] == item)
            return i;
    }
    return -1;
}

void Contract::removeItem(int id)
{
    m_items.erase(m_items.begin()+id);
    g_project->setSaved(false);
}

void Contract::onSet(const QString& key, const QVariant& value)
{
    clearEvents();

    if(key == "state" && value.toInt() == STATE_PENDING) {
        Event *event = new Event;
        event->set("id", "waiting_send");
        event->set("identifier", get("reference"));
        event->set("date", QDate::currentDate().addDays(1));
        addEvent(event);
    }
    else if(key == "state" && value.toInt() == STATE_SENT) {
        Event *event = new Event;
        event->set("id", "waiting_response");
        event->set("identifier", get("reference"));
        event->set("date", QDate::currentDate().addDays(10));
        addEvent(event);
    }
    else if(key == "state" && value.toInt() == STATE_ACCEPTED) {
        // TODO
        // send client to contracts page
    }
}
