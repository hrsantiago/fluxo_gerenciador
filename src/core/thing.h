#ifndef THING_H
#define THING_H

#include "headers.h"

class Event;

class Thing : public QObject
{
    Q_OBJECT

public:
    Thing(const QString& type);
    ~Thing();

    void save(QSettings& settings);
    void load(QSettings& settings);

    void set(const QString& key, const QVariant& value, bool fromUser = false);
    QVariant get(const QString& key, const QVariant& def = QVariant());
    void remove(const QString& key) { m_properties.remove(key); }

    int getInt(const QString& key, const QVariant& def = QVariant()) { return get(key, def).toInt(); }
    QString getString(const QString& key, const QVariant& def = QVariant()) { return get(key, def).toString(); }
    QDate getDate(const QString& key, const QVariant& def = QVariant()) { return get(key, def).toDate(); }

    void setParent(Thing *thing) { m_parent = thing; }
    Thing *getParent() { return m_parent; }

    void copy(Thing *other);

    void addChild(Thing *thing, int index = -1);
    int getChildIndex(Thing *thing);
    const QVector<Thing*>& getChildren() { return m_children; }
    QVector<Thing*> getChildren(const QString& type);
    bool removeChild(Thing *thing);
    bool removeChild(int index);
    void clearChildren();
    void clearChildren(const QString& type);

public slots:
    void onSet(const QString&, const QVariant&);

private:
    QMap<QString, QVariant> m_properties;

    Thing *m_parent;
    QVector<Thing*> m_children;

};

Q_DECLARE_METATYPE(Thing*)

#endif // THING_H
