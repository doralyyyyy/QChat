#include "tray_manager.h"
#include <QAction>
#include <QApplication>

TrayManager::TrayManager(QWidget *mainWindow, QObject *parent)
    : QObject(parent), mainWindow(mainWindow) {
    trayIcon = new QSystemTrayIcon(mainWindow);
    trayIcon->setIcon(QIcon(QCoreApplication::applicationDirPath() + "/../../../images/icon.png"));
    trayIcon->setToolTip("QChatServer 正在后台运行");

    trayMenu = new QMenu();

    QAction *showAction = new QAction("打开 QChatServer");
    QAction *exitAction = new QAction("退出");

    connect(showAction, &QAction::triggered, this, &TrayManager::onShowWindow);
    connect(exitAction, &QAction::triggered, this, &TrayManager::onExit);
    connect(trayIcon, &QSystemTrayIcon::activated, this, &TrayManager::onIconActivated);

    trayMenu->addAction(showAction);
    trayMenu->addAction(exitAction);
    trayIcon->setContextMenu(trayMenu);
}

void TrayManager::show() {
    trayIcon->show();
}

void TrayManager::onIconActivated(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) {
        mainWindow->show();
        mainWindow->raise();
        mainWindow->activateWindow();
    }
}

void TrayManager::onShowWindow() {
    mainWindow->show();
    mainWindow->raise();
    mainWindow->activateWindow();
}

void TrayManager::onExit() {
    QApplication::quit();
}
