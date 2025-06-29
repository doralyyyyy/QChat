#include <QDialog>
#include <QPushButton>
#include <QMediaRecorder>
#include <QMediaCaptureSession>
#include <QAudioInput>
#include <QTimer>
#include <QLabel>

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
    void timePass();

private:
    QPushButton *startButton;
    QPushButton *stopButton;
    QTimer *timer;
    QLabel *timeLabel;
    int elapsedSeconds;
    QMediaRecorder *mediaRecorder;
    QMediaCaptureSession *mediaCaptureSession;
    QAudioInput *audioInput;
    QString audioFilePath;
};
