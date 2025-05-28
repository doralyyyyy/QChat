#ifndef BACKGROUNDSETTINGDIALOG_H
#define BACKGROUNDSETTINGDIALOG_H

#include "client.h"
#include <QDialog>
#include <QPixmap>
#include <QColor>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QFileDialog>
#include <QColorDialog>

class ChatPage; // 前向声明

class BackgroundSettingDialog : public QDialog {
    Q_OBJECT

public:
    explicit BackgroundSettingDialog(Client *client,QWidget *parent=nullptr);

private slots:
    void chooseColor();
    void chooseImage();
    void applyBackground();

private:
    Client *client;
    QPixmap selectedPixmap;
    QLabel *preview;

    QPushButton *colorBtn;
    QPushButton *imageBtn;
    QPushButton *applyBtn;
    QPushButton *cancelBtn;
};

#endif // BACKGROUNDSETTINGDIALOG_H
