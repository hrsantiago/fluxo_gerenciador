#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <QtGui>
#include "thing.h"

class Template : public Thing
{
public:
    Template();

    QString getMainKey() { return "name"; }
};

#endif // TEMPLATE_H
