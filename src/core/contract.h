#ifndef CONTRACT_H
#define CONTRACT_H

#include "thing.h"

#include "headers.h"
#include "thing.h"

class ContractItem;

class Contract : public Thing
{
    Q_OBJECT

public:
    enum State {
        STATE_PENDING,
        STATE_SENT,
        STATE_ACCEPTED,
        STATE_DECLINED
    };

    Contract();
    ~Contract();

    QString getName() { return "Contract"; }
    QString getMainKey() { return "reference"; }

    void save(QSettings& settings);
    void load(QSettings& settings);

    void addItem(ContractItem *item, int index = -1);
    int getItemId(ContractItem *item);
    const QVector<ContractItem*>& getItems() { return m_items; }
    void removeItem(int id);

public slots:
    void onSet(const QString& key, const QVariant& value);

private:
    QVector<ContractItem*> m_items;
};

class ContractItem : public Thing
{
    Q_OBJECT

public:
    ContractItem() {
        m_parent = NULL;
    }

    QString getName() { return "ContractItem"; }
    QString getMainKey() { return "id"; }
    int getId() { return m_parent->getItemId(this); }

    void setParent(Contract *parent) { m_parent = parent; }

private:
    Contract *m_parent;
};

Q_DECLARE_METATYPE(Contract*)
Q_DECLARE_METATYPE(ContractItem*)

#endif // CONTRACT_H
