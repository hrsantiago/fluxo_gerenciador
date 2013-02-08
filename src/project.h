#ifndef PROJECT_H
#define PROJECT_H

#include <QtGui>

struct ProposalItem
{
    enum State {
        STATE_PENDING,
        STATE_DONE,
        STATE_DELAYED,
        STATE_LAST
    };

    QDate start;
    QDate end;
    QString value;
    State state;
};

struct Proposal
{
    QString description;
    QDate start;
    QDate end;
    QVector<ProposalItem*> items;
};

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

    void addProposal(Proposal *proposal);
    const QVector<Proposal*>& getProposals() { return m_proposals; }
    void removeProposal();

    void setSaved(bool saved) { m_isSaved = saved; emit projectUpdate(); }
    bool isSaved() { return m_isSaved; }

private:
    QString m_name;
    QString m_filename;
    QVector<Proposal*> m_proposals;
    bool m_isSaved;

signals:
    void projectLoad();
    void projectUpdate();

};

extern Project *g_project;

#endif // PROJECT_H
