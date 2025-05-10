#ifndef TRAYMANAGER_H
#define TRAYMANAGER_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>

class TrayManager : public QObject {
    Q_OBJECT

public:
    explicit TrayManager(QWidget *mainWindow, QObject *parent=nullptr);
    void show();

private slots:
    void onIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onShowWindow();
    void onExit();

private:
    QWidget *mainWindow;
    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
};

#endif
