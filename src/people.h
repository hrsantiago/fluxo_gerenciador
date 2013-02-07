#ifndef PEOPLE_H
#define PEOPLE_H

#include <QWidget>
#include <QGridLayout>
#include <QListWidget>
#include <QComboBox>
#include <QLineEdit>

struct Person
{
    enum {
        NAME,
        TELEPHONE,
        EMAIL,
        LINEEDIT_LAST
    };
    enum {
        GENDER,
        COMBOBOX_LAST
    };
    enum {
        LAST = LINEEDIT_LAST + COMBOBOX_LAST
    };

    Person() {
        for(int i = 0; i < LINEEDIT_LAST; ++i)
            lineEdit[i] = new QLineEdit;
        for(int i = 0; i < COMBOBOX_LAST; ++i)
            comboBox[i] = new QComboBox;
        comboBox[GENDER]->addItem(QObject::tr("Male"));
        comboBox[GENDER]->addItem(QObject::tr("Female"));
    }

    QLineEdit *lineEdit[LINEEDIT_LAST];
    QComboBox *comboBox[COMBOBOX_LAST];
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
    void onListItemChanged(QListWidgetItem *current, QListWidgetItem *);
    void onAddButtonClicked();
    void onRemoveButtonClicked();

};

#endif // PEOPLE_H
