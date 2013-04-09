#ifndef PROJECT_H
#define PROJECT_H

#include "headers.h"
#include "thing.h"

class Project : public QWidget
{
    Q_OBJECT

public:
    Project();
    ~Project();

    void create(const QString& companyName);
    void clean();

    void save(bool backup);
    void load(bool backup);

    void setName(const QString& name) { m_companyName = name; setSaved(false); }
    QString getName() { return m_companyName; }

    void setFilename(const QString& filename) { m_filename = filename; }
    QString getFilename() { return m_filename; }

    void setSaved(bool saved);
    bool isSaved() { return m_isSaved; }

    bool addThing(Thing *thing);
    Thing *getThing(const QString& type, const QVariant& mainValue);
    const QVector<Thing *> getThings(const QString& type);
    bool removeThing(const QString& type, const QVariant& mainValue);

private:
    QString m_companyName;
    QString m_filename;
    QVector<Thing*> m_things;

    bool m_isSaved;
    bool m_isLoading;

signals:
    void projectLoad();
    void projectUpdate();

};

extern Project *g_project;

#endif // PROJECT_H
