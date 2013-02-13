#ifndef COMPANIES_H
#define COMPANIES_H

#include <QtGui>

struct Company
{
    enum {
        NAME,
        ADDRESS,
        CITY,
        STATE,
        ZIPCODE,
        TELEPHONE,
        LINEEDIT_LAST
    };
    enum {
        LAST = LINEEDIT_LAST
    };

    Company() {
        for(int i = 0; i < LINEEDIT_LAST; ++i)
            lineEdit[i] = new QLineEdit;
    }

    QLineEdit *lineEdit[LINEEDIT_LAST];
};

class Companies : public QWidget
{
    Q_OBJECT
public:
    explicit Companies(QWidget *parent = 0);

    void save();
    void load();

private:
    void updateList();

    QGridLayout *m_layout;
    QListWidget *m_list;
    QVector<Company> m_companies;

private slots:
    void onListItemChanged(QListWidgetItem *current, QListWidgetItem *);
    void onAddButtonClicked();
    void onRemoveButtonClicked();

};

#endif // COMPANIES_H
