#include "company.h"
#include "project.h"

Company::Company()
{
}

void Company::setName(const QString& name)
{
    if(name != m_name) {
        m_name = name;
        g_project->setSaved(false);
    }
}

void Company::setAddress(const QString& address)
{
    if(address != m_address) {
        m_address = address;
        g_project->setSaved(false);
    }
}

void Company::setCity(const QString& city)
{
    if(city != m_city) {
        m_city = city;
        g_project->setSaved(false);
    }
}

void Company::setState(const QString& state)
{
    if(state != m_state) {
        m_state = state;
        g_project->setSaved(false);
    }
}

void Company::setZipcode(const QString& zipcode)
{
    if(zipcode != m_zipcode) {
        m_zipcode = zipcode;
        g_project->setSaved(false);
    }
}

void Company::setTelephone(const QString& telephone)
{
    if(telephone != m_telephone) {
        m_telephone = telephone;
        g_project->setSaved(false);
    }
}
