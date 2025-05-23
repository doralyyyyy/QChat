#include "avatar_cropper.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

AvatarCropper::AvatarCropper(const QPixmap& src,QWidget* parent):QDialog(parent),original(src) {
    setWindowTitle("裁剪头像");
    setFixedSize(original.size());
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QPushButton* ok=new QPushButton("确定");
    QPushButton* cancel=new QPushButton("取消");
    connect(ok,&QPushButton::clicked,this,&QDialog::accept);
    connect(cancel,&QPushButton::clicked,this,&QDialog::reject);

    QHBoxLayout* btnLayout=new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(ok);
    btnLayout->addWidget(cancel);

    QVBoxLayout* layout=new QVBoxLayout(this);
    layout->addStretch();
    layout->addLayout(btnLayout);
    layout->setContentsMargins(0,0,0,0);
}

void AvatarCropper::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.drawPixmap(0,0,original);
    if(!selection.isNull()) {
        p.setPen(QPen(Qt::red,2));
        p.drawRect(selection.normalized());
    }
}

void AvatarCropper::mousePressEvent(QMouseEvent* e) {
    dragging=true;
    startPos=e->pos();
    selection=QRect(startPos,startPos);
    update();
}

void AvatarCropper::mouseMoveEvent(QMouseEvent* e) {
    if(dragging) {
        QPoint end=e->pos();
        int w=end.x()-startPos.x();
        int h=end.y()-startPos.y();
        int side=qMin(abs(w),abs(h));
        if(w<0) side=-side;
        QRect r(startPos,QSize(side,side));
        selection=r.normalized();
        update();
    }
}

void AvatarCropper::mouseReleaseEvent(QMouseEvent*) {
    dragging=false;
}

QPixmap AvatarCropper::getCropped() const {
    if(selection.isNull()) return original;
    return original.copy(selection.normalized());
}
