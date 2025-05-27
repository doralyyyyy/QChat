#ifndef EMOJI_PICKER_H
#define EMOJI_PICKER_H

#include <QWidget>
#include <QPushButton>
#include <QGridLayout>

class EmojiPicker : public QWidget {
    Q_OBJECT
public:
    explicit EmojiPicker(QWidget *parent=nullptr);
signals:
    void emojiSelected(const QString &emoji);
};

#endif // EMOJI_PICKER_H
