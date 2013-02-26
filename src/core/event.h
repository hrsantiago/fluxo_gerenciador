#ifndef EVENT_H
#define EVENT_H

#include "thing.h"

class Event : public Thing
{
public:
    Event();

    QString getMainKey() { return "description"; }

};

#endif // EVENT_H
