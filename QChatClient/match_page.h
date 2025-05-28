#ifndef MATCH_PAGE_H
#define MATCH_PAGE_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QMovie>
#include <QTimer>

class Client;

class MatchPage : public QWidget {
    Q_OBJECT

public:
    explicit MatchPage(Client *client, QWidget *parent = nullptr);
    void updateCroppedFrame(int frameNumber);
    void sleep(int ms);
    void stopMatching();
    void matchingFinished(const QString& result);
    void addFriend();
    void resizeEvent(QResizeEvent *event);

private slots:
    void startMatching();
    void cancelMatching();

private:
    QString buttonStyle() const;

    Client *client;

    QLabel *statusLabel;
    QPushButton *matchButton;
    QPushButton *cancelButton;
    QPushButton *addFriendButton;
    QString lastMatchedUser;
    QTimer *matchTimer;
    QLabel *gifLabel;   // GIF背景
    QMovie *movie;      // GIF播放器
};

#endif // MATCH_PAGE_H
