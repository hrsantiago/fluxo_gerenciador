#ifndef EVENTS_H
#define EVENTS_H

#include <QtGui>
#include "tools.h"
#include <core/project.h>

class Events : public QWidget
{
    Q_OBJECT

public:
    Events(QWidget *parent = 0);

    enum TableHeader {
        HEADER_DATE,
        HEADER_DESCRIPTION,
        HEADER_IDENTIFIER,
        HEADER_LAST
    };

private:
    void updateEventsList();
    Event *getCurrentEvent();
    MyTableWidgetItem *addEvent(Event *event);

    QLabel *m_dateTimeLabel;
    QTableWidget *m_eventsTable;

private slots:
    //void onAddEventClicked();
    //void onRemoveEventClicked();
    void onEventsCustomContextMenuRequested(QPoint pos);

    void onUpdateDateTime();
    void onProjectLoad();

};

#endif // EVENTS_H
