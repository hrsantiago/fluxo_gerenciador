#include "const.h"
#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <QMessageBox>

void messageHandler(QtMsgType type, const char *msg)
{
    switch(type) {
#ifdef __DEBUG__
        case QtDebugMsg:
            fprintf(stderr, "Debug: %s\n", msg);
            break;
#endif
        case QtWarningMsg:
            fprintf(stderr, "Warning: %s\n", msg);
            break;

        case QtCriticalMsg:
            QMessageBox::critical(QApplication::activeWindow(), QMessageBox::tr("Error"), msg);
            fprintf(stderr, "Critical: %s\n", msg);
            break;

        case QtFatalMsg:
            QMessageBox::critical(QApplication::activeWindow(), QMessageBox::tr("Fatal Error"), msg);
            fprintf(stderr, "Fatal: %s\n", msg);
            abort();
            break;

        default:
            break;
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("Fluxo Engenharia");
    QCoreApplication::setOrganizationDomain("fluxoengenharia.com.br");
    QCoreApplication::setApplicationName("Manager");
    QCoreApplication::setApplicationVersion("1.0.0");

    qInstallMsgHandler(messageHandler);

    int ret;
    do {
        MainWindow w;
        w.show();
        ret = a.exec();
    } while(ret == EXIT_RESTART);

    return ret;
}
