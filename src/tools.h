#ifndef TOOLS_H
#define TOOLS_H

#include <QtGui>

namespace Tools
{
QWidget *getLayoutWidget(QGridLayout *layout, int row, int column);
QHBoxLayout *createAcceptReject(QDialog *dialog, const QString& acceptText, const QString& rejectText);
QHBoxLayout *createOkCancel(QDialog *dialog);
QString requestYesNoFromUser(const QString& title, const QString& question);
QStringList requestDataFromUser(const QString &title, const QStringList& dataName, const QMap<int, QString>& defaultData = QMap<int, QString>());
bool isRequestedDataValid(const QStringList& data);
}

#endif // TOOLS_H
