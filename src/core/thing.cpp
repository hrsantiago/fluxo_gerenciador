#include "thing.h"
#include "project.h"
#include "event.h"
#include "const.h"
#include "view/mainwindow.h"

Thing::Thing(const QString& type)
{
    set("type", type);

    if(type == "Event")
        set("mainKey", "description");
    else if(type == "Company")
        set("mainKey", "name");
    else if(type == "Contract")
        set("mainKey", "reference");
    else if(type == "Proposal")
        set("mainKey", "reference");
    else if(type == "ItemList")
        set("mainKey", "name");
    else if(type == "Item")
        set("mainKey", "description");
    else if(type == "Person")
        set("mainKey", "name");
    else if(type == "Template")
        set("mainKey", "name");
}

Thing::~Thing()
{
    clearChildren();
}

void Thing::save(QSettings& settings)
{
    for(QMap<QString, QVariant>::iterator it = m_properties.begin(); it != m_properties.end(); ++it)
        settings.setValue(it.key(), it.value());

    settings.beginGroup("Children");
    for(int i = 0; i < m_children.size(); ++i) {
        settings.beginGroup(QString("Child%1").arg(i));
        m_children[i]->save(settings);
        settings.endGroup();
    }
    settings.endGroup();
}

void Thing::load(QSettings& settings)
{
    QStringList keys = settings.childKeys();
    for(QStringList::iterator it = keys.begin(), end = keys.end(); it != end; ++it)
        set(*it, settings.value(*it));

    settings.beginGroup("Children");
    QStringList children = settings.childGroups();
    for(QStringList::iterator it = children.begin(), end = children.end(); it != end; ++it) {
        settings.beginGroup(*it);
        Thing *thing = new Thing(settings.value("type").toString());
        thing->load(settings);
        thing->setParent(this);
        settings.endGroup();
        m_children.push_back(thing);
    }
    settings.endGroup();
}

void Thing::set(const QString& key, const QVariant& value, bool fromUser)
{
    if(fromUser && (key == "type" || key == "mainKey")) { // This is protected!
        qCritical() << "Trying to set key '" << key << "'.";
        return;
    }

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

void Thing::copy(Thing *other)
{
    for(QMap<QString, QVariant>::iterator it = other->m_properties.begin(); it != other->m_properties.end(); ++it)
        if(it.key() != "type" && it.key() != "mainKey")
            m_properties[it.key()] = it.value();

    for(int i = 0; i < other->m_children.size(); ++i) {
        Thing *child = new Thing(other->m_children[i]->getString("type"));
        child->copy(other->m_children[i]);
        child->setParent(this);
        m_children.push_back(child);
    }
}

bool Thing::addChild(Thing *thing, int index)
{
    if(getChild(thing->getString("type"), thing->getString(thing->getString("mainKey")))) {
        delete thing;
        return false;
    }

    if(index == -1)
        m_children.push_back(thing);
    else
        m_children.insert(m_children.begin() + index, thing);
    thing->setParent(this);
    g_project->setSaved(false);
    return true;
}

int Thing::getChildIndex(Thing *thing)
{
    for(int i = 0; i < m_children.size(); ++i) {
        if(m_children[i] == thing)
            return i;
    }
    return -1;
}

Thing *Thing::getChild(const QString& type, const QString& mainKey)
{
    for(int i = 0; i < m_children.size(); ++i) {
        Thing *child = m_children[i];
        if(child->getString("type") == type && child->getString(child->getString("mainKey")) == mainKey)
            return child;
    }
    return NULL;
}

QVector<Thing*> Thing::getChildren(const QString& type)
{
    QVector<Thing*> things;
    for(int i = 0; i < m_children.size(); ++i) {
        if(m_children[i]->getString("type") == type)
            things.push_back(m_children[i]);
    }
    return things;
}

bool Thing::moveChild(int fromIndex, int toIndex)
{
    if(fromIndex < 0 || fromIndex >= m_children.size() || toIndex >= m_children.size())
        return false;

    Thing *child = m_children[fromIndex];
    m_children.erase(m_children.begin()+fromIndex);

    if(toIndex <= -1)
        m_children.push_back(child);
    else
        m_children.insert(m_children.begin()+toIndex, child);

    return true;
}

bool Thing::removeChild(Thing *thing)
{
    for(QVector<Thing*>::iterator it = m_children.begin(); it != m_children.end(); ++it) {
        if(*it == thing) {
            delete thing;
            m_children.erase(it);
            g_project->setSaved(false);
            return true;
        }
    }
    return false;
}

bool Thing::removeChild(int index)
{
    if(index >= 0 && index < m_children.size()) {
        delete m_children[index];
        m_children.erase(m_children.begin()+index);
        return true;
    }
    return false;
}

void Thing::clearChildren()
{
    for(int i = 0; i < m_children.size(); ++i)
        delete m_children[i];
    m_children.clear();
}

void Thing::clearChildren(const QString& type)
{
    bool saved = true;
    for(QVector<Thing*>::iterator it = m_children.begin(); it != m_children.end();) {
        Thing *thing = *it;
        if(thing->getString("type") == type) {
            delete thing;
            it = m_children.erase(it);
            saved = false;
        }
        else
            ++it;
    }
    if(!saved)
        g_project->setSaved(false);
}

void Thing::onSet(const QString& key, const QVariant& value)
{
    // Todo: move this somewhere else.

    if(getString("type") == "Proposal") {
        clearChildren("Event");

        if(key == "state" && value.toInt() == STATE_PENDING) {
            Thing *event = new Thing("Event");
            event->set("id", "waiting_send");
            event->set("identifier", get("reference"));
            event->set("date", QDate::currentDate().addDays(1));
            addChild(event);
        }
        else if(key == "state" && value.toInt() == STATE_SENT) {
            Thing *event = new Thing("Event");
            event->set("id", "waiting_response");
            event->set("identifier", get("reference"));
            event->set("date", QDate::currentDate().addDays(10));
            addChild(event);
        }
        else if(key == "state" && value.toInt() == STATE_ACCEPTED) {
            // Convert this proposal to a contract
            Thing *contract = new Thing("Contract");
            contract->copy(this);
            contract->set("state", STATE_IN_PROGRESS);
            contract->remove("template");

            g_project->addThing(contract);
            g_mainWindow->selectThing(contract);
        }
    }
}
