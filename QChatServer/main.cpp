#include "main_window.h"
#include "qsettings.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setWindowIcon(QIcon(":/images/icon.png"));
    Server *server=new Server(11455);

    MainWindow w(server);
    w.show();

    // 清除关闭记忆（正常应被注释掉）
    // QSettings s("MyCompany", "QChat");
    // s.remove("close_behavior");

    return a.exec();
}
