#ifndef CAMERA_H
#define CAMERA_H

#include <QDialog>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QVideoSink>
#include <QImageCapture>
#include <QVideoWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "client.h"

class Camera : public QDialog {
    Q_OBJECT

public:
    explicit Camera(Client *client, QWidget *parent=nullptr);
    ~Camera();

private slots:
    void onCaptureClicked();
    void onImageSaved(int id, const QString &fileName);
    void onFrameChanged(const QVideoFrame &frame);

private:
    QCamera *camera;
    QMediaCaptureSession session;
    QVideoSink *sink;
    QImageCapture *imageCapture;
    QVideoWidget *videoWidget;
    QPushButton *btnCapture;
    QLabel *labelStatus;
    QLabel *labelRes;
    QString imagePath;
    Client *client;

protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif
