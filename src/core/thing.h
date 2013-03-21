#ifndef THING_H
#define THING_H

#include "headers.h"

class Event;

class Thing : public QObject
{
    Q_OBJECT

public:
    Thing();
    virtual ~Thing();

    virtual QString getName() = 0;
    virtual QString getMainKey() = 0;

    virtual void save(QSettings& settings);
    virtual void load(QSettings& settings);

    void set(const QString& key, const QVariant& value, bool fromUser = false);
    QVariant get(const QString& key, const QVariant& def = QVariant());

    int getInt(const QString& key, const QVariant& def = QVariant()) { return get(key, def).toInt(); }
    QString getString(const QString& key, const QVariant& def = QVariant()) { return get(key, def).toString(); }
    QDate getDate(const QString& key, const QVariant& def = QVariant()) { return get(key, def).toDate(); }

    void addEvent(Event *event);
    const QVector<Event*>& getEvents() { return m_events; }
    void clearEvents();

public slots:
    virtual void onSet(const QString&, const QVariant&) {}

private:
    QMap<QString, QVariant> m_properties;
    QVector<Event*> m_events;

};

Q_DECLARE_METATYPE(Thing*)

#endif // THING_H
