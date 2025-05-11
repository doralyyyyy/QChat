#include "main_window.h"
#include "login_window.h"
#include "client.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QTimer>

int main(int argc,char *argv[]) {
    QApplication a(argc,argv);
    QCoreApplication::setOrganizationName("QChat");
    QCoreApplication::setApplicationName("QChatClient");

    // 欢迎界面
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/welcome.qml")));
    auto rootObjects=engine.rootObjects();
    QObject *rootObject=rootObjects.first();
    if(rootObjects.isEmpty())
        return -1;

    Client *client=new Client("localhost",11455);

    QTimer::singleShot(2000,[&](){
        rootObject->deleteLater();

        // LoginWindow *l=new LoginWindow(client);
        // l->show();
        // 注释掉上面四行，给下面三行取消注释，可直接进入main_window测试
        MainWindow *w=new MainWindow(client);
        client->mainWindow=w;
        w->show();
    });


    return a.exec();
}
