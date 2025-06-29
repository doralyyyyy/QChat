#ifndef MESSAGEBUBBLEWIDGET_H
#define MESSAGEBUBBLEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>

class MessageBubbleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MessageBubbleWidget(const QString &time, const QString &sender, const QString &content, bool isSelf, QWidget *parent = nullptr);
    ~MessageBubbleWidget() = default;

private:
    void setupTextMessage();
    void setupFileMessage(const QString &content);

    QVBoxLayout *bubbleLayout;
    QHBoxLayout *hLayout;
    QWidget *bubble;
    QLabel *labelTop;
    QLabel *labelBottom;
    bool isSelf;
};

#endif
