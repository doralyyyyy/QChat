#include "friend_list_widget.h"
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QApplication>

FriendListWidget::FriendListWidget(QWidget *parent) : QListWidget(parent) {
    setMouseTracking(true);
}

void FriendListWidget::setItemAvatar(QListWidgetItem *item, const QString &path) {
    avatarPaths[item]=path;
}

void FriendListWidget::mouseMoveEvent(QMouseEvent *e) {
    hoverItem = itemAt(e->pos());
    hoverPos = e->pos();
    viewport()->update();
    QListWidget::mouseMoveEvent(e);
}

void FriendListWidget::leaveEvent(QEvent *e) {
    hoverItem = nullptr;
    viewport()->update();
    QListWidget::leaveEvent(e);
}

void FriendListWidget::paintEvent(QPaintEvent *e) {
    QListWidget::paintEvent(e);

    if (!hoverItem) return;

    QPainter p(viewport());
    QRect rect = visualItemRect(hoverItem);
    QIcon icon = hoverItem->icon();

    if (!icon.isNull()) {
        QRect iconRect(rect.left()+10, rect.top()+10, 44, 44);
        if (iconRect.contains(hoverPos)) {
            QString path=avatarPaths.value(hoverItem);
            QPixmap fullPix(path); // 用原图

            if (!fullPix.isNull()) {
                QPixmap zoomed=fullPix.scaled(300,300,Qt::KeepAspectRatio,Qt::SmoothTransformation);
                QPoint center=hoverPos+QPoint(60,70);
                QRect target(center.x()-50,center.y()-50,100,100);
                p.drawPixmap(target, zoomed);
            }
        }
    }
}
