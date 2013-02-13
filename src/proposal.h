#ifndef PROPOSAL_H
#define PROPOSAL_H

#include <QtGui>

class ProposalItem;

class Proposal
{
public:
    Proposal();

    void setReference(const QString& reference) { m_reference = reference; }
    QString getReference() { return m_reference; }

    void setDescription(const QString& description) { m_description = description; }
    QString getDescription() { return m_description; }

    void setDate(const QDate& date) { m_date = date; }
    QDate getDate() { return m_date; }

    void addItem(ProposalItem *item);
    int getItemId(ProposalItem *item);
    const QVector<ProposalItem*>& getItems() { return m_items; }
    void removeItem(int id);

private:
    QString m_reference;
    QString m_description;
    QDate m_date;
    QVector<ProposalItem*> m_items;
};

class ProposalItem
{
public:
    int getId() { return m_parent->getItemId(this); }

    void setParent(Proposal *parent) { m_parent = parent; }

    void setDescription(const QString& description) { m_description = description; }
    QString getDescription() { return m_description; }

    void setUnit(const QString& unit) { m_unit = unit; }
    QString getUnit() { return m_unit; }

    void setPrice(float price) { m_price = price; }
    float getPrice() { return m_price; }

    void setAmount(int amount) { m_amount = amount; }
    int getAmount() { return m_amount; }

private:
    Proposal *m_parent;
    QString m_description;
    QString m_unit;
    float m_price;
    int m_amount;
};



#endif // PROPOSAL_H
