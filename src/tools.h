#ifndef TOOLS_H
#define TOOLS_H

#include <QtGui>

class MyTableWidgetItem : public QTableWidgetItem
{
public:
    MyTableWidgetItem(int type = Type) : QTableWidgetItem(type) {}
    ~MyTableWidgetItem () {}

    bool operator<(const QTableWidgetItem& other) const
    {
        Q_ASSERT(tableWidget());
        if(data(Qt::UserRole).type() == QVariant::Int && other.data(Qt::UserRole).type() == QVariant::Int)
            return data(Qt::UserRole).toInt() < other.data(Qt::UserRole).toInt();
        else if(data(Qt::UserRole).type() == QVariant::Double && other.data(Qt::UserRole).type() == QVariant::Double)
            return data(Qt::UserRole).toDouble() < other.data(Qt::UserRole).toDouble();
        else if(data(Qt::UserRole).type() == QVariant::Date && other.data(Qt::UserRole).type() == QVariant::Date)
            return data(Qt::UserRole).toDate() < other.data(Qt::UserRole).toDate();

        return data(Qt::DisplayRole).toString().toLower() < other.data(Qt::DisplayRole).toString().toLower();
    }
};

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
