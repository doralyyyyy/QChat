#include "background_setting_dialog.h"
#include "chat_page.h"
#include "chat_page2.h"
#include "client.h"
#include <QStandardPaths>

BackgroundSettingDialog::BackgroundSettingDialog(Client *client, QWidget *parent)
    : QDialog(parent),client(client) {
    colorBtn=new QPushButton("选择纯色背景");
    imageBtn=new QPushButton("选择图片背景");
    applyBtn=new QPushButton("应用");
    cancelBtn=new QPushButton("取消");
    preview=new QLabel;
    preview->setFixedSize(320,240);
    preview->setFrameShape(QFrame::Box);
    preview->setAlignment(Qt::AlignCenter);

    QVBoxLayout *btnLayout=new QVBoxLayout;
    btnLayout->addWidget(colorBtn);
    btnLayout->addWidget(imageBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(applyBtn);
    btnLayout->addWidget(cancelBtn);

    QHBoxLayout *mainLayout=new QHBoxLayout;
    mainLayout->addLayout(btnLayout);
    mainLayout->addWidget(preview);

    setLayout(mainLayout);
    setWindowTitle("设置聊天背景");
    resize(450,240);

    connect(colorBtn,&QPushButton::clicked,this,&BackgroundSettingDialog::chooseColor);
    connect(imageBtn,&QPushButton::clicked,this,&BackgroundSettingDialog::chooseImage);
    connect(applyBtn,&QPushButton::clicked,this,&BackgroundSettingDialog::applyBackground);
    connect(cancelBtn,&QPushButton::clicked,this,&QDialog::reject);
}

void BackgroundSettingDialog::chooseColor() {
    QColor color=QColorDialog::getColor(Qt::white,this,"选择颜色");
    if(color.isValid()){
        selectedPixmap=QPixmap(800,600);
        selectedPixmap.fill(color);
        preview->setPixmap(selectedPixmap.scaled(preview->size(),Qt::KeepAspectRatio));
    }
}

void BackgroundSettingDialog::chooseImage() {
    QString path=QFileDialog::getOpenFileName(this,"选择图片","","Images (*.png *.jpg *.bmp)");
    if(!path.isEmpty()){
        QPixmap pix(path);
        if(pix.isNull())return;
        selectedPixmap=pix.scaled(800,600,Qt::KeepAspectRatioByExpanding); // 简化处理，不裁剪
        preview->setPixmap(selectedPixmap.scaled(preview->size(),Qt::KeepAspectRatio));
    }
}

void BackgroundSettingDialog::applyBackground() {
    if(selectedPixmap.isNull()) {
        accept();
        return;
    }

    QListWidget *w=client->chatPage->listWidget,*w2=client->chatPage2->listWidget;
    if(!w)return;

    // 临时保存图片为文件，用于样式表设置背景图
    QString tempPath=QStandardPaths::writableLocation(QStandardPaths::TempLocation)+"/chat_bg_temp.png";
    selectedPixmap.save(tempPath);

    // 构建样式表字符串，保持圆角和 margin
    QString style=QString(R"(
        QListWidget {
            background-image: url(%1);
            background-repeat: repeat;
            background-attachment: scroll;
            background-position: center;
            background-size: cover;
            border-radius: 10px;
            margin-bottom: 10px;
        }
    )").arg(tempPath.replace("\\","/")); // 路径兼容性处理

    w->setStyleSheet(style);
    w->update();

    if(!w2)return;

    // 临时保存图片为文件，用于样式表设置背景图
    QString tempPath2=QStandardPaths::writableLocation(QStandardPaths::TempLocation)+"/chat2_bg_temp.png";
    selectedPixmap.save(tempPath2);

    // 构建样式表字符串，保持圆角和 margin
    QString style2=QString(R"(
        QListWidget {
            background-image: url(%1);
            background-repeat: no-repeat;
            background-position: center;
            background-size: cover;
            border-radius: 10px;
            margin-bottom: 10px;
        }
    )").arg(tempPath.replace("\\","/")); // 路径兼容性处理

    w2->setStyleSheet(style2);
    w2->update();

    accept();
}
