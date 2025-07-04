#include "login_window.h"
#include "client.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QTimer>

int main(int argc,char *argv[]) {
    QApplication a(argc,argv);
    QCoreApplication::setOrganizationName("QChat");
    QCoreApplication::setApplicationName("QChatClient");
    a.setWindowIcon(QIcon(QCoreApplication::applicationDirPath() + "/../../../images/icon.png"));

    // 欢迎界面
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/welcome.qml")));
    auto rootObjects=engine.rootObjects();
    QObject *rootObject=rootObjects.first();

    Client *client=new Client("localhost",11455);    // 修改第一个传入参数，即可切换连接IP地址

    QTimer::singleShot(2000,rootObject,[=](){
        rootObject->deleteLater();
        LoginWindow *l=new LoginWindow(client);
        l->show();
    });

    return a.exec();
}
