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
#include <QTimer>
#include <QProcess>
#include <QLabel>
#include <QPixmap>
#include "delay_send_dialog.h"
#include "qlistwidget.h"
#include "server.h"
#include "database_manager.h"
#include "message_search_widget.h"

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

    void updateMessage(const QString &message);
    void exportChatToTxt();


private slots:
    void onSendButtonClicked();
    void onSendFileButtonClicked();
    void onSearchButtonClicked();
    void onDelaySendClicked();
    void onWordCloudRequested();
    void onRelationAnalysisRequested();
    void onRecordButtonClicked();

private:
    Ui::MainWindow *ui;
    QListWidget *listWidget;
    QLineEdit *inputField;
    QPushButton *recordButton;
    QPushButton *sendButton;
    QPushButton *sendFileButton;
    MessageSearchWidget *searchWidget;
    QPushButton *searchButton;
    QPushButton *delaySendButton;
    Server *server;
    DatabaseManager dbManager;
};

#endif
