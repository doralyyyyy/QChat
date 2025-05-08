#ifndef DELAYSENDDIALOG_H
#define DELAYSENDDIALOG_H
#include "server.h"
#include <QDialog>
#include <QDateTime>
#include <QTimer>

class QLineEdit;
class QPushButton;
class QStackedWidget;
class QDateTimeEdit;
class QSpinBox;

class DelaySendDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DelaySendDialog(Server *server, QWidget *parent=nullptr);

private slots:
    void switchMode(); // 切换相对/绝对
    void startDelaySend(); // 点击“确定”后处理延迟逻辑

private:
    QLineEdit *textInput;
    QWidget *relativePage;
    QSpinBox *hSpin,*mSpin,*sSpin;
    QWidget *absolutePage;
    QDateTimeEdit *timeEdit;
    QStackedWidget *stack;
    QPushButton *switchBtn;
    QPushButton *confirmBtn;
    Server *server;
};

#endif
