#include "tools.h"
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QDebug>

QWidget *Tools::getLayoutWidget(QGridLayout *layout, int row, int column)
{
    QLayoutItem *item = layout->itemAtPosition(row, column);
    if(item)
        return item->widget();
    return NULL;
}

QString Tools::requestYesNoFromUser(const QString& title, const QString& question)
{
    QDialog dialog;
    dialog.setWindowTitle(title);

    QGridLayout *layout = new QGridLayout();
    dialog.setLayout(layout);

    layout->addWidget(new QLabel(question), 0, 0, 1, 2);

    QPushButton *yes = new QPushButton(QObject::tr("Yes"));
    yes->connect(yes, SIGNAL(clicked()), &dialog, SLOT(accept()));
    layout->addWidget(yes, 1, 0);
    QPushButton *no = new QPushButton(QObject::tr("No"));
    no->connect(no, SIGNAL(clicked()), &dialog, SLOT(reject()));
    layout->addWidget(no, 1, 1);

    if(dialog.exec() == QDialog::Accepted)
        return "Yes";
    return "No";
}

QStringList Tools::requestDataFromUser(const QString &title, const QStringList& dataName, const QMap<int, QString>& defaultData)
{
    QDialog dialog;
    dialog.setWindowTitle(title);

    QGridLayout *layout = new QGridLayout();
    dialog.setLayout(layout);

    for(int i = 0; i < dataName.size(); ++i) {
        int columnSpan = defaultData[i] == "QLabel|" ? 2 : 1;
        layout->addWidget(new QLabel(QString("%1:").arg(dataName.at(i))), i, 0, 1, columnSpan);

        if(!defaultData[i].isEmpty()) {
            QStringList data = defaultData[i].split("|");

            if(data[0] == "QLineEdit") {
                QLineEdit *lineEdit = new QLineEdit;
                lineEdit->setText(data[1]);
                layout->addWidget(lineEdit, i, 1);
            }
            else if(data[0] == "QComboBox") {
                QComboBox *comboBox = new QComboBox;
                for(int j = 1; j < data.size(); j++)
                    comboBox->addItem(data[j]);
                layout->addWidget(comboBox, i, 1);
            }
            else if(data[0] == "QLabel");
            else {
                qWarning() << "Unknown QWidget type" << data[0];
            }
        }
        else {
            QLineEdit *lineEdit = new QLineEdit;
            layout->addWidget(lineEdit, i, 1);
        }
    }

    QHBoxLayout *hLayout = new QHBoxLayout();
    layout->addLayout(hLayout, dataName.size(), 0, 1, 2);

    QPushButton *ok = new QPushButton("Ok");
    ok->connect(ok, SIGNAL(clicked()), &dialog, SLOT(accept()));
    hLayout->addWidget(ok);
    QPushButton *cancel = new QPushButton("Cancelar");
    cancel->connect(cancel, SIGNAL(clicked()), &dialog, SLOT(reject()));
    hLayout->addWidget(cancel);

    if(dialog.exec() == QDialog::Accepted) {
        QStringList returnStringList;

        for(int i = 0; i < dataName.size(); ++i) {
            QLineEdit *lineEdit = qobject_cast<QLineEdit*>(getLayoutWidget(layout, i, 1));
            if(!lineEdit) {
                QComboBox *comboBox = qobject_cast<QComboBox*>(getLayoutWidget(layout, i, 1));
                if(comboBox)
                    returnStringList.append(QString("%1|%2").arg(comboBox->currentText()).arg(comboBox->currentIndex()));
            }
            else
                returnStringList.append(lineEdit->text());
        }

        return returnStringList;
    }

    return QStringList();
}

bool Tools::isRequestedDataValid(const QStringList& data)
{
    if(data.isEmpty())
        return false;

    for(int i = 0; i < data.size(); ++i) {
        if(data[i].isEmpty()) {
            qCritical() << QObject::tr("All data must be filled correctly.");
            return false;
        }
    }

    return true;
}
