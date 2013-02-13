#ifndef PROJECT_H
#define PROJECT_H

#include <QtGui>
#include "proposal.h"
#include "person.h"

class Project : public QWidget
{
    Q_OBJECT

public:
    Project();

    void reset();

    void save();
    void load();

    void setName(const QString& name) { m_name = name; setSaved(false); }
    QString getName() { return m_name; }

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

    void setSaved(bool saved) { m_isSaved = saved; emit projectUpdate(); }
    bool isSaved() { return m_isSaved; }

private:
    QString m_name;
    QString m_filename;
    QVector<Proposal*> m_proposals;
    QVector<Person*> m_people;
    bool m_isSaved;

signals:
    void projectLoad();
    void projectUpdate();

};

extern Project *g_project;

#endif // PROJECT_H
