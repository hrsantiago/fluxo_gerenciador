#ifndef COMPANIES_H
#define COMPANIES_H

#include <QtGui>

class Companies : public QWidget
{
    Q_OBJECT
public:
    explicit Companies(QWidget *parent = 0);

    QString createCompany();

private:
    void connectWidgets();
    void disconnectWidgets();
    void updateList();
    void updateCompany(const QString& name);

    QLineEdit *m_nameWidget;
    QLineEdit *m_addressWidget;
    QLineEdit *m_cityWidget;
    QLineEdit *m_stateWidget;
    QLineEdit *m_zipcodeWidget;
    QLineEdit *m_telephoneWidget;

    QListWidget *m_list;

private slots:
    void onListItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void onWidgetChanged();
    void onAddButtonClicked();
    void onRemoveButtonClicked();

    void onProjectLoad();
};

#endif // COMPANIES_H
