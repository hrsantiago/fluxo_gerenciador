#ifndef PERSON_H
#define PERSON_H

#include "headers.h"
#include "thing.h"

class Person : public Thing
{
    Q_OBJECT

public:
    enum Gender {
        GENDER_MALE,
        GENDER_FEMALE
    };

    Person();

    QString getName() { return "Person"; }
    QString getMainKey() { return "name"; }

};

Q_DECLARE_METATYPE(Person*)

#endif // PERSON_H
