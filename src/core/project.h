#ifndef PROJECT_H
#define PROJECT_H

#include <QtGui>
#include "proposal.h"
#include "person.h"
#include "company.h"
#include "template.h"

class Project : public QWidget
{
    Q_OBJECT

public:
    Project();
    ~Project();

    void create(const QString& companyName);
    void clean();

    void save(bool backup);
    void load(bool backup);

    void setName(const QString& name) { m_companyName = name; setSaved(false); }
    QString getName() { return m_companyName; }

    void setFilename(const QString& filename) { m_filename = filename; }
    QString getFilename() { return m_filename; }

    bool addProposal(Proposal *proposal);
    Proposal *getProposal(const QString& reference);
    const QVector<Proposal*>& getProposals() { return m_proposals; }
    void removeProposal(const QString &reference);

    bool addPerson(Person *person);
    Person *getPerson(const QString& name);
    const QVector<Person*>& getPeople() { return m_people; }
    void removePerson(const QString& name);

    bool addCompany(Company *company);
    Company *getCompany(const QString& name);
    const QVector<Company*>& getCompanies() { return m_companies; }
    bool removeCompany(const QString& name);

    bool addTemplate(Template *tp);
    Template *getTemplate(const QString& name);
    const QVector<Template*>& getTemplates() { return m_templates; }
    bool removeTemplate(const QString& name);

    void setSaved(bool saved);
    bool isSaved() { return m_isSaved; }

private:
    void saveProposals(QSettings &settings);
    void savePeople(QSettings& settings);
    void saveCompanies(QSettings& settings);
    void saveTemplates(QSettings& settings);

    void loadProposals(QSettings& settings);
    void loadPeople(QSettings& settings);
    void loadCompanies(QSettings& settings);
    void loadTemplates(QSettings& settings);

    QString m_companyName;
    QString m_filename;
    QVector<Proposal*> m_proposals;
    QVector<Person*> m_people;
    QVector<Company*> m_companies;
    QVector<Template*> m_templates;
    bool m_isSaved;
    bool m_isLoading;

signals:
    void projectLoad();
    void projectUpdate();

};

extern Project *g_project;

#endif // PROJECT_H
