#include "person.h"
#include "project.h"

Person::Person()
{
    m_gender = GENDER_MALE;
}

void Person::setName(const QString& name)
{
    if(name != m_name) {
        m_name = name;
        g_project->setSaved(false);
    }
}

void Person::setGender(Gender gender)
{
    if(gender != m_gender) {
        m_gender = gender;
        g_project->setSaved(false);
    }
}

void Person::setTelephone(const QString& telephone)
{
    if(telephone != m_telephone) {
        m_telephone = telephone;
        g_project->setSaved(false);
    }
}

void Person::setEmail(const QString& email)
{
    if(email != m_email) {
        m_email = email;
        g_project->setSaved(false);
    }
}
