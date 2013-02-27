#ifndef COMPANY_H
#define COMPANY_H

#include <QtGui>
#include "thing.h"

class Company : public Thing
{
public:
    Company();

    QString getName() { return "Company"; }
    QString getMainKey() { return "name"; }

};

Q_DECLARE_METATYPE(Company*)

#endif // COMPANY_H
