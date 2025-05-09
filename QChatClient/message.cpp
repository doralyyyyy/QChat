#include "message.h"

Message::Message(const QString &time, const QString &sender, const QString &content)
    : time(time), sender(sender), content(content) {}

QString Message::getTime() const {
    return time;
}

QString Message::getSender() const {
    return sender;
}

QString Message::getContent() const {
    return content;
}
