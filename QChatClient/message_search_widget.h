#ifndef MESSAGE_SEARCH_WIDGET_H
#define MESSAGE_SEARCH_WIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>

class MessageSearchWidget : public QWidget {
    Q_OBJECT
public:
    explicit MessageSearchWidget(QListWidget *listWidget, QWidget *parent=nullptr);
    void activate(); // 调用后显示搜索框等
    void hideAll();  // 隐藏搜索框等（可选）

private slots:
    void onTextChanged(const QString &text);
    void onPrevClicked();
    void onNextClicked();

private:
    QListWidget *listWidget;
    QLineEdit *searchEdit;
    QPushButton *searchPrevButton;
    QPushButton *searchNextButton;

    QString lastSearchText;
    int currentSearchIndex;
};

#endif // MESSAGE_SEARCH_WIDGET_H
