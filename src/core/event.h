#ifndef EVENT_H
#define EVENT_H

#include "thing.h"

class Event : public Thing
{
    Q_OBJECT

public:
    Event();

    QString getName() { return "Event"; }
    QString getMainKey() { return "description"; }

    void setParent(Thing *parent) { m_parent = parent; }
    Thing *getParent() { return m_parent; }

private:
    Thing *m_parent;
};

Q_DECLARE_METATYPE(Event*)

#endif // EVENT_H
