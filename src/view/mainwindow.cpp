#include "const.h"
#include "mainwindow.h"
#include "core/project.h"
#include "tools.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_translator = NULL;
    QString savedLanguage = m_settings.value("language", "portuguese").toString();
    setLanguage(savedLanguage);

    setWindowTitle(tr("Manager"));
    setMinimumSize(800, 600);
    setWindowIcon(QIcon("resources/icons/logo32.ico"));
    restoreGeometry(m_settings.value("mainWindowGeometry").toByteArray());

    QMenuBar *qMenuBar = new QMenuBar();
    setMenuBar(qMenuBar);

    // File
    QMenu *fileMenu = qMenuBar->addMenu(tr("File"));
    fileMenu->addAction(tr("New"), this, SLOT(newFile()), QKeySequence("Ctrl+N"));
    fileMenu->addAction(tr("Open..."), this, SLOT(openFile()), QKeySequence("Ctrl+O"));
    fileMenu->addSeparator();
    m_saveAction = fileMenu->addAction(tr("Save"), this, SLOT(saveFile()), QKeySequence("Ctrl+S"));
    m_saveAsAction = fileMenu->addAction(tr("Save as..."), this, SLOT(saveFileAs()), QKeySequence("Ctrl+Shift+S"));
    fileMenu->addSeparator();
    fileMenu->addAction(tr("Exit"), this, SLOT(close()), QKeySequence("Ctrl+Q"));

    // Edit
    QMenu *editMenu = qMenuBar->addMenu(tr("Edit"));
    editMenu->addAction(tr("Find"), this, SLOT(findThing()), QKeySequence("Ctrl+F"));

    // View
    QMenu *viewMenu = qMenuBar->addMenu(tr("View"));
    m_eventsAction = viewMenu->addAction(tr("Events"), this, SLOT(onViewChanged()), QKeySequence("F1"));
    m_eventsAction->setCheckable(true);
    m_contractsAction = viewMenu->addAction(tr("Contracts"), this, SLOT(onViewChanged()), QKeySequence("F2"));
    m_contractsAction->setCheckable(true);
    m_proposalsAction = viewMenu->addAction(tr("Proposals"), this, SLOT(onViewChanged()), QKeySequence("F3"));
    m_proposalsAction->setCheckable(true);
    m_peopleAction = viewMenu->addAction(tr("People"), this, SLOT(onViewChanged()), QKeySequence("F5"));
    m_peopleAction->setCheckable(true);
    m_companiesAction = viewMenu->addAction(tr("Companies"), this, SLOT(onViewChanged()), QKeySequence("F6"));
    m_companiesAction->setCheckable(true);

    QActionGroup *viewActionGroup = new QActionGroup(this);
    viewActionGroup->addAction(m_eventsAction);
    viewActionGroup->addAction(m_contractsAction);
    viewActionGroup->addAction(m_proposalsAction);
    viewActionGroup->addAction(m_peopleAction);
    viewActionGroup->addAction(m_companiesAction);
    m_eventsAction->setChecked(true);

    // Preferences
    QMenu *preferencesMenu = qMenuBar->addMenu(tr("Preferences"));
    QMenu *language = preferencesMenu->addMenu(tr("Language"));
    m_englishAction = language->addAction(tr("English"), this, SLOT(onLanguageChanged()));
    m_englishAction->setCheckable(true);
    m_portugueseAction = language->addAction(tr("Portuguese"), this, SLOT(onLanguageChanged()));
    m_portugueseAction->setCheckable(true);

    QActionGroup *actionGroup = new QActionGroup(this);
    actionGroup->addAction(m_englishAction);
    actionGroup->addAction(m_portugueseAction);
    if(savedLanguage == "en-US")
        m_englishAction->setChecked(true);
    else if(savedLanguage == "pt-BR")
        m_portugueseAction->setChecked(true);

    restoreState(m_settings.value("mainWindowState").toByteArray());

    g_project = new Project;
    connect(g_project, SIGNAL(projectUpdate()), this, SLOT(onProjectUpdate()));

    m_void = new QWidget();
    m_events = new Events();
    m_contracts = new Contracts();
    m_proposals = new Proposals();
    m_people = new People();
    m_companies = new Companies();

    g_project->load(true);

    onViewChanged();

    // Backup Save
    QTimer::singleShot(BACKUP_TIME, this, SLOT(backup()));
}

MainWindow::~MainWindow()
{
    if(m_translator) {
        QApplication::removeTranslator(m_translator);
        delete m_translator;
        m_translator = NULL;
    }

    m_void->setParent(NULL);
    delete m_void;

    m_events->setParent(NULL);
    delete m_events;

    m_contracts->setParent(NULL);
    delete m_contracts;

    m_proposals->setParent(NULL);
    delete m_proposals;

    m_people->setParent(NULL);
    delete m_people;

    m_companies->setParent(NULL);
    delete m_companies;
}

void MainWindow::saveSettings()
{
    m_settings.setValue("language", m_englishAction->isChecked() ? "en-US" : "pt-BR");
    m_settings.setValue("mainWindowGeometry", saveGeometry());
    m_settings.setValue("mainWindowState", saveState());
}

void MainWindow::closeEvent(QCloseEvent *)
{
    saveSettings();
    g_project->save(true);
}

void MainWindow::setLanguage(const QString& language)
{
    if(m_translator) {
        QApplication::removeTranslator(m_translator);
        delete m_translator;
        m_translator = NULL;
    }

    if(language == "en-US") {
        QLocale locale = QLocale(QLocale::English, QLocale::UnitedStates);
        QLocale::setDefault(locale);
    }
    else if(language == "pt-BR") {
        QLocale locale = QLocale(QLocale::Portuguese, QLocale::Brazil);
        QLocale::setDefault(locale);
    }
    else if(language == "es-ES") {
        QLocale locale = QLocale(QLocale::Spanish, QLocale::Spain);
        QLocale::setDefault(locale);
    }

    if(language != "en-US") {
        m_translator = new QTranslator();
        if(!m_translator->load(QString("translations/%1").arg(language)))
            qCritical() << "Translations not loaded.";
        QApplication::installTranslator(m_translator);
    }
}

void MainWindow::backup()
{
    saveSettings();
    g_project->save(true);
    QTimer::singleShot(BACKUP_TIME, this, SLOT(backup()));
}

void MainWindow::newFile()
{
    g_project->clean();
    onViewChanged();

    QString companyName = m_companies->createCompany();
    if(!companyName.isEmpty()) {
        g_project->create(companyName);
        onViewChanged();
    }
}

void MainWindow::openFile()
{
    QString defaultPath = m_settings.value("defaultPath", QDir::homePath()).toString();
    QString filename = QFileDialog::getOpenFileName(this, tr("Open"), defaultPath, tr("Data file (*.fm)"));
    if(filename.isEmpty())
        return;

    m_settings.setValue("defaultPath", QFileInfo(filename).absolutePath());

    g_project->setFilename(filename);
    g_project->load(false);
    onProjectUpdate();
}

void MainWindow::saveFile()
{
    if(g_project->getFilename().isEmpty()) {
        saveFileAs();
        return;
    }

    g_project->save(false);
}

void MainWindow::saveFileAs()
{
    QString defaultPath = g_project->getFilename();
    if(defaultPath.isEmpty()) {
        defaultPath = m_settings.value("defaultPath", QDir::homePath()).toString();
        QString name = g_project->getName() + ".fm";
        name = name.toLower();
        name = name.replace(" ", "_");

        QFileInfo info(QDir(defaultPath), name);
        defaultPath = info.absoluteFilePath();
    }

    QString filename = QFileDialog::getSaveFileName(this, tr("Save"), defaultPath, tr("Data file (*.fm)"));
    if(filename.isEmpty())
        return;

    m_settings.setValue("defaultPath", QFileInfo(filename).absolutePath());

    g_project->setFilename(filename);
    saveFile();
}

void MainWindow::findThing()
{

}

void MainWindow::onProjectUpdate()
{
    QString name = g_project->getName();
    if(name.isEmpty())
        setWindowTitle(tr("Manager"));
    else
        setWindowTitle(tr("Manager - %1%2").arg(name).arg(g_project->isSaved() ? "" : " *"));

    m_saveAction->setEnabled(!name.isEmpty());
    m_saveAsAction->setEnabled(!name.isEmpty());
}

void MainWindow::onLanguageChanged()
{
    setLanguage("en-US"); // This uninstalls all translations.
    close();
    QApplication::exit(EXIT_RESTART);
}

void MainWindow::onViewChanged()
{
    QWidget *widget = centralWidget();
    if(widget) {
        widget->setVisible(false);
        widget->setParent(0);
    }

    if(g_project->getName().isEmpty()) {
        setCentralWidget(m_void);
    }
    else if(m_eventsAction->isChecked()) {
        setCentralWidget(m_events);
        m_events->setVisible(true);
    }
    else if(m_contractsAction->isChecked()) {
        setCentralWidget(m_contracts);
        m_contracts->setVisible(true);
    }
    else if(m_proposalsAction->isChecked()) {
        setCentralWidget(m_proposals);
        m_proposals->setVisible(true);
    }
    else if(m_peopleAction->isChecked()) {
        setCentralWidget(m_people);
        m_people->setVisible(true);
    }
    else if(m_companiesAction->isChecked()) {
        setCentralWidget(m_companies);
        m_companies->setVisible(true);
    }
}
