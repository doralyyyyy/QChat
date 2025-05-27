#ifndef FEEDBACK_DIALOG_H
#define FEEDBACK_DIALOG_H

#include <QDialog>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

class FeedbackDialog : public QDialog {
    Q_OBJECT
public:
    FeedbackDialog(QWidget *parent = nullptr);

signals:
    void feedbackSubmitted(const QString &text);

private:
    QTextEdit *edit;
    QPushButton *sendButton;
};

#endif // FEEDBACK_DIALOG_H
