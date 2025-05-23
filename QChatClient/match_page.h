#ifndef MATCHPAGE_H
#define MATCHPAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include "client.h"

class MatchPage : public QWidget {
    Q_OBJECT

public:
    explicit MatchPage(Client *client, QWidget *parent = nullptr);

private:
    Client *client;
};

#endif // MATCHPAGE_H
