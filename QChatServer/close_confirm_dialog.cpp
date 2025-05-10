#include "close_confirm_dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

CloseConfirmDialog::CloseConfirmDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("退出确认");
    resize(300, 140);

    QLabel *label = new QLabel("你要退出 QChat 吗？", this);
    QPushButton *btnExit = new QPushButton("退出 QChat", this);
    QPushButton *btnMinimize = new QPushButton("最小化到托盘", this);
    QPushButton *btnCancel = new QPushButton("取消", this);
    checkBoxDontAsk = new QCheckBox("不再提示", this);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(btnCancel);
    btnLayout->addWidget(btnMinimize);
    btnLayout->addWidget(btnExit);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(label);
    mainLayout->addWidget(checkBoxDontAsk);
    mainLayout->addLayout(btnLayout);
    setLayout(mainLayout);

    connect(btnExit, &QPushButton::clicked, this, &CloseConfirmDialog::onExitClicked);
    connect(btnMinimize, &QPushButton::clicked, this, &CloseConfirmDialog::onMinimizeClicked);
    connect(btnCancel, &QPushButton::clicked, this, [this]() {
        selectedAction=Cancel;
        reject();
    });
}

void CloseConfirmDialog::onExitClicked() {
    selectedAction=Exit;
    accept();
}

void CloseConfirmDialog::onMinimizeClicked() {
    selectedAction=Minimize;
    accept();
}

CloseConfirmDialog::Action CloseConfirmDialog::getAction() const {
    return selectedAction;
}

bool CloseConfirmDialog::dontAskAgain() const {
    return checkBoxDontAsk->isChecked();
}
