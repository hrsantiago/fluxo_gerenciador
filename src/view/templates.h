#ifndef TEMPLATES_H
#define TEMPLATES_H

#include <QtGui>
#include "core/proposal.h"
#include "core/template.h"

class Templates : public QWidget
{
    Q_OBJECT
public:
    Templates(QWidget *parent = NULL);

    void print(Proposal *proposal);

private:
    void connectWidgets();
    void disconnectWidgets();
    void updateList();
    void updateTemplate(const QString& name);

    Template *m_currentTemplate;

    QLineEdit *m_nameWidget;
    QLineEdit *m_descriptionWidget;
    QLineEdit *m_headerWidget;
    QLineEdit *m_footerWidget;
    QLineEdit *m_baseWidget;

    QListWidget *m_list;

private slots:
    void onListItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void onWidgetChanged();
    void onAddButtonClicked();
    void onRemoveButtonClicked();
    void onSearchButtonClicked();

    void printPreview(QPrinter *printer);
    void onProjectLoad();
};

#endif // TEMPLATES_H
