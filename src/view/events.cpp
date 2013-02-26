#include "events.h"

Events::Events(QWidget *parent) :
    QWidget(parent)
{
    QGridLayout *gridLayout = new QGridLayout;
    setLayout(gridLayout);
    int row = 0;

    gridLayout->addWidget(new QLabel(tr("List of events:")), row, 0);

    gridLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), row, 1);

    m_dateTimeLabel = new QLabel;
    gridLayout->addWidget(m_dateTimeLabel, row++, 2);

    // Top
    m_eventsTable = new QTableWidget;
    m_eventsTable->setColumnCount(HEADER_LAST);
    m_eventsTable->setHorizontalHeaderLabels(QString("%1,%2,%3").arg(tr("Date")).arg(tr("Description")).arg(tr("Identifier")).split(","));
    m_eventsTable->verticalHeader()->hide();
    m_eventsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_eventsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_eventsTable->setSortingEnabled(true);
    m_eventsTable->setContextMenuPolicy(Qt::CustomContextMenu);
    //connect(m_eventsTable, SIGNAL(currentCellChanged(int,int,int,int)), this, SLOT(onProposalsCurrentCellChanged(int,int,int,int)));
    //connect(m_eventsTable, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(onProposalsCellDoubleClicked(int, int)));
    //connect(m_eventsTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onProposalsCustomContextMenuRequested(QPoint)));
    gridLayout->addWidget(m_eventsTable, row++, 0, 1, 3);

    onUpdateDateTime();
    connect(g_project, SIGNAL(projectLoad()), this, SLOT(onProjectLoad()));
}

void Events::updateEventsList()
{
    QString description;
    QTableWidgetItem *referenceItem = m_eventsTable->item(m_eventsTable->currentRow(), HEADER_DESCRIPTION);
    if(referenceItem)
        description = referenceItem->text();

    MyTableWidgetItem *currentItem = NULL;

    m_eventsTable->setSortingEnabled(false);
    m_eventsTable->clearContents();
    m_eventsTable->setRowCount(0);

    const QVector<Event*>& events = g_project->getEvents();
    for(QVector<Event*>::const_iterator it = events.constBegin(), end = events.constEnd(); it != end; ++it) {
        MyTableWidgetItem *item = addEvent(*it);
        if(description == (*it)->getString("description"))
            currentItem = item;
    }

    m_eventsTable->setSortingEnabled(true);
    m_eventsTable->resizeColumnsToContents();

    if(currentItem)
        m_eventsTable->selectRow(m_eventsTable->row(currentItem));
    else if(m_eventsTable->rowCount() > 0)
        m_eventsTable->selectRow(0);
}

MyTableWidgetItem *Events::addEvent(Event *event)
{
    QLocale locale;

    MyTableWidgetItem *date = new MyTableWidgetItem();
    date->setData(Qt::DisplayRole, locale.toString(event->getDate("date"), "dd/MM/yyyy (dddd)"));
    date->setData(Qt::UserRole, event->getDate("date"));
    date->setFlags(date->flags() & ~Qt::ItemIsEditable);
    if(event->getDate("date") <= QDate::currentDate())
        date->setForeground(QBrush(QColor(255, 0, 0)));

    MyTableWidgetItem *description = new MyTableWidgetItem();
    description->setFlags(description->flags() & ~Qt::ItemIsEditable);

    QString id = event->getString("id"); // This must be done this way, so tr will work properly.
    if(id == "waiting_send")
        description->setData(Qt::DisplayRole, tr("Send proposal to client."));
    else if(id == "waiting_response")
        description->setData(Qt::DisplayRole, tr("Update proposal with client response."));
    else
        description->setData(Qt::DisplayRole, event->getString("description"));

    QString identifierStr = event->getString("identifier");
    if(identifierStr.isEmpty())
        identifierStr = tr("Project");

    MyTableWidgetItem *identifier = new MyTableWidgetItem();
    identifier->setData(Qt::DisplayRole, identifierStr);
    identifier->setFlags(identifier->flags() & ~Qt::ItemIsEditable);

    m_eventsTable->setRowCount(m_eventsTable->rowCount()+1);
    m_eventsTable->setItem(m_eventsTable->rowCount()-1, HEADER_DATE, date);
    m_eventsTable->setItem(m_eventsTable->rowCount()-1, HEADER_DESCRIPTION, description);
    m_eventsTable->setItem(m_eventsTable->rowCount()-1, HEADER_IDENTIFIER, identifier);

    return description;
}

void Events::onUpdateDateTime()
{
    QString text = QDateTime::currentDateTime().toString("hh:mm:ss ; dd/MM/yyyy (dddd)");
    m_dateTimeLabel->setText(text);
    QTimer::singleShot(1000, this, SLOT(onUpdateDateTime()));
}

void Events::onProjectLoad()
{
    updateEventsList();
    m_eventsTable->sortItems(HEADER_DATE, Qt::AscendingOrder);
}
