#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTranslator>
#include <QSettings>
#include "contracts.h"
#include "proposals.h"
#include "people.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow() {}

    void closeEvent(QCloseEvent *);

    void setLanguage(const QString& language);

private:
    QAction *m_saveAction;
    QAction *m_saveAsAction;
    QAction *m_englishAction;
    QAction *m_portugueseAction;
    QTranslator *m_translator;
    QSettings m_settings;

    QAction *m_contractsAction;
    QAction *m_proposalsAction;
    QAction *m_peopleAction;

    Contracts *m_contracts;
    Proposals *m_proposals;
    People *m_people;

private slots:
    void newFile();
    void openFile();
    void saveFile();
    void saveFileAs();
    void findThing();

    void onProjectUpdate();
    void onLanguageChanged();
    void onViewChanged();
};

#endif // MAINWINDOW_H
