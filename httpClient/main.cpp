#include <QApplication>
#include "httprequestwindow.h"

void mesHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    (HttpRequestWindow::getInsatance())->logMessage(msg);
}

int main(int argc, char **argv)
{
   QApplication app(argc, argv);
   qInstallMessageHandler(mesHandler);

   (HttpRequestWindow::getInsatance())->show();

    return app.exec();
}
