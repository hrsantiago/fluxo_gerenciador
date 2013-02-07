#ifndef PEOPLE_H
#define PEOPLE_H

#include <QWidget>
#include <QGridLayout>
#include <QListWidget>

struct Person
{
    QString name;
    int gender;
    QString telephone;
    QString email;
};

class People : public QWidget
{
    Q_OBJECT
public:
    explicit People(QWidget *parent = 0);

    void save();
    void load();
    
private:
    void updateList();

    QGridLayout *m_layout;
    QListWidget *m_list;
    QVector<Person> m_people;

private slots:
    void onListItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void onAddButtonClicked();
    void onRemoveButtonClicked();

};

#endif // PEOPLE_H
