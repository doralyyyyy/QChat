#include "emoji_picker.h"

EmojiPicker::EmojiPicker(QWidget *parent) : QWidget(parent) {
    setWindowFlags(Qt::Popup);
    setStyleSheet("background:white; border-radius:6px;");
    QGridLayout *layout=new QGridLayout(this);
    layout->setSpacing(2);
    layout->setContentsMargins(2,2,2,2);

    QStringList emojis={
        "😀","😂","😍","😢","😡","😎","😭","😘","🙄","😴",
        "🤯","😇","🤢","🥰","😬","😜","😳","😔","😌","🤓",
        "👻","💀","😈","😱","🧐","😷","🤧","🤠","😵","🥵",
        "🥶","😪","🤤","😤","🤬","😒","😕","🫠","🤡","🫢",
        "🙃","🫣","😚","😽","👍","👎","❤️","🤔","🎉","💩"
    };

    int columns=10,row=0,col=0;
    auto addEmojis=[&](const QStringList &emojis){
        for(const QString &e:emojis){
            QPushButton *btn=new QPushButton(e,this);
            btn->setFixedSize(38,38);
            btn->setStyleSheet(R"(
                QPushButton {
                    font-size:15px;
                    border:none;
                    background:transparent;
                }
                QPushButton:hover {
                    background:#e0e0e0;
                    border-radius:4px;
                })");
            connect(btn,&QPushButton::clicked,this,[=](){
                emit emojiSelected(e);
                this->hide();
            });
            layout->addWidget(btn,row,col);
            if(++col==columns) col=0,++row;
        }
    };

    addEmojis(emojis);

    setLayout(layout);
    setFixedSize(400,layout->sizeHint().height());
}
