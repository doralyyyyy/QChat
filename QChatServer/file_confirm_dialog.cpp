#include "file_confirm_dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileInfo>
#include <QFileIconProvider>
#include <QDialogButtonBox>

FileConfirmDialog::FileConfirmDialog(const QString &filePath, QWidget *parent)
    : QDialog(parent), path(filePath) {
    setWindowTitle("发送文件");
    setModal(true);

    QVBoxLayout *v=new QVBoxLayout(this);
    QHBoxLayout *h=new QHBoxLayout();

    QFileInfo fi(filePath);
    QString name=fi.fileName();
    QFileIconProvider provider;
    QIcon icon=provider.icon(fi);

    QLabel *iconLabel=new QLabel();
    iconLabel->setPixmap(icon.pixmap(48,48));
    QLabel *nameLabel=new QLabel(name);
    nameLabel->setWordWrap(true);

    h->addWidget(iconLabel);
    h->addWidget(nameLabel);
    v->addLayout(h);

    QDialogButtonBox *btnBox=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    v->addWidget(btnBox);

    connect(btnBox,&QDialogButtonBox::accepted,this,[=](){
        accepted=true;
        accept();
    });
    connect(btnBox,&QDialogButtonBox::rejected,this,&QDialog::reject);
}

bool FileConfirmDialog::isAccepted() const {
    return accepted;
}

QString FileConfirmDialog::getFilePath() const {
    return path;
}
