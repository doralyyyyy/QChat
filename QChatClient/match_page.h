#ifndef MATCH_PAGE_H
#define MATCH_PAGE_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>

class Client; // 前向声明，避免循环依赖

class MatchPage : public QWidget {
    Q_OBJECT

public:
    explicit MatchPage(Client *client, QWidget *parent = nullptr);

    void sleep(int ms);
    void stopMatching();
    void matchingFinished(const QString& result);
    void addFriend();

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
};

#endif // MATCH_PAGE_H
