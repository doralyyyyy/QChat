#include "main_window.h"
#include "login_window.h"
#include "client.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("QChat");
    QCoreApplication::setApplicationName("QChatClient");

    Client *client = new Client("localhost", 11455);      // 连接穿透网，11455端口

    // LoginWindow l(client);
    // l.show();

    // 注释掉上面两行，给下面三行取消注释，可直接进入main_window测试
    MainWindow w(client);
    client->mainWindow = &w;
    w.show();

    return a.exec();
}
