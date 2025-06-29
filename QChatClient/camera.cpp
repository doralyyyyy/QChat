#include "camera.h"
#include <QMediaDevices>
#include <QDir>
#include <QCoreApplication>
#include <QCloseEvent>

Camera::Camera(Client *client, QWidget *parent) : QDialog(parent), client(client) {
    setWindowTitle("拍照");
    this->resize(980, 600);

    videoWidget=new QVideoWidget(this);
    videoWidget->setFixedSize(960,540);

    btnCapture=new QPushButton("拍照并发送",this);
    labelStatus=new QLabel("",this);
    labelRes=new QLabel("分辨率",this);

    QHBoxLayout *bottomLayout=new QHBoxLayout;
    bottomLayout->addWidget(btnCapture);
    bottomLayout->addStretch();
    bottomLayout->addWidget(labelStatus);
    bottomLayout->addSpacing(10);
    bottomLayout->addWidget(labelRes);

    QVBoxLayout *mainLayout=new QVBoxLayout(this);
    mainLayout->addWidget(videoWidget,0,Qt::AlignHCenter);
    mainLayout->addLayout(bottomLayout);
    setLayout(mainLayout);

    camera=new QCamera(QMediaDevices::defaultVideoInput(),this);
    imageCapture=new QImageCapture(this);

    session.setCamera(camera);
    session.setVideoOutput(videoWidget);
    session.setImageCapture(imageCapture);

    sink=session.videoSink();
    connect(sink,&QVideoSink::videoFrameChanged,this,&Camera::onFrameChanged);
    connect(btnCapture,&QPushButton::clicked,this,&Camera::onCaptureClicked);
    connect(imageCapture,&QImageCapture::imageSaved,this,&Camera::onImageSaved);

    camera->start();
}

Camera::~Camera() {
}

void Camera::closeEvent(QCloseEvent *event){
    if(camera) camera->stop();
    QDialog::closeEvent(event);
}

void Camera::onCaptureClicked() {
    imagePath=QCoreApplication::applicationDirPath()+'/'+QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")+"_image.jpg";
    imageCapture->captureToFile(imagePath);
}

void Camera::onImageSaved(int, const QString &) {
    labelStatus->setText("已尝试发送并保存于: "+imagePath);
    client->sendFile(imagePath);
}

void Camera::onFrameChanged(const QVideoFrame &frame) {
    labelRes->setText(QString("%1 x %2").arg(frame.width()).arg(frame.height()));
}
