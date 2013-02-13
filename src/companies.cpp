#include "companies.h"
#include "tools.h"

Companies::Companies(QWidget *parent) :
    QWidget(parent)
{
    // Left Widget
    QWidget *left = new QWidget;
    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setContentsMargins(0, 0, 0, 0);
    left->setLayout(gridLayout);

    m_list = new QListWidget;
    connect(m_list, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(onListItemChanged(QListWidgetItem*,QListWidgetItem*)));
    gridLayout->addWidget(m_list, 1, 0);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    gridLayout->addLayout(buttonsLayout, 2, 0);

    QPushButton *addButton = new QPushButton(tr("Add"));
    connect(addButton, SIGNAL(clicked()), this, SLOT(onAddButtonClicked()));
    buttonsLayout->addWidget(addButton);

    QPushButton *removeButton = new QPushButton(tr("Remove"));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(onRemoveButtonClicked()));
    buttonsLayout->addWidget(removeButton);

    // Right Widget
    QWidget *right = new QWidget;
    m_layout = new QGridLayout();
    right->setLayout(m_layout);

    int row = 0;
    m_layout->addWidget(new QLabel(tr("Name:")), row++, 0);
    m_layout->addWidget(new QLabel(tr("Address:")), row++, 0);
    m_layout->addWidget(new QLabel(tr("City:")), row++, 0);
    m_layout->addWidget(new QLabel(tr("State:")), row++, 0);
    m_layout->addWidget(new QLabel(tr("Zipcode:")), row++, 0);
    m_layout->addWidget(new QLabel(tr("Telephone:")), row++, 0);
    m_layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), row++, 0, 1, 2);

    // Splitter
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    QSplitter *hSplitter = new QSplitter(Qt::Horizontal);
    layout->addWidget(hSplitter);

    hSplitter->addWidget(left);
    hSplitter->addWidget(right);
    hSplitter->setStretchFactor(0, 30);
    hSplitter->setStretchFactor(1, 70);

    load();
}

void Companies::save()
{
    QSettings settings("companies.ini", QSettings::IniFormat);
    settings.clear();

    settings.beginGroup("People");
    for(int i = 0; i < m_companies.size(); ++i) {
        settings.beginGroup(QString("Company%1").arg(i));
        settings.setValue("name", m_companies[i].lineEdit[Company::NAME]->text());
        settings.setValue("address", m_companies[i].lineEdit[Company::ADDRESS]->text());
        settings.setValue("city", m_companies[i].lineEdit[Company::CITY]->text());
        settings.setValue("state", m_companies[i].lineEdit[Company::STATE]->text());
        settings.setValue("zipcode", m_companies[i].lineEdit[Company::ZIPCODE]->text());
        settings.setValue("telephone", m_companies[i].lineEdit[Company::TELEPHONE]->text());
        settings.endGroup();
    }
    settings.endGroup();
}

void Companies::load()
{
    QSettings settings("companies.ini", QSettings::IniFormat);

    settings.beginGroup("People");
    QStringList childGroups = settings.childGroups();
    for(QStringList::iterator it = childGroups.begin(), end = childGroups.end(); it != end; ++it) {
        settings.beginGroup(*it);
        Company company;
        company.lineEdit[Company::NAME]->setText(settings.value("name").toString());
        company.lineEdit[Company::ADDRESS]->setText(settings.value("address").toString());
        company.lineEdit[Company::CITY]->setText(settings.value("city").toString());
        company.lineEdit[Company::STATE]->setText(settings.value("state").toString());
        company.lineEdit[Company::ZIPCODE]->setText(settings.value("zipcode").toString());
        company.lineEdit[Company::TELEPHONE]->setText(settings.value("telephone").toString());
        m_companies.push_back(company);
        settings.endGroup();
    }
    settings.endGroup();

    updateList();
}

void Companies::updateList()
{
    QModelIndex currentIndex = m_list->currentIndex();
    int count = m_list->count();

    m_list->clear();
    for(QVector<Company>::iterator it = m_companies.begin(), end = m_companies.end(); it != end; ++it)
        m_list->addItem((*it).lineEdit[Company::NAME]->text());
    m_list->sortItems();

    if(m_list->count() == count)
        m_list->setCurrentIndex(currentIndex);
    else if(m_list->count() > 0)
        m_list->setCurrentRow(0);
}

void Companies::onListItemChanged(QListWidgetItem *current, QListWidgetItem *)
{
    for(int i = 0; i < m_companies.size(); ++i) {
        if(current && current->text() == m_companies[i].lineEdit[Company::NAME]->text()) {
            for(int j = 0; j < Company::LAST; ++j) {
                QWidget *widget = Tools::getLayoutWidget(m_layout, j, 1);
                if(widget) {
                    widget->hide();
                    widget->setParent(0);
                }
            }
            int row = 0;
            m_layout->addWidget(m_companies[i].lineEdit[Company::NAME], row++, 1);
            m_layout->addWidget(m_companies[i].lineEdit[Company::ADDRESS], row++, 1);
            m_layout->addWidget(m_companies[i].lineEdit[Company::CITY], row++, 1);
            m_layout->addWidget(m_companies[i].lineEdit[Company::STATE], row++, 1);
            m_layout->addWidget(m_companies[i].lineEdit[Company::ZIPCODE], row++, 1);
            m_layout->addWidget(m_companies[i].lineEdit[Company::TELEPHONE], row++, 1);
            m_companies[i].lineEdit[Company::NAME]->setVisible(true);
            m_companies[i].lineEdit[Company::ADDRESS]->setVisible(true);
            m_companies[i].lineEdit[Company::CITY]->setVisible(true);
            m_companies[i].lineEdit[Company::STATE]->setVisible(true);
            m_companies[i].lineEdit[Company::ZIPCODE]->setVisible(true);
            m_companies[i].lineEdit[Company::TELEPHONE]->setVisible(true);
        }
    }
}

void Companies::onAddButtonClicked()
{
    QMap<int, QString> defaultData;

    QStringList a = Tools::requestDataFromUser(tr("Add Company"), tr("Name,"
                                                                     "Address,"
                                                                     "City,"
                                                                     "State,"
                                                                     "Zipcode,"
                                                                     "Telephone").split(","), defaultData);
    if(!Tools::isRequestedDataValid(a))
        return;

    for(int i = 0; i < m_companies.size(); ++i) {
        if(a[0] == m_companies[i].lineEdit[Company::NAME]->text()) {
            qCritical() << tr("A company with this name already exists.");
            return;
        }
    }

    Company company;
    company.lineEdit[Company::NAME]->setText(a[0]);
    company.lineEdit[Company::ADDRESS]->setText(a[1]);
    company.lineEdit[Company::CITY]->setText(a[2]);
    company.lineEdit[Company::STATE]->setText(a[3]);
    company.lineEdit[Company::ZIPCODE]->setText(a[4]);
    company.lineEdit[Company::TELEPHONE]->setText(a[5]);
    m_companies.push_back(company);

    updateList();
}

void Companies::onRemoveButtonClicked()
{
    if(m_list->currentItem()) {
        if(Tools::requestYesNoFromUser(tr("Remove Company"), tr("Do you really want to remove this company?")) == "No")
            return;

        QListWidgetItem *currentItem = m_list->currentItem();
        for(int i = 0; i < m_companies.size(); ++i) {
            if(currentItem->text() == m_companies[i].lineEdit[Company::NAME]->text()) {
                m_companies.erase(m_companies.begin()+i);
                break;
            }
        }
        m_list->removeItemWidget(currentItem);

        updateList();
    }
}
