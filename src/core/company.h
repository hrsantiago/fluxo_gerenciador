#ifndef COMPANY_H
#define COMPANY_H

#include <QtGui>
#include "thing.h"

class Company : public Thing
{
public:
    Company();

    QString getMainKey() { return "name"; }

};

#endif // COMPANY_H
