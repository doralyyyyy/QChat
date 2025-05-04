#include "main_window.h"
#include "ui_main_window.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // 创建聊天记录显示区
    listWidget = new QListWidget(this);
    listWidget->setSelectionMode(QAbstractItemView::NoSelection);
    listWidget->setFocusPolicy(Qt::NoFocus);
    listWidget->setStyleSheet("QListWidget { border: none; }");

    // 创建输入框和发送按钮
    inputField = new QLineEdit(this);
    sendButton = new QPushButton("发送", this);
    sendFileButton = new QPushButton("发送文件", this);

    // 创建水平布局，将按钮放到右侧
    QHBoxLayout *inputLayout = new QHBoxLayout;
    inputLayout->addWidget(inputField);
    inputLayout->addWidget(sendButton);
    inputLayout->addWidget(sendFileButton);

    // 创建垂直布局，将文本框和输入区域放到一起
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(listWidget);
    mainLayout->addLayout(inputLayout);

    // 创建 centralWidget，设置布局
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    // 启动服务器，并监听1145端口
    server = new Server(1145, this);

    connect(inputField, &QLineEdit::returnPressed, this, &MainWindow::onSendButtonClicked);
    connect(sendButton, &QPushButton::clicked, this, &MainWindow::onSendButtonClicked);
    connect(sendFileButton, &QPushButton::clicked, this, &MainWindow::onSendFileButtonClicked);

    QVector<QString> msgs = dbManager.loadMessages();
    for (const QString &line : std::as_const(msgs)) {
        updateMessage(line);
    }

}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::updateMessage(const QString &msg) {      // 将文本/文件显示到屏幕
    if (msg.contains("：FILE|")) {
        int idx=msg.indexOf("：FILE|");
        QString head=msg.left(idx+1); // 提取"[时间] 发送方："
        QString fname=msg.mid(idx+6); // 提取文件名
        QString tryPath1 = QCoreApplication::applicationDirPath()+"/received_"+fname;  // 获取要写入的文件，前缀有两种可能
        QString tryPath2 = QCoreApplication::applicationDirPath()+"/sent_"+fname;
        QString path;
        if (QFile::exists(tryPath1))
            path = tryPath1;
        else if (QFile::exists(tryPath2))
            path = tryPath2;

        // 创建主 widget 显示消息 + 按钮
        QWidget *w=new QWidget;
        QHBoxLayout *layout=new QHBoxLayout(w);
        layout->setContentsMargins(5,5,5,5);
        layout->setSpacing(10);

        QLabel *label=new QLabel(head+fname);
        QPushButton *btn=new QPushButton("打开");

        layout->addWidget(label);
        layout->addWidget(btn);
        layout->addStretch();

        connect(btn,&QPushButton::clicked,this,[path](){
            QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        });

        QListWidgetItem *item=new QListWidgetItem(listWidget);
        item->setSizeHint(QSize(0, 40));
        listWidget->setItemWidget(item,w);
    } else {
        QListWidgetItem *item=new QListWidgetItem(msg);
        item->setSizeHint(QSize(0, 40));
        listWidget->addItem(item);
    }
}

void MainWindow::onSendButtonClicked() {
    QString message = inputField->text();
    if (!message.isEmpty()) {
        server->sendMessage(message);
        inputField->clear();
    }
}

void MainWindow::onSendFileButtonClicked() {
    QString filePath = QFileDialog::getOpenFileName(this, tr("选择文件"));
    if (!filePath.isEmpty()) {
        server->sendFile(filePath);
    }
}
