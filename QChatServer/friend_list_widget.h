#include <QListWidget>
#include <QMouseEvent>
#include <QTimer>
#include <QMap>

class FriendListWidget : public QListWidget {
    Q_OBJECT
public:
    FriendListWidget(QWidget *parent=nullptr);
    void setItemAvatar(QListWidgetItem *item, const QString &path); // 添加这个接口

protected:
    void mouseMoveEvent(QMouseEvent *e) override;
    void leaveEvent(QEvent *e) override;
    void paintEvent(QPaintEvent *e) override;

private:
    QListWidgetItem *hoverItem=nullptr;
    QPoint hoverPos;
    QMap<QListWidgetItem*, QString> avatarPaths; // 存头像路径
};
