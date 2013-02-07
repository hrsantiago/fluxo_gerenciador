#ifndef PROJECT_H
#define PROJECT_H

#include <QWidget>
#include <QString>

class Project : public QWidget
{
    Q_OBJECT

public:
    Project();

    void reset();

    void save();
    void load();

    void setName(const QString& name) { m_name = name; setSaved(false); }
    QString getName() { return m_name; }

    void setFilename(const QString& filename) { m_filename = filename; }
    QString getFilename() { return m_filename; }

    void setSaved(bool saved) { m_isSaved = saved; emit projectUpdate(); }
    bool isSaved() { return m_isSaved; }

private:
    QString m_name;
    QString m_filename;
    bool m_isSaved;

signals:
    void projectUpdate();

};

extern Project *g_project;

#endif // PROJECT_H
