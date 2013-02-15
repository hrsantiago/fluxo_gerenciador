#ifndef COMPANY_H
#define COMPANY_H

#include <QtGui>

class Company
{
public:
    Company();

    void setName(const QString& name);
    QString getName() { return m_name; }

    void setAddress(const QString& address);
    QString getAddress() { return m_address; }

    void setCity(const QString& city);
    QString getCity() { return m_city; }

    void setState(const QString& state);
    QString getState() { return m_state; }

    void setZipcode(const QString& zipcode);
    QString getZipcode() { return m_zipcode; }

    void setTelephone(const QString& telephone);
    QString getTelephone() { return m_telephone; }

private:
    QString m_name;
    QString m_address;
    QString m_city;
    QString m_state;
    QString m_zipcode;
    QString m_telephone;
};

#endif // COMPANY_H
