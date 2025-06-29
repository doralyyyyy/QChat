#ifndef FILECONFIRMDIALOG_H
#define FILECONFIRMDIALOG_H

#include <QDialog>

class QLabel;
class QDialogButtonBox;

class FileConfirmDialog : public QDialog {
    Q_OBJECT
public:
    explicit FileConfirmDialog(const QString &filePath, QWidget *parent=nullptr);
    bool isAccepted() const;
    QString getFilePath() const;

private:
    QString path;
    bool accepted=false;
};

#endif
