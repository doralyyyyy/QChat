#include "feedback_dialog.h"

FeedbackDialog::FeedbackDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("反馈与建议");
    resize(400, 300);

    edit=new QTextEdit(this);
    edit->setPlaceholderText("请向开发者分享您宝贵的反馈或建议...");
    sendButton=new QPushButton("发送",this);

    QVBoxLayout *layout=new QVBoxLayout(this);
    layout->addWidget(edit);
    layout->addWidget(sendButton);

    edit->setStyleSheet(R"(
        QTextEdit {
            padding: 10px;
            border: 2px solid #ccc;
            border-radius: 12px;
            background: #ffffff;
            font-size: 14px;
        }
        QTextEdit:focus {
            border-color: #ff9a9e;
        }
    )");

    connect(sendButton,&QPushButton::clicked,this,[=](){
        if(!edit->toPlainText().trimmed().isEmpty())
            emit feedbackSubmitted(edit->toPlainText());
        edit->clear();
        close();
    });
}
