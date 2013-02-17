#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <QtGui>

class Template
{
public:
    Template();

    void setName(const QString& name);
    QString getName() { return m_name; }

    void setDescription(const QString& description);
    QString getDescription() { return m_description; }

    void setHeader(const QString& header);
    QString getHeader() { return m_header; }

    void setFooter(const QString& footer);
    QString getFooter() { return m_footer; }

    void setBase(const QString& base);
    QString getBase() { return m_base; }

private:
    QString m_name;
    QString m_description;
    QString m_header;
    QString m_footer;
    QString m_base;
};

#endif // TEMPLATE_H
