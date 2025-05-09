#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>

class Message {
public:
    Message(const QString &time, const QString &sender, const QString &content);

    QString getTime() const;
    QString getSender() const;
    QString getContent() const;
    QString time;     // 消息时间
    QString sender;   // 消息发送者
    QString content;  // 消息内容

private:

};

#endif
