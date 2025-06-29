#ifndef CHAT_PAGE2_H
#define CHAT_PAGE2_H

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
#include "emoji_picker.h"
#include "feedback_dialog.h"
#include "qlistwidget.h"
#include "server.h"
#include "message_search_widget.h"
#include "message.h"
#include "message_context_menu_handler.h"

class ChatPage2 : public QWidget
{
    Q_OBJECT

public:
    ChatPage2(Server *server, QWidget *parent = nullptr);
    ~ChatPage2();

    void exportChatToTxt();
    void refresh();
    void popMessageBox();

    QLabel *chatTitleLabel;

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
    void onFeedbackRequested();
    void onFeedbackSend(const QString &feedback);

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
    Server *server;
    QList<Message> messages;
    EmojiPicker *emojiPicker;
    QPushButton *emojiButton;
    FeedbackDialog *feedbackDialog;
};

#endif
