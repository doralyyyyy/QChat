#ifndef CHAT_PAGE_H
#define CHAT_PAGE_H

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

class ChatPage : public QWidget
{
    Q_OBJECT

public:
    ChatPage(Client *client, QWidget *parent = nullptr);
    ~ChatPage();

    void updateMessage(const QString &message);
    void exportChatToTxt();
    void refresh();

protected:
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dropEvent(QDropEvent *e) override;

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
    Client *client;
    DatabaseManager dbManager;
    QList<Message> messages;
};

#endif
