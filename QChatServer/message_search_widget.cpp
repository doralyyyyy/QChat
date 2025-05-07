#include "message_search_widget.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidgetItem>

MessageSearchWidget::MessageSearchWidget(QListWidget *lw, QWidget *parent)
    : QWidget(parent), listWidget(lw), currentSearchIndex(-1) {
    searchEdit=new QLineEdit(this);
    searchEdit->setPlaceholderText("搜索聊天记录...");
    searchEdit->setClearButtonEnabled(true);
    searchEdit->setVisible(false);

    searchPrevButton=new QPushButton("上一个",this);
    searchNextButton=new QPushButton("下一个",this);
    searchPrevButton->setVisible(false);
    searchNextButton->setVisible(false);

    auto layout=new QHBoxLayout(this);
    layout->addWidget(searchEdit);
    layout->addWidget(searchPrevButton);
    layout->addWidget(searchNextButton);
    layout->setContentsMargins(0,0,0,0);

    connect(searchEdit,&QLineEdit::textChanged,this,&MessageSearchWidget::onTextChanged);
    connect(searchPrevButton,&QPushButton::clicked,this,&MessageSearchWidget::onPrevClicked);
    connect(searchNextButton,&QPushButton::clicked,this,&MessageSearchWidget::onNextClicked);
}

void MessageSearchWidget::activate() {
    searchEdit->setVisible(true);
    searchPrevButton->setVisible(true);
    searchNextButton->setVisible(true);
    searchEdit->setFocus();
}

void MessageSearchWidget::hideAll() {
    searchEdit->setVisible(false);
    searchPrevButton->setVisible(false);
    searchNextButton->setVisible(false);
    lastSearchText.clear();
    currentSearchIndex=-1;
}

void MessageSearchWidget::onTextChanged(const QString &text) {
    if(text.isEmpty()) {
        for(int i=0;i<listWidget->count();++i)
            listWidget->item(i)->setBackground(QBrush());
        hideAll();
        return;
    }

    lastSearchText=text;
    currentSearchIndex=-1;

    for(int i=0;i<listWidget->count();++i){
        QListWidgetItem *item=listWidget->item(i);
        QWidget *w=listWidget->itemWidget(item);
        QString content=w?w->findChild<QLabel *>()->text():item->text();
        item->setBackground(content.contains(text,Qt::CaseInsensitive)?QBrush(Qt::yellow):QBrush());
    }
}

void MessageSearchWidget::onPrevClicked() {
    if(lastSearchText.isEmpty()) return;
    for(int i=currentSearchIndex-1;i>=0;--i){
        QListWidgetItem *item=listWidget->item(i);
        QWidget *w=listWidget->itemWidget(item);
        QString content=w?w->findChild<QLabel *>()->text():item->text();
        if(content.contains(lastSearchText,Qt::CaseInsensitive)) {
            currentSearchIndex=i;
            listWidget->setCurrentRow(i);
            listWidget->scrollToItem(item);
            return;
        }
    }
}

void MessageSearchWidget::onNextClicked() {
    if(lastSearchText.isEmpty()) return;
    for(int i=currentSearchIndex+1;i<listWidget->count();++i){
        QListWidgetItem *item=listWidget->item(i);
        QWidget *w=listWidget->itemWidget(item);
        QString content=w?w->findChild<QLabel *>()->text():item->text();
        if(content.contains(lastSearchText,Qt::CaseInsensitive)) {
            currentSearchIndex=i;
            listWidget->setCurrentRow(i);
            listWidget->scrollToItem(item);
            return;
        }
    }
}
