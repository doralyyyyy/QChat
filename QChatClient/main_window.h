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
#include <QPrinter>
#include <QList>
#include "delay_send_dialog.h"
#include "qlistwidget.h"
#include "client.h"
#include "database_manager.h"
#include "message_search_widget.h"
#include "message.h"
#include "message_context_menu_handler.h"
#include "tray_manager.h"

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
    void exportChatToTxt();
    void closeEvent(QCloseEvent *event);

private slots:
    void onSendButtonClicked();
    void onSendFileButtonClicked();
    void onSearchButtonClicked();
    void onDelaySendClicked();
    void onRecordButtonClicked();
    void onCameraButtonClicked();
    void onWordCloudRequested();
    void onRelationAnalysisRequested();
    void onExportChatToPdfRequested();
    void onGenerateTimelineRequested();

private:
    Ui::MainWindow *ui;
    QListWidget *listWidget;
    QLineEdit *inputField;
    QPushButton *cameraButton;
    QPushButton *recordButton;
    QPushButton *sendButton;
    QPushButton *sendFileButton;
    MessageSearchWidget *searchWidget;
    QPushButton *searchButton;
    QPushButton *delaySendButton;
    MessageContextMenuHandler *contextMenu;
    TrayManager *trayManager;
    Client *client;
    DatabaseManager dbManager;
    QList<Message> messages;
};

#endif
