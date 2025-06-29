#include "delay_send_dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QDateTimeEdit>
#include <QSpinBox>
#include <QStackedWidget>
#include <QTimer>
#include <QMessageBox>

DelaySendDialog::DelaySendDialog(Client *client, QWidget *parent) : QDialog(parent), client(client) {
    setWindowTitle("延迟发送");
    setFixedSize(300,250);

    textInput=new QLineEdit(this);
    textInput->setPlaceholderText("请输入要发送的消息");

    // 相对时间设置
    hSpin=new QSpinBox(this);
    hSpin->setRange(0,23);
    hSpin->setFixedSize(60,25); // 给它一个合理尺寸

    mSpin=new QSpinBox(this);
    mSpin->setRange(0,59);
    mSpin->setFixedSize(60,25);

    sSpin=new QSpinBox(this);
    sSpin->setRange(0,59);
    sSpin->setFixedSize(60,25);

    // 设置样式，确保按钮不被压缩
    QString spinStyle =
        "QSpinBox::up-button { width: 16px; height: 12px; }"
        "QSpinBox::down-button { width: 16px; height: 12px; }"
        "QSpinBox::up-arrow, QSpinBox::down-arrow { width: 8px; height: 8px; }";

    hSpin->setStyleSheet(spinStyle);
    mSpin->setStyleSheet(spinStyle);
    sSpin->setStyleSheet(spinStyle);

    // 保证布局不要太挤压
    QHBoxLayout *relativeLayout=new QHBoxLayout;
    relativeLayout->addWidget(new QLabel("时:"));
    relativeLayout->addWidget(hSpin);
    relativeLayout->addSpacing(10);
    relativeLayout->addWidget(new QLabel("分:"));
    relativeLayout->addWidget(mSpin);
    relativeLayout->addSpacing(10);
    relativeLayout->addWidget(new QLabel("秒:"));
    relativeLayout->addWidget(sSpin);

    relativePage=new QWidget(this);
    relativePage->setLayout(relativeLayout);

    // 绝对时间设置
    timeEdit=new QDateTimeEdit(QDateTime::currentDateTime(),this);
    timeEdit->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    timeEdit->setCalendarPopup(true);

    QVBoxLayout *absoluteLayout=new QVBoxLayout;
    absoluteLayout->addWidget(new QLabel("发送时间:"));
    absoluteLayout->addWidget(timeEdit);

    absolutePage=new QWidget(this);
    absolutePage->setLayout(absoluteLayout);

    // 切换按钮 + 确定按钮
    switchBtn=new QPushButton("切换为绝对时间",this);
    confirmBtn=new QPushButton("确定发送",this);

    // 页面切换
    stack=new QStackedWidget(this);
    stack->addWidget(relativePage);
    stack->addWidget(absolutePage);
    stack->setCurrentIndex(0);

    QVBoxLayout *mainLayout=new QVBoxLayout(this);
    mainLayout->addWidget(new QLabel("消息内容:"));
    mainLayout->addWidget(textInput);
    mainLayout->addWidget(stack);
    mainLayout->addWidget(switchBtn);
    mainLayout->addWidget(confirmBtn);
    setLayout(mainLayout);

    connect(switchBtn,&QPushButton::clicked,this,&DelaySendDialog::switchMode);
    connect(confirmBtn,&QPushButton::clicked,this,&DelaySendDialog::startDelaySend);
}

void DelaySendDialog::switchMode()
{
    int index=stack->currentIndex();
    if(index==0){
        stack->setCurrentIndex(1);
        switchBtn->setText("切换为相对时间");
    }else{
        stack->setCurrentIndex(0);
        switchBtn->setText("切换为绝对时间");
    }
}

void DelaySendDialog::startDelaySend()
{
    QString msg=textInput->text().trimmed();
    if(msg.isEmpty()){
        QMessageBox *msgBox = new QMessageBox(this);
        msgBox->setWindowTitle("错误");
        msgBox->setText("消息不能为空");
        msgBox->setIcon(QMessageBox::Warning);
        msgBox->setStandardButtons(QMessageBox::Ok);

        msgBox->setStyleSheet(R"(
            QMessageBox {
                background-color: #fff3f3;
                border-radius: 15px;
                padding: 20px;
                box-shadow: 0px 4px 12px rgba(0, 0, 0, 0.1);
            }
            QLabel {
                font-size: 14px;
                color: #ff4444;
                background: transparent;
            }
            QPushButton {
                background-color: #ff9a9e;
                border: none;
                border-radius: 10px;
                padding: 8px;
                font-weight: bold;
                color: white;
            }
            QPushButton:hover {
                background-color: #fbc2eb;
            }
        )");

        msgBox->exec();
        return;
    }

    qint64 delayMs=0;

    if(stack->currentIndex()==0){
        int h=hSpin->value();
        int m=mSpin->value();
        int s=sSpin->value();
        delayMs=(h*3600+m*60+s)*1000;
    }else{
        QDateTime now=QDateTime::currentDateTime();
        QDateTime target=timeEdit->dateTime();
        delayMs=now.msecsTo(target);
        if(delayMs<=0){
            QMessageBox *msgBox = new QMessageBox(this);
            msgBox->setWindowTitle("错误");
            msgBox->setText("请选择未来的时间");
            msgBox->setIcon(QMessageBox::Warning);
            msgBox->setStandardButtons(QMessageBox::Ok);

            msgBox->setStyleSheet(R"(
                QMessageBox {
                    background-color: #fff3f3;
                    border-radius: 15px;
                    padding: 20px;
                    box-shadow: 0px 4px 12px rgba(0, 0, 0, 0.1);
                }
                QLabel {
                    font-size: 14px;
                    color: #ff4444;
                    background: transparent;
                }
                QPushButton {
                    background-color: #ff9a9e;
                    border: none;
                    border-radius: 10px;
                    padding: 8px;
                    font-weight: bold;
                    color: white;
                }
                QPushButton:hover {
                    background-color: #fbc2eb;
                }
            )");

            msgBox->exec();
            return;
        }
    }

    QTimer::singleShot(delayMs,this,[=](){
        client->sendMessage(msg);
    });
    this->close();
}
