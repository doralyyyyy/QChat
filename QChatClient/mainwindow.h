#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QApplication>
#include "client.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void updateMessage(const QString& message);

private slots:
    void onSendButtonClicked();

private:
    Ui::MainWindow *ui;
    QTextEdit *textEdit;
    QLineEdit *inputField;
    QPushButton *sendButton;
    Client* client;
    DatabaseManager dbManager;
};
#endif
