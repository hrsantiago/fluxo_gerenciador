#ifndef PROPOSAL_H
#define PROPOSAL_H

#include <QtGui>

class ProposalItem;

class Proposal
{
public:
    enum State {
        STATE_PENDING,
        STATE_SENT,
        STATE_ACCEPTED,
        STATE_DECLINED
    };

    Proposal();

    void setState(State state);
    State getState() { return m_state; }

    void setReference(const QString& reference);
    QString getReference() { return m_reference; }

    void setDescription(const QString& description);
    QString getDescription() { return m_description; }

    void setClient(const QString& client);
    QString getClient() { return m_client; }

    void setDate(const QDate& date);
    QDate getDate() { return m_date; }

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
    QVector<ProposalItem*> m_items;
};

class ProposalItem
{
public:
    ProposalItem() {
        m_price = 0;
        m_amount = 0;
    }

    int getId() { return m_parent->getItemId(this); }

    void setParent(Proposal *parent);

    void setDescription(const QString& description);
    QString getDescription() { return m_description; }

    void setUnit(const QString& unit);
    QString getUnit() { return m_unit; }

    void setPrice(float price);
    float getPrice() { return m_price; }

    void setAmount(int amount);
    int getAmount() { return m_amount; }

private:
    Proposal *m_parent;
    QString m_description;
    QString m_unit;
    float m_price;
    int m_amount;
};

#endif // PROPOSAL_H
