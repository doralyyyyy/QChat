#include "record_dialog.h"
#include <QMediaDevices>
#include <QCoreApplication>
#include <QDir>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QUrl>
#include <QDebug>

RecordDialog::RecordDialog(QWidget *parent)
    : QDialog(parent), mediaRecorder(nullptr), mediaCaptureSession(nullptr), audioInput(nullptr)
{
    setWindowTitle("语音转文字");
    setFixedSize(250,150);  // 调整界面大小
    setStyleSheet(R"(
        QDialog {
            background-color: #f0f0f0;
            border-radius: 15px;
            padding: 15px;
        }
        QLabel {
            font-size: 14px;
            color: #333;
            font-weight: bold;
            background: transparent;
        }
        QPushButton {
            background-color: #ff7f7f;
            color: white;
            border-radius: 12px;
            padding: 10px;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton:disabled {
            background-color: #cccccc;
        }
        QPushButton:hover {
            background-color: #ff4c4c;
        }
    )");

    QVBoxLayout *layout = new QVBoxLayout(this);

    timeLabel = new QLabel("已录制：0 秒", this);
    timeLabel->setAlignment(Qt::AlignCenter);

    startButton = new QPushButton("开始录音", this);
    stopButton = new QPushButton("停止录音", this);
    stopButton->setEnabled(false); // 默认禁止停止按钮

    layout->addWidget(timeLabel);
    layout->addWidget(startButton);
    layout->addWidget(stopButton);

    // 计时器
    timer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, &RecordDialog::timePass);
    connect(startButton, &QPushButton::clicked, this, &RecordDialog::startRecording);
    connect(stopButton, &QPushButton::clicked, this, &RecordDialog::stopRecording);

    // 初始化录音相关组件
    mediaRecorder = new QMediaRecorder(this);
    mediaCaptureSession = new QMediaCaptureSession(this);
    audioInput = new QAudioInput(this);

    mediaCaptureSession->setAudioInput(audioInput);
    mediaCaptureSession->setRecorder(mediaRecorder);
}

RecordDialog::~RecordDialog()
{
}

void RecordDialog::timePass(){
    elapsedSeconds++;
    timeLabel->setText(QString("已录制：%1 秒").arg(elapsedSeconds));
}

void RecordDialog::startRecording()
{
    // 计时器
    elapsedSeconds = 0;
    timeLabel->setText("已录制：0 秒");
    timer->start(1000);

    // 设置输出路径
    audioFilePath = QCoreApplication::applicationDirPath() + "/../record.wav";
    mediaRecorder->setOutputLocation(QUrl::fromLocalFile(audioFilePath));

    // 设置录音质量与编码方式
    mediaRecorder->setQuality(QMediaRecorder::HighQuality);
    mediaRecorder->setEncodingMode(QMediaRecorder::ConstantQualityEncoding);

    mediaRecorder->record();

    startButton->setEnabled(false); // 禁止开始录音按钮
    stopButton->setEnabled(true);   // 启用停止按钮
}

void RecordDialog::stopRecording()
{
    timer->stop();
    mediaRecorder->stop();

    startButton->setEnabled(true);  // 启用开始录音按钮
    stopButton->setEnabled(false);  // 禁止停止按钮

    accept(); // 关闭对话框
}

QString RecordDialog::getAudioFilePath() const
{
    return audioFilePath;
}
