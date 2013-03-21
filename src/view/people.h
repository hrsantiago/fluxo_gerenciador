#ifndef PEOPLE_H
#define PEOPLE_H

#include "headers.h"

class People : public QWidget
{
    Q_OBJECT
public:
    explicit People(QWidget *parent = 0);
    
private:
    void connectWidgets();
    void disconnectWidgets();
    void updateList();
    void updatePerson(const QString& name);

    QLineEdit *m_nameWidget;
    QComboBox *m_genderWidget;
    QLineEdit *m_telephoneWidget;
    QLineEdit *m_emailWidget;

    QListWidget *m_list;

private slots:
    void onListItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void onWidgetChanged();
    void onAddButtonClicked();
    void onRemoveButtonClicked();

    void onProjectLoad();
};

#endif // PEOPLE_H
