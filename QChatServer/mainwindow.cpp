#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 创建显示框
    textEdit = new QTextEdit(this);
    textEdit->setReadOnly(true);  // 设置为只读

    // 创建输入框
    inputField = new QLineEdit(this);

    // 创建发送按钮
    sendButton = new QPushButton("发送",this);

    // 创建水平布局，将按钮放到右侧
    QHBoxLayout *inputLayout = new QHBoxLayout;
    inputLayout->addWidget(inputField);  // 添加输入框
    inputLayout->addWidget(sendButton);  // 添加按钮

    // 创建垂直布局，将文本框和输入区域放到一起
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(textEdit);        // 显示消息区域
    mainLayout->addLayout(inputLayout);     // 输入框 + 按钮布局

    // 创建 centralWidget，设置布局
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    // 启动服务端，监听端口 1145
    server = new Server(1145,this);

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
    textEdit->append(message);  // 将新信息追加到显示框
}

void MainWindow::onSendButtonClicked()
{
    QString message = inputField->text();  // 获取输入框中的文本
    if (!message.isEmpty()) {
        server->sendMessage(message);
        textEdit->append("我："+message);   // 显示自己发的消息
        inputField->clear();  // 清空输入框
    }
}
