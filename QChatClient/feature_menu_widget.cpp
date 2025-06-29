#include "feature_menu_widget.h"
#include <QToolButton>
#include <QMenu>
#include <QAction>
#include <QToolBar>

FeatureMenuWidget::FeatureMenuWidget(QWidget *parent) : QWidget(parent) {
    QToolButton *menuBtn=new QToolButton(this);
    menuBtn->setText("菜单");
    menuBtn->setPopupMode(QToolButton::InstantPopup); // 点击即弹出
    menuBtn->setStyleSheet(R"(
        QToolButton {
            padding: 10px;
            border: none;
            border-radius: 12px;
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #ff9a9e, stop:1 #fad0c4);
            color: white;
            font-size: 18px;
            font-weight: bold;
        }

        QToolButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #fbc2eb, stop:1 #a6c1ee);
        }
    )");

    QMenu *menu=new QMenu(menuBtn);

    QPixmap wordPixmap(":/icons/wordcloud.png");
    QPixmap relationPixmap(":/icons/relation.png");
    QPixmap pdfPixmap(":/icons/pdf.png");
    QPixmap timelinePixmap(":/icons/timeline.png");
    QPixmap feedbackPicmap(":/icons/feedback.png");

    QAction *wordCloudAction = menu->addAction(QIcon(wordPixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation)), "高频词统计");
    QAction *relationAction = menu->addAction(QIcon(relationPixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation)), "用户关系分析");
    QAction *exportPdfAction = menu->addAction(QIcon(pdfPixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation)), "导出为 PDF");
    QAction *timelineAction = menu->addAction(QIcon(timelinePixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation)), "聊天时间轴");
    QAction *feedbackAction = menu->addAction(QIcon(feedbackPicmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation)), "反馈与建议");


    menu->setStyleSheet(R"(
        QMenu {
            font-size: 14px;
            background-color: #ffffff;
            border-radius: 10px;
            padding: 2px;
        }

        QMenu::icon {
            width: 60px;
            height: 60px;
        }

        QMenu::item {
            padding: 5px 7px;
            min-height: 20px;
            min-width: 90px;
            font-size: 14px;
            border-radius: 10px;
            color: #333333;
        }

        QMenu::item:selected {
            background-color: #ff9a9e;
            color: white;
        }

        QMenu::item:hover {
            background-color: #fbc2eb;
        }
    )");

    menuBtn->setMenu(menu);

    toolBar=new QToolBar(this);
    toolBar->addWidget(menuBtn);
    toolBar->setMovable(false);
    toolBar->setFloatable(false);

    connect(wordCloudAction,&QAction::triggered,this,&FeatureMenuWidget::wordCloudRequested);
    connect(relationAction,&QAction::triggered,this,&FeatureMenuWidget::relationAnalysisRequested);
    connect(exportPdfAction,&QAction::triggered,this,&FeatureMenuWidget::exportChatToPdfRequested);
    connect(timelineAction,&QAction::triggered,this,&FeatureMenuWidget::generateTimelineRequested);
    connect(feedbackAction,&QAction::triggered,this,&FeatureMenuWidget::feedbackRequested);
}

QToolBar* FeatureMenuWidget::getToolBar() {
    return toolBar;
}
