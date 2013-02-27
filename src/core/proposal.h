#ifndef PROPOSAL_H
#define PROPOSAL_H

#include <QtGui>
#include "thing.h"

class ProposalItem;

class Proposal : public Thing
{
public:
    enum State {
        STATE_PENDING,
        STATE_SENT,
        STATE_ACCEPTED,
        STATE_DECLINED
    };

    Proposal();
    ~Proposal();

    QString getName() { return "Proposal"; }
    QString getMainKey() { return "reference"; }

    void save(QSettings& settings);
    void load(QSettings& settings);

    void addItem(ProposalItem *item, int index = -1);
    int getItemId(ProposalItem *item);
    const QVector<ProposalItem*>& getItems() { return m_items; }
    void removeItem(int id);

public slots:
    void onSet(const QString& key, const QVariant& value);

private:
    QVector<ProposalItem*> m_items;
};

class ProposalItem : public Thing
{
public:
    ProposalItem() {
        m_parent = NULL;
    }

    QString getName() { return "ProposalItem"; }
    QString getMainKey() { return "id"; }
    int getId() { return m_parent->getItemId(this); }

    void setParent(Proposal *parent) { m_parent = parent; }

private:
    Proposal *m_parent;
};

Q_DECLARE_METATYPE(Proposal*)
Q_DECLARE_METATYPE(ProposalItem*)

#endif // PROPOSAL_H
