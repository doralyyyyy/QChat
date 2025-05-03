#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QTextEdit>
#include "server.h"
#include "DatabaseManager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    void extracted(QVector<QString> &msgs);
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void updateMessage(const QString &message);

private slots:
    void onSendButtonClicked();

private:
    Ui::MainWindow *ui;
    QTextEdit *textEdit;
    QLineEdit *inputField;
    QPushButton *sendButton;
    Server *server;
    DatabaseManager dbManager;
};
#endif
