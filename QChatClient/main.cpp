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

    Client *client=new Client("localhost",11455);

    QTimer::singleShot(2000,rootObject,[=](){
        rootObject->deleteLater();
        LoginWindow *l=new LoginWindow(client);
        l->show();
    });

    // 清除关闭记忆（正常应被注释掉）
    // QSettings s("MyCompany", "QChat");
    // s.remove("close_behavior");

    return a.exec();
}
