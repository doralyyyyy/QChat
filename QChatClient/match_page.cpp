#include "match_page.h"

MatchPage::MatchPage(Client *client, QWidget *parent)
    : QWidget(parent), client(client) {
    QVBoxLayout *layout=new QVBoxLayout(this);
    client->matchPage=this;

    QLabel *label=new QLabel("这里是好友匹配界面");
    layout->addWidget(label);
    setLayout(layout);
}
