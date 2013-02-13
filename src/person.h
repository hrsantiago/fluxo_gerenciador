#ifndef PERSON_H
#define PERSON_H

#include <QtGui>

class Person
{
public:
    enum Gender {
        GENDER_MALE,
        GENDER_FEMALE
    };

    Person();

    void setName(const QString& name) { m_name = name; }
    QString getName() { return m_name; }

    void setGender(Gender gender) { m_gender = gender; }
    Gender getGender() { return m_gender; }

    void setTelephone(const QString& telephone) { m_telephone = telephone; }
    QString getTelephone() { return m_telephone; }

    void setEmail(const QString& email) { m_email = email; }
    QString getEmail() { return m_email; }

private:
    QString m_name;
    Gender m_gender;
    QString m_telephone;
    QString m_email;
};

#endif // PERSON_H
