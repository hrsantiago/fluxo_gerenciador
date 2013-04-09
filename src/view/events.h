#ifndef EVENTS_H
#define EVENTS_H

#include "headers.h"
#include "tools.h"
#include "core/project.h"

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
    Thing *getCurrentEvent();
    MyTableWidgetItem *addEvent(Thing *event);

    QLabel *m_dateTimeLabel;
    QTableWidget *m_eventsTable;

private slots:
    //void onAddEventClicked();
    //void onRemoveEventClicked();
    void onEventsCustomContextMenuRequested(QPoint pos);

    void onUpdateDateTime();
    void onProjectLoad();
    void onProjectUpdate();

};

#endif // EVENTS_H
