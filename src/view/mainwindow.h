#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "events.h"
#include "contracts.h"
#include "proposals.h"
#include "people.h"
#include "companies.h"
#include "headers.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void closeEvent(QCloseEvent *);

    void setLanguage(const QString& language);
    void selectThing(Thing *thing);

    Events *getEvents() { return m_events; }
    Contracts *getContracts() { return m_contracts; }
    Proposals *getProposals() { return m_proposals; }
    People *getPeople() { return m_people; }
    Companies *getCompanies() { return m_companies; }

private:
    void saveSettings();

    QAction *m_saveAction;
    QAction *m_saveAsAction;
    QAction *m_englishAction;
    QAction *m_portugueseAction;
    QTranslator *m_translator;
    QSettings m_settings;

    QAction *m_eventsAction;
    QAction *m_contractsAction;
    QAction *m_proposalsAction;
    QAction *m_peopleAction;
    QAction *m_companiesAction;

    QWidget *m_void;
    Events *m_events;
    Contracts *m_contracts;
    Proposals *m_proposals;
    People *m_people;
    Companies *m_companies;

private slots:
    void backup();
    void newFile();
    void openFile();
    void saveFile();
    void saveFileAs();
    void findThing();

    void onProjectUpdate();
    void onLanguageChanged();
    void onViewChanged();
};

extern MainWindow *g_mainWindow;

#endif // MAINWINDOW_H
