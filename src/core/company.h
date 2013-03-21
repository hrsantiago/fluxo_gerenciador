#ifndef COMPANY_H
#define COMPANY_H

#include "headers.h"
#include "thing.h"

class Company : public Thing
{
    Q_OBJECT

public:
    Company();

    QString getName() { return "Company"; }
    QString getMainKey() { return "name"; }

};

Q_DECLARE_METATYPE(Company*)

#endif // COMPANY_H
