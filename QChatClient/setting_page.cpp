#include "setting_page.h"
#include "avatar_cropper.h"
#include "background_setting_dialog.h"
#include "qregularexpression.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QBuffer>

SettingPage::SettingPage(Client *client, QWidget *parent)
    : QWidget(parent), client(client) {
    client->settingPage = this;

    // 美化
    this->setStyleSheet(R"(
        QWidget {
            background-color: #f0f0f0;
            font-size: 16px;
        }

        QLabel {
            font-weight: bold;
            color: #333;
            background-color: transparent;
        }

        QPushButton {
            padding: 10px;
            border: none;
            border-radius: 12px;
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #ff9a9e, stop:1 #fad0c4);
            color: white;
            font-size: 16px;
            font-weight: bold;
        }

        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #fbc2eb, stop:1 #a6c1ee);
        }
    )");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(20);
    layout->setContentsMargins(30, 30, 30, 30);

    avatarLabel = new QLabel(this);
    avatarLabel->setFixedSize(120, 120);
    avatarLabel->setStyleSheet(R"(
        QLabel {
            border-radius: 60px;
            border: 2px solid #ddd;
            background-color: #fff;
        }
    )");
    avatarLabel->setAlignment(Qt::AlignCenter);
    avatarLabel->setScaledContents(true);
    changeAvatarButton = new QPushButton("修改头像", this);

    changeBackgroundButton = new QPushButton("修改聊天背景", this);

    QHBoxLayout *nicknameLayout = new QHBoxLayout;
    nicknameLabel = new QLabel("昵称：", this);
    changeNicknameButton = new QPushButton("修改昵称", this);
    nicknameLayout->addWidget(nicknameLabel);
    nicknameLayout->addStretch();
    nicknameLayout->addWidget(changeNicknameButton);

    QHBoxLayout *emailLayout = new QHBoxLayout;
    emailLabel = new QLabel("邮箱：", this);
    changeEmailButton = new QPushButton("修改邮箱", this);
    changeEmailButton->setStyleSheet("background: transparent; border: none; color: transparent;");
    changeEmailButton->setEnabled(false);
    emailLayout->addWidget(emailLabel);
    emailLayout->addStretch();
    emailLayout->addWidget(changeEmailButton);

    QHBoxLayout *passwordLayout = new QHBoxLayout;
    passwordLabel = new QLabel("密码：", this);
    changePasswordButton = new QPushButton("修改密码", this);
    passwordLayout->addWidget(passwordLabel);
    passwordLayout->addStretch();
    passwordLayout->addWidget(changePasswordButton);

    QHBoxLayout *interestLayout = new QHBoxLayout;
    interestLabel = new QLabel("兴趣：未设置",this);
    changeInterestButton = new QPushButton("修改兴趣",this);
    interestLayout->addWidget(interestLabel);
    interestLayout->addStretch();
    interestLayout->addWidget(changeInterestButton);

    connect(changeAvatarButton, &QPushButton::clicked, this, &SettingPage::changeAvatar);
    connect(changeBackgroundButton, &QPushButton::clicked, this, &SettingPage::changeBackground);
    connect(changeNicknameButton, &QPushButton::clicked, this, &SettingPage::changeNickname);
    connect(changePasswordButton, &QPushButton::clicked, this, &SettingPage::changePassword);
    connect(changeInterestButton, &QPushButton::clicked, this, &SettingPage::changeInterest);

    layout->addWidget(avatarLabel, 0, Qt::AlignCenter);
    layout->addWidget(changeAvatarButton, 0, Qt::AlignCenter);
    layout->addWidget(changeBackgroundButton, 0, Qt::AlignCenter);
    layout->addLayout(nicknameLayout);
    layout->addLayout(emailLayout);
    layout->addLayout(passwordLayout);
    layout->addLayout(interestLayout);

    setLayout(layout);
    updateUserInfo();
}


void SettingPage::updateUserInfo() {
    avatarLabel->setPixmap(client->avatar.scaled(200,200,Qt::KeepAspectRatio,Qt::SmoothTransformation));
    nicknameLabel->setText("昵称：" + client->nickname);
    emailLabel->setText("邮箱：" + client->email);
    passwordLabel->setText("密码："+QString(client->password.length(),QChar(0x25CF)));
    interestLabel->setText("兴趣："+((client->interest=="")?"未设置":client->interest));
}

void SettingPage::changeAvatar() {
    QString file=QFileDialog::getOpenFileName(this,"选择新头像","","Images (*.png *.jpg *.bmp)");
    if(!file.isEmpty()) {
        QPixmap pix(file);
        if(!pix.isNull()) {
            AvatarCropper cropper(pix);
            if(cropper.exec()!=QDialog::Accepted) return;
            QPixmap cropped=cropper.getCropped();
            QBuffer buffer;
            buffer.open(QIODevice::WriteOnly);
            cropped.save(&buffer,"PNG");
            QByteArray data=buffer.data();
            QString header="FILE:"+client->nickname+"_avatar.png:"+QString::number(data.size())+'\n';
            if(client->ifconnected()) {
                client->socket->write(header.toUtf8()+data);
                client->socket->flush();
                client->avatar=cropped;
            }
            else
            {
                QMessageBox *msgBox = new QMessageBox(this);
                msgBox->setWindowTitle("提示");
                msgBox->setText("请输入有效的邮箱地址");
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
            }
            updateUserInfo();
        }
    }
}

void SettingPage::changeBackground() {
    BackgroundSettingDialog dlg(client,this);
    dlg.exec();
}

void SettingPage::changeNickname() {
    bool ok;
    QString newName=QInputDialog::getText(this,"修改昵称","输入新昵称",QLineEdit::Normal,"",&ok);
    if(ok && !newName.isEmpty()) {
        client->sendNonTextMessage("CHANGE_NICKNAME|"+client->nickname+"|"+newName);
        client->newNickname=newName;
    }
}

void SettingPage::changePassword() {
    bool ok;
    QString newPassword=QInputDialog::getText(this,"修改密码","输入新密码",QLineEdit::Password,"",&ok);
    if(ok && !newPassword.isEmpty()) {
        client->sendNonTextMessage("CHANGE_PASSWORD|"+client->nickname+"|"+newPassword);
        client->password=newPassword;
        updateUserInfo();
    }
}

void SettingPage::changeInterest() {
    bool ok;
    QString hint="多个兴趣请用中文逗号分隔，如：编程，游戏，电影";
    QString newInterest=QInputDialog::getText(this,"修改兴趣",hint,QLineEdit::Normal,"",&ok);
    if(ok) {
        newInterest=newInterest.trimmed();
        // 正则：中文汉字，中文逗号隔开
        static const QRegularExpression re("^([\u4e00-\u9fa5]+)(，[\u4e00-\u9fa5]+)*$");
        if(re.match(newInterest).hasMatch()) {
            QString msg="CHANGE_INTEREST|"+newInterest;
            interestLabel->setText("兴趣："+newInterest);
            client->sendNonTextMessage(msg);
        } else {
            QMessageBox::warning(this, "格式错误", "兴趣格式应为“唱歌，跳舞，读书”等（仅允许中文逗号，且每项必须是中文）");
        }
    }
}
