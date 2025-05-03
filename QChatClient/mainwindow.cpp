#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    textEdit = new QTextEdit(this);    // 创建显示框
    textEdit->setReadOnly(true);       // 设置为只读
    inputField = new QLineEdit(this);  // 创建输入框
    sendButton = new QPushButton("发送",this);     // 创建发送按钮

    // 创建水平布局，将按钮放到右侧
    QHBoxLayout *inputLayout = new QHBoxLayout;
    inputLayout->addWidget(inputField);
    inputLayout->addWidget(sendButton);

    // 创建垂直布局，将文本框和输入区域放到一起
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(textEdit);
    mainLayout->addLayout(inputLayout);

    // 创建 centralWidget，设置布局
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    // 启动客户端，主机为本机，连接 1145 端口
    client = new Client("10.7.54.26", 1145, this);

    connect(sendButton, &QPushButton::clicked, this, &MainWindow::onSendButtonClicked);
    connect(inputField, &QLineEdit::returnPressed, this, &MainWindow::onSendButtonClicked);

    QVector<QString> msgs=dbManager.loadMessages();       //恢复历史记录
    for(const QString &line:std::as_const(msgs)) {
        textEdit->append(line);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateMessage(const QString &message)
{
    textEdit->append(message);  // 将新信息追加到 QTextEdit
}

void MainWindow::onSendButtonClicked()
{
    QString message = inputField->text();  // 获取输入框中的文本
    if (!message.isEmpty()) {
        client->sendMessage(message);
        textEdit->append("我："+message);   // 显示自己发的消息
        inputField->clear();  // 清空输入框
    }
}
