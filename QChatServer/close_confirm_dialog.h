#ifndef CLOSECONFIRMDIALOG_H
#define CLOSECONFIRMDIALOG_H

#include <QDialog>
#include <QCheckBox>

class CloseConfirmDialog : public QDialog {
    Q_OBJECT
public:
    enum Action { Exit, Minimize, Cancel };

    explicit CloseConfirmDialog(QWidget *parent = nullptr);
    Action getAction() const;
    bool dontAskAgain() const;

private:
    Action selectedAction=Minimize;
    QCheckBox *checkBoxDontAsk=nullptr;

private slots:
    void onExitClicked();
    void onMinimizeClicked();
};

#endif
