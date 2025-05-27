#include "chat_page2.h"
#include "feature_menu_widget.h"
#include "feedback_dialog.h"
#include "record_dialog.h"
#include "file_confirm_dialog.h"
#include <QMimeData>
#include <QUrl>
#include <QToolButton>
#include <QSettings>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <QChart>
#include <QMap>
#include <QTime>
#include <QBarSet>
#include <QBarSeries>
#include <QBarCategoryAxis>

ChatPage2::ChatPage2(Client *client, QWidget *parent)
    : QWidget(parent), client(client) {
    this->setWindowTitle("聊天界面");

    // 创建聊天记录显示区
    listWidget = new QListWidget(this);
    listWidget->setSelectionMode(QAbstractItemView::NoSelection);
    listWidget->setFocusPolicy(Qt::NoFocus);
    listWidget->setStyleSheet("QListWidget { border: none; }");

    // 创建输入框和按钮
    inputField = new QLineEdit(this);
    sendButton = new QPushButton("发送", this);

    // 表情包按钮
    emojiButton = new QPushButton(this);
    emojiButton->setIcon(QIcon(QCoreApplication::applicationDirPath() + "/../../../icons/smile_icon.png"));
    emojiButton->setIconSize(QSize(28,28));
    emojiButton->setFixedSize(24, 24);
    emojiButton->setStyleSheet(R"(
        QPushButton {
            border: none;
            background: transparent;
        }
        QPushButton:hover {
            background:#e0e0e0;
            border-radius:4px;
        })");
    emojiPicker = new EmojiPicker(this);
    emojiPicker->hide();

    connect(emojiButton, &QPushButton::clicked, this, [=]() {
        QPoint pos = emojiButton->mapToGlobal(QPoint(0, emojiButton->height()));
        emojiPicker->move(pos);
        emojiPicker->show();
    });

    connect(emojiPicker, &EmojiPicker::emojiSelected, this, [=](const QString &emoji) {
        inputField->insert(emoji);
    });

    // 创建下拉菜单按钮
    QToolButton *menuButton = new QToolButton(this);
    menuButton->setText("▼");
    menuButton->setStyleSheet("QToolButton { background: transparent; font-size: 18px; }");

    // 创建下拉菜单
    QMenu *menu = new QMenu(menuButton);
    QAction *sendFileAction = menu->addAction("发送文件");
    QAction *delaySendAction = menu->addAction("延迟发送");

    //设置下拉菜单
    menu->setStyleSheet(R"(
        QMenu {
            background-color: #ffffff;  /* 设置菜单背景色 */
            border-radius: 8px;
            box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1); /* 阴影效果 */
        }

        QMenu::item {
            padding: 10px;
            color: #555;
            font-size: 14px;
            border-radius: 6px;
        }

        QMenu::item:selected {
            background-color: #fbc2eb;
            color: white;
        }

        QMenu::item:pressed {
            background-color: #a6c1ee;
        }
    )");
    // 将下拉菜单绑定到按钮
    menuButton->setMenu(menu);
    menuButton->setPopupMode(QToolButton::InstantPopup);

    connect(sendFileAction, &QAction::triggered, this, &ChatPage2::onSendFileButtonClicked);
    connect(delaySendAction, &QAction::triggered, this, &ChatPage2::onDelaySendClicked);

    // 创建其他按钮
    recordButton = new QPushButton("语音转文字", this);
    cameraButton = new QPushButton("拍照", this);

    // 创建搜索框（提前隐藏）和搜索按钮
    searchWidget = new MessageSearchWidget(listWidget, this);
    searchWidget->hideAll();
    searchButton = new QPushButton("查找", this);

    // 创建水平布局，将按钮放到右侧
    QHBoxLayout *inputLayout = new QHBoxLayout;
    inputLayout->addWidget(cameraButton);
    inputLayout->addWidget(recordButton);
    inputLayout->addWidget(inputField);
    inputLayout->addWidget(emojiButton);
    inputLayout->addWidget(sendButton);
    inputLayout->addWidget(menuButton);  // 将菜单按钮添加到布局中

    // 创建垂直布局
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(listWidget);
    mainLayout->addLayout(inputLayout);
    mainLayout->insertWidget(0, searchButton, 0, Qt::AlignRight);
    mainLayout->insertWidget(1, searchWidget);

    // 设置中央窗口
    setLayout(mainLayout);

    // 设置右键菜单
    contextMenu=new MessageContextMenuHandler(listWidget,this);

    // 清除关闭记忆（正常应被注释掉）
    // QSettings s("MyCompany", "QChat");
    // s.remove("close_behavior");

    // 创建菜单栏功能区
    FeatureMenuWidget *menuWidget=new FeatureMenuWidget(this);
    QToolBar *toolbar=menuWidget->getToolBar();
    feedbackDialog=new FeedbackDialog(this);

    // 顶部显示对方名字
    chatTitleLabel = new QLabel("KKK", this);
    chatTitleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");

    // 顶部横向布局：菜单栏 + 拉伸 + 搜索按钮
    QHBoxLayout *topRightLayout=new QHBoxLayout;
    topRightLayout->addWidget(toolbar);
    topRightLayout->addWidget(chatTitleLabel);
    topRightLayout->addStretch();
    topRightLayout->addWidget(searchButton);

    // 放进一个 widget 以便插入主垂直布局
    QWidget *topRightWidget=new QWidget(this);
    topRightWidget->setLayout(topRightLayout);

    // 插入主布局顶部
    mainLayout->insertWidget(0,topRightWidget);

    connect(menuWidget, &FeatureMenuWidget::wordCloudRequested, this, &ChatPage2::onWordCloudRequested);
    connect(menuWidget, &FeatureMenuWidget::relationAnalysisRequested, this, &ChatPage2::onRelationAnalysisRequested);
    connect(menuWidget, &FeatureMenuWidget::exportChatToPdfRequested, this, &ChatPage2::onExportChatToPdfRequested);
    connect(menuWidget, &FeatureMenuWidget::generateTimelineRequested, this, &ChatPage2::onGenerateTimelineRequested);
    connect(menuWidget, &FeatureMenuWidget::feedbackRequested, this, &ChatPage2::onFeedbackRequested);
    connect(feedbackDialog, &FeedbackDialog::feedbackSubmitted, this, &ChatPage2::onFeedbackSend);

    // 连接控件与槽函数
    connect(inputField, &QLineEdit::returnPressed, this, &ChatPage2::onSendButtonClicked);
    connect(sendButton, &QPushButton::clicked, this, &ChatPage2::onSendButtonClicked);
    connect(searchButton, &QPushButton::clicked, this, &ChatPage2::onSearchButtonClicked);
    connect(recordButton, &QPushButton::clicked, this, &ChatPage2::onRecordButtonClicked);
    connect(cameraButton, &QPushButton::clicked, this, &ChatPage2::onCameraButtonClicked);

    setAcceptDrops(true);

    // 设置界面样式
    setStyleSheet(R"(
        QWidget {
            background-color: #f0f0f0; /* 设置聊天界面背景色 */
            border-radius: 20px;
        }

        QListWidget {
            background-color: #ffffff; /* 设置聊天记录背景为白色 */
            border-radius: 10px;
            margin-bottom: 10px;
        }

        QLineEdit {
            padding: 10px;
            border: 2px solid #ccc;
            border-radius: 12px;
            background: #ffffff;
            font-size: 14px;
        }

        QLineEdit:focus {
            border-color: #ff9a9e;
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

        QPushButton#sendButton {
            background-color: #42b983;
        }

        QPushButton#sendFileButton {
            background-color: #ff7043;
        }

        QPushButton#delaySendButton {
            background-color: #ffeb3b;
        }

        QPushButton#cameraButton {
            background-color: #f5a623;
        }

        QPushButton#recordButton {
            background-color: #4a90e2;
        }

        QPushButton#searchButton {
            background-color: transparent;
            color: #007bff;
            text-decoration: underline;
        }

        QToolButton {
            background: transparent;
            color: #007bff;
        }

        QHBoxLayout {
            margin: 5px 0;
        }
    )");
}

void ChatPage2::popMessageBox(){
    QMessageBox *msgBox = new QMessageBox(this);
    msgBox->setWindowTitle("错误");
    msgBox->setText("对方离线中");
    msgBox->setIcon(QMessageBox::Warning);
    msgBox->setStandardButtons(QMessageBox::Ok);

    msgBox->setStyleSheet(R"(
        QMessageBox {
            background-color: #ffe6e6;
            border-radius: 15px;
            padding: 15px;
        }
        QLabel {
            font-size: 14px;
            color: #ff4444;
        }
        QPushButton {
            background-color: #ff7f7f;
            border: none;
            border-radius: 10px;
            color: white;
            font-size: 14px;
            padding: 8px;
        }
        QPushButton:hover {
            background-color: #ff4c4c;
        }
    )");
    msgBox->exec();
}

void ChatPage2::refresh(){
    listWidget->scrollToBottom();
    listWidget->viewport()->update();
}

void ChatPage2::onSendButtonClicked() {
    popMessageBox();
}

void ChatPage2::onSendFileButtonClicked() {
    popMessageBox();
}

void ChatPage2::onDelaySendClicked() {
    popMessageBox();
}

void ChatPage2::dragEnterEvent(QDragEnterEvent *e) {
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void ChatPage2::dropEvent(QDropEvent *e) {
    if(e) popMessageBox();
}

void ChatPage2::onSearchButtonClicked() {
    if (searchWidget->isVisible()) {
        searchWidget->hideAll();
    } else {
        searchWidget->activate();
    }
}

void ChatPage2::exportChatToTxt() {      // 把聊天内容打包成 chat_log.txt
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

void ChatPage2::onWordCloudRequested() {
    exportChatToTxt();
    QString scriptPath = QCoreApplication::applicationDirPath() + "/../../../wordcloud_gen.py";

    QProcess p;
    p.start("python", QStringList() << scriptPath);
    if (!p.waitForFinished()) {
        QMessageBox *msgBox = new QMessageBox(this);
        msgBox->setWindowTitle("错误");
        msgBox->setText("词云生成脚本执行失败。");
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
        return;
    }

    QByteArray stderrOutput = p.readAllStandardError();
    QString errorText = QString::fromLocal8Bit(stderrOutput);

    if (errorText.contains("ValueError") && errorText.contains("at least 1 word")) {
        QMessageBox *msgBox = new QMessageBox(this);
        msgBox->setWindowTitle("词云生成失败");
        msgBox->setText("聊天记录中没有有效词汇，无法生成词云。");
        msgBox->setIcon(QMessageBox::Warning);
        msgBox->setStandardButtons(QMessageBox::Ok);

        msgBox->setStyleSheet(R"(
            QMessageBox {
                background-color: #ffe6e6;
                border-radius: 15px;
                padding: 15px;
            }
            QLabel {
                font-size: 14px;
                color: #ff4444;
            }
            QPushButton {
                background-color: #ff7f7f;
                border: none;
                border-radius: 10px;
                color: white;
                font-size: 14px;
                padding: 8px;
            }
            QPushButton:hover {
                background-color: #ff4c4c;
            }
        )");

        msgBox->exec();
    }
}

void ChatPage2::onRelationAnalysisRequested() {
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
        QMessageBox *msgBox = new QMessageBox(this);
        msgBox->setWindowTitle("错误");
        msgBox->setText("无法读取分析结果文件");
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
}

void ChatPage2::onExportChatToPdfRequested() {
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

void ChatPage2::onGenerateTimelineRequested() {
    if (messages.isEmpty()) {
        QMessageBox::warning(this, "提示", "暂无消息记录，无法生成时间轴。");
        return;
    }

    QVector<int> hourCount(24, 0);
    for (const Message &m : std::as_const(messages)) {

        QString timeString = m.time.mid(12, 17);
        timeString = timeString.mid(0,5);
        QTime time = QTime::fromString(timeString, "HH:mm");
        if (time.isValid()) {
            hourCount[time.hour()]++;  // 统计对应小时的消息数
        }
    }

    QBarSet *set = new QBarSet("消息数");
    for (int i : std::as_const(hourCount)) *set << i;

    QBarSeries *series = new QBarSeries;
    series->append(set);

    QChart *chart = new QChart;
    chart->addSeries(series);
    chart->setTitle("24小时消息时间轴");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QStringList hours;
    for (int i = 0; i < 24; ++i) {
        hours << QString("%1:00").arg(i, 2, 10, QChar('0'));  // 格式化小时为 00:00，01:00，...
    }

    QBarCategoryAxis *axisX = new QBarCategoryAxis;
    axisX->append(hours);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis;
    int maxVal = *std::max_element(hourCount.begin(), hourCount.end());
    axisY->setRange(0, maxVal + 1);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QChartView *view = new QChartView(chart);
    view->setRenderHint(QPainter::Antialiasing);
    view->resize(1200, 600);
    view->setWindowTitle("消息时间轴");
    view->show();
}

void ChatPage2::onFeedbackRequested() {
    feedbackDialog->exec();
}

void ChatPage2::onFeedbackSend(const QString &feedback) {
    QString msg="FEEDBACK|"+feedback;
    client->sendNonTextMessage(msg);
}

void ChatPage2::onRecordButtonClicked() {
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
            QMessageBox *msgBox = new QMessageBox(this);
            msgBox->setWindowTitle("错误");
            msgBox->setText("Python 脚本启动失败：" + QString::number(err));
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
        });
    }
}

void ChatPage2::onCameraButtonClicked(){
    popMessageBox();
}

ChatPage2::~ChatPage2() {
}
