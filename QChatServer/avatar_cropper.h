#ifndef AVATAR_CROPPER_H
#define AVATAR_CROPPER_H

#include <QDialog>
#include <QPixmap>
#include <QRect>

class AvatarCropper : public QDialog {
    Q_OBJECT
public:
    AvatarCropper(const QPixmap& src,QWidget* parent=nullptr);
    QPixmap getCropped() const;

protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;

private:
    QPixmap original;
    QRect selection;
    bool dragging=false;
    QPoint startPos;
};

#endif // AVATAR_CROPPER_H
