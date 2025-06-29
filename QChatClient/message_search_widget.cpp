#include "message_search_widget.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidgetItem>

MessageSearchWidget::MessageSearchWidget(QListWidget *lw, QWidget *parent)    // 用于搜索聊天记录
    : QWidget(parent), listWidget(lw), currentSearchIndex(-1) {
    searchEdit=new QLineEdit(this);
    searchEdit->setPlaceholderText("查找聊天记录...");
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

    searchEdit->installEventFilter(this);
}

void MessageSearchWidget::activate() {
    this->setVisible(true);
    searchEdit->setVisible(true);
    searchPrevButton->setVisible(true);
    searchNextButton->setVisible(true);
    searchEdit->setFocus();
}

void MessageSearchWidget::hideAll() {
    searchEdit->clear(); // 清除查找内容
    searchEdit->setVisible(false);
    searchPrevButton->setVisible(false);
    searchNextButton->setVisible(false);
    lastSearchText.clear();
    currentSearchIndex = -1;
    for(int i=0;i<listWidget->count();++i)
        listWidget->item(i)->setBackground(QBrush()); // 取消高亮
    this->setVisible(false);
}

void MessageSearchWidget::onTextChanged(const QString &text) {
    if (text.isEmpty()) {
        for (int i = 0; i < listWidget->count(); ++i)
            listWidget->item(i)->setBackground(QBrush());
        hideAll();
        return;
    }

    lastSearchText = text;
    currentSearchIndex = -1;

    // 高亮匹配项，并找到第一个匹配项作为当前索引
    for (int i = 0; i < listWidget->count(); ++i) {
        QListWidgetItem *item = listWidget->item(i);
        QString content = item->data(Qt::UserRole).toString();
        bool matched = content.contains(text, Qt::CaseInsensitive);

        if (matched) {
            if (i == currentSearchIndex) {
                item->setBackground(QBrush(QColor("#424242")));
            } else {
                item->setBackground(QBrush(QColor("#F1F1F1")));
            }
        } else {
            item->setBackground(QBrush());
        }

        if (matched && currentSearchIndex == -1) {
            currentSearchIndex = i;
        }
    }

    if (currentSearchIndex != -1) {
        listWidget->setCurrentRow(currentSearchIndex);
        listWidget->scrollToItem(listWidget->item(currentSearchIndex), QAbstractItemView::PositionAtCenter);
    }
}


void MessageSearchWidget::onPrevClicked() {
    if (lastSearchText.isEmpty()) return;

    for (int i = currentSearchIndex - 1; i >= 0; --i) {
        QListWidgetItem *item = listWidget->item(i);
        QString content = item->data(Qt::UserRole).toString();
        if (content.contains(lastSearchText, Qt::CaseInsensitive)) {
            currentSearchIndex = i;
            listWidget->setCurrentRow(i);
            listWidget->scrollToItem(item, QAbstractItemView::PositionAtCenter);
            return;
        }
    }
}

void MessageSearchWidget::onNextClicked() {
    if (lastSearchText.isEmpty()) return;

    for (int i = currentSearchIndex + 1; i < listWidget->count(); ++i) {
        QListWidgetItem *item = listWidget->item(i);
        QString content = item->data(Qt::UserRole).toString();
        if (content.contains(lastSearchText, Qt::CaseInsensitive)) {
            currentSearchIndex = i;
            listWidget->setCurrentRow(i);
            listWidget->scrollToItem(item, QAbstractItemView::PositionAtCenter);
            return;
        }
    }
}

bool MessageSearchWidget::eventFilter(QObject *obj,QEvent *event) {     // 事件过滤器
    if(obj==searchEdit&&event->type()==QEvent::KeyPress) {
        QKeyEvent *keyEvent=static_cast<QKeyEvent*>(event);
        if(keyEvent->key()==Qt::Key_Return||keyEvent->key()==Qt::Key_Enter) {
            if(keyEvent->modifiers()&Qt::ShiftModifier)
                onPrevClicked();  // Shift+Enter = 上一个
            else
                onNextClicked();  // Enter = 下一个
            return true;
        }
    }
    return QWidget::eventFilter(obj,event);
}
