#include "main_window.h"
#include "login_window.h"
#include "client.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("QChat");
    QCoreApplication::setApplicationName("QChatClient");

    LoginWindow l;
    l.show();

    // 注释掉上面两行，给下面四行取消注释，修改两个传入参数，可直接进入main_window测试
    // Client *client = new Client("localhost", 11455);
    // MainWindow w(client);
    // client->mainWindow = &w;
    // w.show();

    return a.exec();
}
