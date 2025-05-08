#ifndef RECORDDIALOG_H
#define RECORDDIALOG_H

#include <QDialog>
#include <QAudioSource>
#include <QFile>
#include <QPushButton>
#include <QVBoxLayout>

class RecordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RecordDialog(QWidget *parent = nullptr);
    ~RecordDialog();

    QString getAudioFilePath() const;

private slots:
    void startRecording();
    void stopRecording();

private:
    QAudioSource *audioSource;
    QFile audioFile;
    QString audioFilePath;
    QAudioFormat audioFormat;
    QPushButton *startButton;
    QPushButton *stopButton;

    void writeWavHeader();
    void finalizeWavFile();
};

#endif // RECORDDIALOG_H
