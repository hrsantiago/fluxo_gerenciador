#include "const.h"
#include "mainwindow.h"
#include "project.h"
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
    m_contractsAction = viewMenu->addAction(tr("Contracts"), this, SLOT(onViewChanged()), QKeySequence("F1"));
    m_contractsAction->setCheckable(true);
    m_proposalsAction = viewMenu->addAction(tr("Proposals"), this, SLOT(onViewChanged()), QKeySequence("F2"));
    m_proposalsAction->setCheckable(true);
    m_peopleAction = viewMenu->addAction(tr("People"), this, SLOT(onViewChanged()), QKeySequence("F3"));
    m_peopleAction->setCheckable(true);
    m_companiesAction = viewMenu->addAction(tr("Companies"), this, SLOT(onViewChanged()), QKeySequence("F4"));
    m_companiesAction->setCheckable(true);

    QActionGroup *viewActionGroup = new QActionGroup(this);
    viewActionGroup->addAction(m_contractsAction);
    viewActionGroup->addAction(m_proposalsAction);
    viewActionGroup->addAction(m_peopleAction);
    viewActionGroup->addAction(m_companiesAction);
    m_proposalsAction->setChecked(true);

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

    m_contracts = new Contracts();
    m_proposals = new Proposals();
    connect(g_project, SIGNAL(projectLoad()), m_proposals, SLOT(onProjectLoad()));
    m_people = new People();
    connect(g_project, SIGNAL(projectLoad()), m_people, SLOT(onProjectLoad()));
    m_companies = new Companies();

    QString filename = QFileInfo(QFileInfo(QSettings().fileName()).absolutePath(), "project.fm").absoluteFilePath();
    g_project->setFilename(filename);
    g_project->load();

    onViewChanged();
}

MainWindow::~MainWindow()
{
    if(m_translator) {
        QApplication::removeTranslator(m_translator);
        delete m_translator;
        m_translator = NULL;
    }

    m_contracts->setParent(NULL);
    delete m_contracts;

    m_proposals->setParent(NULL);
    delete m_proposals;

    m_people->setParent(NULL);
    delete m_people;

    m_companies->setParent(NULL);
    delete m_companies;
}

void MainWindow::closeEvent(QCloseEvent *)
{
    m_settings.setValue("language", m_englishAction->isChecked() ? "en-US" : "pt-BR");
    m_settings.setValue("mainWindowGeometry", saveGeometry());
    m_settings.setValue("mainWindowState", saveState());

    m_companies->save();

    QString filename = QFileInfo(QFileInfo(QSettings().fileName()).absolutePath(), "project.fm").absoluteFilePath();
    g_project->setFilename(filename);
    g_project->save();
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

void MainWindow::newFile()
{
    QDialog dialog;
    dialog.setWindowTitle(tr("New"));

    QGridLayout *layout = new QGridLayout();
    dialog.setLayout(layout);

    int row = 0;
    layout->addWidget(new QLabel(tr("Company name:")), row, 0);
    QLineEdit *nameLineEdit = new QLineEdit;
    layout->addWidget(nameLineEdit, row++, 1);

    layout->addLayout(Tools::createOkCancel(&dialog), row, 0, 1, 2);

    if(dialog.exec() == QDialog::Accepted) {
        g_project->reset();
        g_project->setName(nameLineEdit->text());
        onProjectUpdate();
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
    g_project->load();
    onProjectUpdate();
}

void MainWindow::saveFile()
{
    if(g_project->getFilename().isEmpty())
        saveFileAs();

    g_project->save();
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

    if(m_contractsAction->isChecked()) {
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
