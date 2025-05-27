#include "close_confirm_dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>

CloseConfirmDialog::CloseConfirmDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("退出确认");
    setFixedSize(300, 180);

    QLabel *label = new QLabel("你要退出 QChat 吗？", this);
    label->setStyleSheet("font-size: 14px; color: #333333;");

    checkBoxDontAsk = new QCheckBox("不再提示", this);
    checkBoxDontAsk->setStyleSheet("font-size: 12px; color: #666666;");

    QPushButton *btnExit = new QPushButton("退出", this);
    QPushButton *btnMinimize = new QPushButton("最小化", this);
    QPushButton *btnCancel = new QPushButton("取消", this);

    QString btnStyle = R"(
        QPushButton {
            background-color: #ff9a9e;
            border: none;
            border-radius: 10px;
            padding: 6px 12px;
            font-weight: bold;
            color: white;
        }
        QPushButton:hover {
            background-color: #fbc2eb;
        }
    )";
    btnExit->setStyleSheet(btnStyle);
    btnMinimize->setStyleSheet(btnStyle);
    btnCancel->setStyleSheet(btnStyle);

    btnExit->setFixedWidth(80);
    btnMinimize->setFixedWidth(80);
    btnCancel->setFixedWidth(80);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(btnCancel);
    btnLayout->addSpacing(10);
    btnLayout->addWidget(btnMinimize);
    btnLayout->addSpacing(10);
    btnLayout->addWidget(btnExit);
    btnLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(label);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(checkBoxDontAsk);
    mainLayout->addStretch();
    mainLayout->addLayout(btnLayout);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);
    setLayout(mainLayout);

    setStyleSheet(R"(
        QDialog {
            background-color: #ffffff;
            border-radius: 15px;
        }
    )");

    connect(btnExit, &QPushButton::clicked, this, &CloseConfirmDialog::onExitClicked);
    connect(btnMinimize, &QPushButton::clicked, this, &CloseConfirmDialog::onMinimizeClicked);
    connect(btnCancel, &QPushButton::clicked, this, [this]() {
        selectedAction = Cancel;
        reject();
    });
}

void CloseConfirmDialog::onExitClicked() {
    selectedAction = Exit;
    accept();
}

void CloseConfirmDialog::onMinimizeClicked() {
    selectedAction = Minimize;
    accept();
}

CloseConfirmDialog::Action CloseConfirmDialog::getAction() const {
    return selectedAction;
}

bool CloseConfirmDialog::dontAskAgain() const {
    return checkBoxDontAsk->isChecked();
}
