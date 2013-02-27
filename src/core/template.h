#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <QtGui>
#include "thing.h"

class Template : public Thing
{
public:
    Template();

    QString getName() { return "Template"; }
    QString getMainKey() { return "name"; }
};

Q_DECLARE_METATYPE(Template*)

#endif // TEMPLATE_H
