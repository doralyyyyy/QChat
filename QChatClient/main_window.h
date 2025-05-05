#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QFile>
#include <QDesktopServices>
#include <QUrl>
#include <QLabel>
#include <QFileInfo>
#include <QFileDialog>
#include "qlistwidget.h"
#include "client.h"
#include "database_manager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(Client *client, QWidget *parent = nullptr);
    ~MainWindow();

    void updateMessage(const QString &message);

private slots:
    void onSendButtonClicked();
    void onSendFileButtonClicked();

private:
    Ui::MainWindow *ui;
    QListWidget *listWidget;
    QLineEdit *inputField;
    QPushButton *sendButton;
    QPushButton *sendFileButton;
    Client *client;
    DatabaseManager dbManager;
};

#endif
