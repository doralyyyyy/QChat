#include "main_window.h"
#include "register_window.h"
#include "client.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    RegisterWindow r;
    r.show();

    // 注释掉上面两行，给下面四行取消注释，可直接进入main_window测试
    // Client *client = new Client("10.7.54.26", 1145);
    // MainWindow w(client);
    // client->mainWindow = &w;
    // w.show();

    return a.exec();
}
