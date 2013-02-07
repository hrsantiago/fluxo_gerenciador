#include "proposals.h"
#include <QtGui>

Proposals::Proposals()
{
    QGridLayout *gLayout;
    int row;

    QPushButton *addProposal = new QPushButton("+");
    connect(addProposal, SIGNAL(clicked()), this, SLOT(addProposal()));

    QPushButton *removeProposal = new QPushButton("-");
    connect(removeProposal, SIGNAL(clicked()), this, SLOT(removeProposal()));

    QPushButton *addItem = new QPushButton("+");
    connect(addItem, SIGNAL(clicked()), this, SLOT(addItem()));

    QPushButton *removeItem = new QPushButton("-");
    connect(removeItem, SIGNAL(clicked()), this, SLOT(removeItem()));

    m_proposalsList = new QListWidget;
    m_itemsTable = new QTableWidget;
    m_itemsTable->setColumnCount(4);
    m_itemsTable->setHorizontalHeaderLabels(QString("%1,%2,%3,%4").arg(tr("Description")).arg(tr("Value")).arg(tr("Start")).arg(tr("End")).split(","));
    m_itemsTable->verticalHeader()->hide();
    m_itemsTable->horizontalHeader()->setStretchLastSection(true);

    // Top
    QWidget *top = new QWidget;
    gLayout = new QGridLayout();
    top->setLayout(gLayout);

    row = 0;
    gLayout->addWidget(new QLabel(tr("List of proposals:")), row++, 0, 1, 2);
    gLayout->addWidget(m_proposalsList, row, 0, 3, 1);
    gLayout->addWidget(addProposal, row++, 1);
    gLayout->addWidget(removeProposal, row++, 1);
    gLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), row++, 1);

    // Bottom
    QWidget *bottom = new QWidget;
    gLayout = new QGridLayout();
    bottom->setLayout(gLayout);

    row = 0;
    gLayout->addWidget(new QLabel(tr("List of items:")), row++, 0, 1, 2);
    gLayout->addWidget(m_itemsTable, row, 0, 3, 1);
    gLayout->addWidget(addItem, row++, 1);
    gLayout->addWidget(removeItem, row++, 1);
    gLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), row++, 1);

    // Splitter
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    QSplitter *vSplitter = new QSplitter(Qt::Vertical);
    layout->addWidget(vSplitter);

    vSplitter->addWidget(top);
    vSplitter->addWidget(bottom);
    vSplitter->setStretchFactor(0, 30);
    vSplitter->setStretchFactor(1, 70);

//    QCalendarWidget *calendar = new QCalendarWidget();
//    calendar->setGridVisible(true);
//    calendar->setVisible(true);
//    QDate selectedDate = calendar->selectedDate();
}

void Proposals::addProposal()
{

}

void Proposals::removeProposal()
{

}

void Proposals::addItem()
{

}

void Proposals::removeItem()
{

}
