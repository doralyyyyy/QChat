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
    setFixedSize(150,100);
    QVBoxLayout *layout = new QVBoxLayout(this);

    timeLabel = new QLabel("已录制：0 秒", this);
    startButton = new QPushButton("开始录音", this);
    stopButton = new QPushButton("停止录音", this);
    stopButton->setEnabled(false);

    layout->insertWidget(0, timeLabel);
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

    startButton->setEnabled(false);
    stopButton->setEnabled(true);
}

void RecordDialog::stopRecording()
{
    timer->stop();
    mediaRecorder->stop();

    startButton->setEnabled(true);
    stopButton->setEnabled(false);

    accept(); // 关闭对话框
}

QString RecordDialog::getAudioFilePath() const
{
    return audioFilePath;
}
