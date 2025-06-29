#include "main_window.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setWindowIcon(QIcon(":/images/icon.png"));
    Server *server=new Server(11455);

    MainWindow w(server);
    w.show();

    return a.exec();
}
