#ifndef PERSON_H
#define PERSON_H

#include <QtGui>
#include "thing.h"

class Person : public Thing
{
public:
    enum Gender {
        GENDER_MALE,
        GENDER_FEMALE
    };

    Person();

};

#endif // PERSON_H
