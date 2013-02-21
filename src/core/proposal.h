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

    void save(QSettings& settings);
    void load(QSettings& settings);

    void addItem(ProposalItem *item);
    int getItemId(ProposalItem *item);
    const QVector<ProposalItem*>& getItems() { return m_items; }
    void removeItem(int id);

private:
    State m_state;
    QString m_reference;
    QString m_description;
    QString m_client;
    QDate m_date;
    QString m_template;
    QVector<ProposalItem*> m_items;
};

class ProposalItem : public Thing
{
public:
    ProposalItem() {
        set("price", 0);
        set("amount", 0);
    }

    int getId() { return m_parent->getItemId(this); }

    void setParent(Proposal *parent);

private:
    Proposal *m_parent;
};

#endif // PROPOSAL_H
