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

    QString getMainKey() { return "reference"; }

    void save(QSettings& settings);
    void load(QSettings& settings);

    void addItem(ProposalItem *item);
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
    ProposalItem() {}

    QString getMainKey() { return "id"; }
    int getId() { return m_parent->getItemId(this); }

    void setParent(Proposal *parent);

private:
    Proposal *m_parent;
};

#endif // PROPOSAL_H
