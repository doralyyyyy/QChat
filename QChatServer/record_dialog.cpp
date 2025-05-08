#include "record_dialog.h"
#include <QAudioDevice>
#include <QMediaDevices>
#include <QMessageBox>
#include <QDir>
#include <QDebug>

RecordDialog::RecordDialog(QWidget *parent)
    : QDialog(parent), audioSource(nullptr)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    startButton = new QPushButton("开始录音", this);
    stopButton = new QPushButton("停止录音", this);
    stopButton->setEnabled(false);

    layout->addWidget(startButton);
    layout->addWidget(stopButton);

    connect(startButton, &QPushButton::clicked, this, &RecordDialog::startRecording);
    connect(stopButton, &QPushButton::clicked, this, &RecordDialog::stopRecording);
}

RecordDialog::~RecordDialog()
{
    if (audioSource) {
        audioSource->stop();
        delete audioSource;
    }
    if (audioFile.isOpen()) {
        audioFile.close();
    }
}

void RecordDialog::startRecording()
{
    audioFilePath = QDir::tempPath() + "/record.wav";
    audioFile.setFileName(audioFilePath);
    if (!audioFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::warning(this, "错误", "无法打开音频文件进行写入。");
        return;
    }

    QAudioDevice inputDevice = QMediaDevices::defaultAudioInput();
    audioFormat = inputDevice.preferredFormat(); // 直接使用推荐格式

    if (!inputDevice.isFormatSupported(audioFormat)) {
        QMessageBox::warning(this, "错误", "当前设备不支持推荐音频格式。");
        audioFile.close();
        return;
    }

    writeWavHeader(); // 写入 WAV 文件头

    audioSource = new QAudioSource(inputDevice, audioFormat, this);
    audioSource->start(&audioFile); // 开始录音

    startButton->setEnabled(false);
    stopButton->setEnabled(true);
}

void RecordDialog::stopRecording()
{
    if (audioSource) {
        audioSource->stop();
    }

    finalizeWavFile();

    audioFile.close();

    startButton->setEnabled(true);
    stopButton->setEnabled(false);

    accept(); // 关闭对话框并返回 Accepted
}

QString RecordDialog::getAudioFilePath() const
{
    return audioFilePath;
}

void RecordDialog::writeWavHeader()
{
    // WAV 文件头占位，稍后更新
    QByteArray header;
    header.resize(44);
    audioFile.write(header);
}

void RecordDialog::finalizeWavFile()
{
    qint64 fileSize = audioFile.size();
    qint32 dataSize = static_cast<qint32>(fileSize - 44);

    QDataStream out(&audioFile);
    out.setByteOrder(QDataStream::LittleEndian);

    audioFile.seek(0);
    out.writeRawData("RIFF", 4);
    out << static_cast<quint32>(fileSize - 8);
    out.writeRawData("WAVE", 4);
    out.writeRawData("fmt ", 4);
    out << static_cast<quint32>(16); // Subchunk1Size for PCM
    out << static_cast<quint16>(1);  // AudioFormat PCM
    out << static_cast<quint16>(audioFormat.channelCount());
    out << static_cast<quint32>(audioFormat.sampleRate());
    quint16 bytesPerSample = audioFormat.bytesPerSample();
    quint32 byteRate = audioFormat.sampleRate() * audioFormat.channelCount() * bytesPerSample;
    out << byteRate;
    quint16 blockAlign = audioFormat.channelCount() * bytesPerSample;
    out << blockAlign;
    out << static_cast<quint16>(bytesPerSample * 8);
    out.writeRawData("data", 4);
    out << static_cast<quint32>(dataSize);
}
