#include "const.h"
#include "view/mainwindow.h"
#include "headers.h"

#if QT_VERSION > 0x050000
void messageHandler(QtMsgType type, const QMessageLogContext &, const QString& msg)
#else
void messageHandler(QtMsgType type, const char* msg)
#endif
{
    QString strMsg = msg;
    switch(type) {
#ifdef __DEBUG__
        case QtDebugMsg:
            fprintf(stderr, "Debug: %s\n", strMsg.toStdString().c_str());
            break;
#endif
        case QtWarningMsg:
            fprintf(stderr, "Warning: %s\n", strMsg.toStdString().c_str());
            break;

        case QtCriticalMsg:
            QMessageBox::critical(QApplication::activeWindow(), QMessageBox::tr("Error"), strMsg);
            fprintf(stderr, "Critical: %s\n", strMsg.toStdString().c_str());
            break;

        case QtFatalMsg:
            QMessageBox::critical(QApplication::activeWindow(), QMessageBox::tr("Fatal Error"), strMsg);
            fprintf(stderr, "Fatal: %s\n", strMsg.toStdString().c_str());
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


#if QT_VERSION > 0x050000
    qInstallMessageHandler(messageHandler);
#else
    qInstallMsgHandler(messageHandler);
#endif

    int ret;
    do {
        MainWindow w;
        w.show();
        ret = a.exec();
    } while(ret == EXIT_RESTART);

    return ret;
}
