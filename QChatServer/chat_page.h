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
#include "emoji_picker.h"
#include "feedback_dialog.h"
#include "qlistwidget.h"
#include "server.h"
#include "database_manager.h"
#include "message_search_widget.h"
#include "message.h"
#include "message_context_menu_handler.h"

class ChatPage : public QWidget
{
    Q_OBJECT

public:
    ChatPage(Server *server, QWidget *parent = nullptr);
    ~ChatPage();

    void updateMessage(const QString &message);
    void exportChatToTxt();
    void refresh();
    void showEmojiAnimation(const QString &emoji);

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
    QMap<QString, QString> keywordMap={
        {"爱你","❤️"},{"我爱你","❤️"},{"喜欢你","❤️"},{"想你","🥺"},{"想见你","🥺"},{"生气","😡"},{"发火","😠"},{"气死我了","😤"},{"笑死","😂"},
        {"搞笑","🤣"},{"无语","🙄"},{"赞","👍"},{"棒","👍"},{"鼓掌","👏"},{"加油","💪"},{"抱抱","🤗"},{"哭了","😭"},{"泪目","😢"},{"好累","😩"},
        {"晚安","🌙"},{"早安","☀️"},{"吃饭了吗","🍚"},{"吃饭","🍽️"},{"生日快乐","🎂"},{"新年快乐","🎉"},{"节日快乐","🎊"},{"圣诞快乐","🎄"},
        {"好美","😍"},{"好帅","😎"},{"摸鱼","🐟"},{"下班","🏃"},{"溜了","🏃"},{"出发","✈️"},{"到家","🏠"},{"回家","🏠"},{"拥抱","🫂"},
        {"亲亲","😘"},{"么么哒","😘"},{"心碎","💔"},{"震惊","😱"},{"我晕","😵"},{"晕了","😵"},{"睡觉","🛌"},{"再见","👋"},{"拜拜","👋"}
    };
    bool ifinit;
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
    DatabaseManager dbManager;
    QList<Message> messages;
    EmojiPicker *emojiPicker;
    QPushButton *emojiButton;
    FeedbackDialog *feedbackDialog;
};

#endif
