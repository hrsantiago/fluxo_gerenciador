#ifndef THING_H
#define THING_H

#include <QtGui>

class Thing
{
public:
    Thing();

    virtual void save(QSettings& settings);
    virtual void load(QSettings& settings);

    void set(const QString& key, const QVariant& value);
    QVariant get(const QString& key, const QVariant& def = QVariant());

    int getInt(const QString& key, const QVariant& def = QVariant()) { return get(key, def).toInt(); }
    QString getString(const QString& key, const QVariant& def = QVariant()) { return get(key, def).toString(); }
    QDate getDate(const QString& key, const QVariant& def = QVariant()) { return get(key, def).toDate(); }

private:
    QMap<QString, QVariant> m_properties;
};

#endif // THING_H
