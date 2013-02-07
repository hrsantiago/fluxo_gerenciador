#ifndef TOOLS_H
#define TOOLS_H

#include <QString>
#include <QWidget>
#include <QGridLayout>

namespace Tools
{
QWidget *getLayoutWidget(QGridLayout *layout, int row, int column);
QString requestYesNoFromUser(const QString& title, const QString& question);
QStringList requestDataFromUser(const QString &title, const QStringList& dataName, const QMap<int, QString>& defaultData);
bool isRequestedDataValid(const QStringList& data);
}

#endif // TOOLS_H
