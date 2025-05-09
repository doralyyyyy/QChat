#include "main_window.h"
#include "message_bubble_widget.h"
#include "ui_main_window.h"
#include "feature_menu_widget.h"
#include "record_dialog.h"
#include "camera.h"

MainWindow::MainWindow(Client *client, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), client(client) {
    ui->setupUi(this);

    // 创建聊天记录显示区
    listWidget=new QListWidget(this);
    listWidget->setSelectionMode(QAbstractItemView::NoSelection);
    listWidget->setFocusPolicy(Qt::NoFocus);
    listWidget->setStyleSheet("QListWidget { border: none; }");

    // 创建输入框和按钮
    inputField=new QLineEdit(this);
    sendButton=new QPushButton("发送",this);
    sendFileButton=new QPushButton("发送文件",this);
    delaySendButton=new QPushButton("延迟发送",this);
    recordButton=new QPushButton("语音转文字",this);
    cameraButton=new QPushButton("拍照",this);

    // 创建搜索框（提前隐藏）和搜索按钮
    searchWidget=new MessageSearchWidget(listWidget,this);
    searchWidget->hideAll();
    searchButton=new QPushButton("查找",this);

    // 创建水平布局，将按钮放到右侧
    QHBoxLayout *inputLayout=new QHBoxLayout;
    inputLayout->addWidget(cameraButton);
    inputLayout->addWidget(recordButton);
    inputLayout->addWidget(inputField);
    inputLayout->addWidget(sendButton);
    inputLayout->addWidget(sendFileButton);
    inputLayout->addWidget(delaySendButton);

    // 创建垂直布局
    QVBoxLayout *mainLayout=new QVBoxLayout;
    mainLayout->addWidget(listWidget);
    mainLayout->addLayout(inputLayout);
    mainLayout->insertWidget(0,searchButton,0,Qt::AlignRight);
    mainLayout->insertWidget(1,searchWidget);

    // 设置中央窗口
    QWidget *centralWidget=new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    // 设置菜单栏
    FeatureMenuWidget *menuWidget=new FeatureMenuWidget(this);
    addToolBar(Qt::TopToolBarArea,menuWidget->getToolBar());
    connect(menuWidget, &FeatureMenuWidget::wordCloudRequested, this, &MainWindow::onWordCloudRequested);
    connect(menuWidget, &FeatureMenuWidget::relationAnalysisRequested, this, &MainWindow::onRelationAnalysisRequested);
    connect(menuWidget, &FeatureMenuWidget::exportChatToPdfRequested, this, &MainWindow::onexportChatToPdfRequested);

    connect(inputField,&QLineEdit::returnPressed,this,&MainWindow::onSendButtonClicked);
    connect(sendButton,&QPushButton::clicked,this,&MainWindow::onSendButtonClicked);
    connect(sendFileButton,&QPushButton::clicked,this,&MainWindow::onSendFileButtonClicked);
    connect(delaySendButton,&QPushButton::clicked,this,&MainWindow::onDelaySendClicked);
    connect(searchButton, &QPushButton::clicked, this, &MainWindow::onSearchButtonClicked);
    connect(recordButton,&QPushButton::clicked,this,&MainWindow::onRecordButtonClicked);
    connect(cameraButton,&QPushButton::clicked,this,&MainWindow::onCameraButtonClicked);

    client->mainWindow=this;

    QVector<QString> msgs=dbManager.loadMessages();
    for(const QString &line:std::as_const(msgs))
        updateMessage(line);
}

void MainWindow::updateMessage(const QString &msg) {
    int timeEnd = msg.indexOf(']');
    int nameEnd = msg.indexOf("：", timeEnd);
    if (timeEnd == -1 || nameEnd == -1) return;

    QString time = msg.left(timeEnd + 1); // [时间]
    QString sender = msg.mid(timeEnd + 1, nameEnd - timeEnd - 1).trimmed(); // 发件人
    QString content = msg.mid(nameEnd + 1); // 内容（支持带冒号）

    bool isSelf = (sender == "我");

    messages.append(Message(time, sender, content));

    MessageBubbleWidget *messageBubble = new MessageBubbleWidget(time, sender, content, isSelf);

    QListWidgetItem *item = new QListWidgetItem(listWidget);
    item->setSizeHint(messageBubble->sizeHint());
    item->setTextAlignment(isSelf ? Qt::AlignRight : Qt::AlignLeft);

    // 存储消息的内容到 QListWidgetItem 的 data 中
    item->setData(Qt::UserRole, content);

    listWidget->addItem(item);
    listWidget->setItemWidget(item, messageBubble);
    listWidget->scrollToBottom();  // 始终滚动到最底部
}

void MainWindow::onSendButtonClicked() {
    QString message = inputField->text();  // 获取输入框中的文本
    if (!message.isEmpty()) {
        client->sendMessage(message);
        inputField->clear();    // 清空输入框
    }
}

void MainWindow::onSendFileButtonClicked() {
    QString filePath = QFileDialog::getOpenFileName(this, tr("选择文件"));
    if (!filePath.isEmpty()) {
        client->sendFile(filePath);
    }
}

void MainWindow::onDelaySendClicked() {
    DelaySendDialog *dialog=new DelaySendDialog(client, this);
    dialog->exec();
}

void MainWindow::onSearchButtonClicked() {
    if (searchWidget->isVisible()) {
        searchWidget->hideAll();
    } else {
        searchWidget->activate();
    }
}

void MainWindow::exportChatToTxt() {      // 把聊天内容打包成 chat_log.txt
    QFile file("chat_log.txt");
    if(file.open(QIODevice::WriteOnly|QIODevice::Text)) {
        QTextStream out(&file);
        for(int i=0;i<listWidget->count();++i) {
            QListWidgetItem *item=listWidget->item(i);
            QString content=item->data(Qt::UserRole).toString();
            out<<content<<'\n';
        }
        file.close();
    }
}

void MainWindow::onWordCloudRequested() {
    exportChatToTxt();
    QString scriptPath = QCoreApplication::applicationDirPath() + "/../../../wordcloud_gen.py";

    QProcess p;
    p.start("python", QStringList() << scriptPath);
    if (!p.waitForFinished()) {
        QMessageBox::warning(this, "错误", "词云生成脚本执行失败。");
        return;
    }

    QByteArray stderrOutput = p.readAllStandardError();
    QString errorText = QString::fromLocal8Bit(stderrOutput);

    if (errorText.contains("ValueError") && errorText.contains("at least 1 word")) {
        QMessageBox::warning(this, "词云生成失败", "聊天记录中没有有效词汇，无法生成词云。");
    }
}

void MainWindow::onRelationAnalysisRequested() {
    exportChatToTxt(); // 导出聊天记录到 chat_log.txt

    QString scriptPath = QCoreApplication::applicationDirPath() + "/../../../relationship_ai_analyzer.py";
    QString chatPath = QCoreApplication::applicationDirPath() + "/../chat_log.txt";
    QString resultPath = QCoreApplication::applicationDirPath() + "/../relationship_result.txt";

    QProcess process;
    process.start("python", QStringList() << scriptPath << chatPath);
    process.waitForFinished(-1); // 等待执行完成

    QFile f(resultPath);
    if(f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString result = QString::fromUtf8(f.readAll());
        f.close();
        QMessageBox::information(this, "关系分析结果", result);
    } else {
        QMessageBox::warning(this, "错误", "无法读取分析结果文件。");
    }
}

void MainWindow::onexportChatToPdfRequested() {
    QTextDocument document;

    // 遍历 messages 列表，将每条消息的内容、发送者和时间拼接成格式化文本
    QString formattedContent;
    for (const Message &msg : std::as_const(messages)) {
        formattedContent += msg.time + " " + msg.sender + "：\n" + msg.content + "\n\n";
    }

    document.setPlainText(formattedContent);  // 设置文本内容

    // 创建 PDF 打印机对象
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);

    // 弹出文件保存对话框，让用户选择保存位置
    QString filePath = QFileDialog::getSaveFileName(this, "保存为 PDF", "", "*.pdf");
    if (filePath.isEmpty()) {
        return; // 如果用户取消了保存，直接退出
    }

    printer.setOutputFileName(filePath); // 设置输出的 PDF 文件路径

    // 打印文档内容到 PDF 文件
    document.print(&printer);

    QMessageBox::information(this, "成功", "聊天记录已成功导出为 PDF");
}

void MainWindow::onRecordButtonClicked() {
    RecordDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        QString filePath = dlg.getAudioFilePath();
        QProcess *p = new QProcess(this);

        // 设置环境变量：确保 ffmpeg 能被 Python 脚本访问
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert("PATH", env.value("PATH") + ";" + QCoreApplication::applicationDirPath() + "/../../../ffmpeg/bin");
        p->setProcessEnvironment(env);

        QString program = "python";
        QStringList args;
        QString scriptPath = QCoreApplication::applicationDirPath() + "/../../../speech_to_text.py";
        args << scriptPath << filePath;
        p->start(program, args);

        connect(p, &QProcess::readyReadStandardOutput, this, [=]() {
            QByteArray output = p->readAllStandardOutput();
            QString result = QString::fromUtf8(output).trimmed();
            inputField->insert(result);
        });

        connect(p, &QProcess::errorOccurred, this, [=](QProcess::ProcessError err) {
            QMessageBox::warning(this, "错误", "Python 脚本启动失败：" + QString::number(err));
        });
    }
}

void MainWindow::onCameraButtonClicked(){
    Camera *cam=new Camera(client,this);
    cam->show();
}

MainWindow::~MainWindow() {
    delete ui;
}
