#include "setting_page.h"
#include "avatar_cropper.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QBuffer>

SettingPage::SettingPage(Client *client, QWidget *parent)
    : QWidget(parent), client(client) {
    client->settingPage=this;

    QVBoxLayout *layout=new QVBoxLayout(this);

    avatarLabel=new QLabel(this);
    avatarLabel->setFixedSize(100,100);
    avatarLabel->setScaledContents(true);

    nicknameLabel=new QLabel("昵称：", this);
    emailLabel=new QLabel("邮箱：", this);

    changeNicknameButton=new QPushButton("修改昵称", this);
    changeAvatarButton=new QPushButton("修改头像", this);

    connect(changeNicknameButton,&QPushButton::clicked,this,&SettingPage::changeNickname);
    connect(changeAvatarButton,&QPushButton::clicked,this,&SettingPage::changeAvatar);

    layout->addWidget(avatarLabel);
    layout->addWidget(changeAvatarButton);
    layout->addWidget(nicknameLabel);
    layout->addWidget(changeNicknameButton);
    layout->addWidget(emailLabel);

    setLayout(layout);
    updateUserInfo();
}

void SettingPage::updateUserInfo() {
    nicknameLabel->setText("昵称：" + client->nickname);
    emailLabel->setText("邮箱：" + client->email);
    avatarLabel->setPixmap(client->avatar.scaled(100,100,Qt::KeepAspectRatio,Qt::SmoothTransformation));
}

void SettingPage::changeNickname() {
    bool ok;
    QString newName=QInputDialog::getText(this,"修改昵称","输入新昵称",QLineEdit::Normal,"",&ok);
    if(ok && !newName.isEmpty()) {
        client->sendNonTextMessage("CHANGE_NICKNAME|"+client->nickname+"|"+newName);
        client->newNickname=newName;
    }
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
            else QMessageBox::warning(nullptr, "提示", "修改失败，请检查您的网络连接！");
            updateUserInfo();
        }
    }
}
