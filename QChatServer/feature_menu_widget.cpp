#include "feature_menu_widget.h"
#include <QPushButton>
#include <QMenu>
#include <QAction>
#include <QToolBar>

FeatureMenuWidget::FeatureMenuWidget(QWidget *parent) : QWidget(parent) {
    QPushButton *menuBtn = new QPushButton("菜单", this);
    menuBtn->setStyleSheet(R"(
        QPushButton {
            font-size: 14px;
            text-align: center;
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #ff9a9e, stop:1 #fad0c4);
            border: none;
            border-radius: 20px;
            color: white;
            min-width: 100px;
            min-height: 50px;
        }

        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #fbc2eb, stop:1 #a6c1ee);
        }
    )");

    menuBtn->setFixedSize(70, 50);

    QMenu *menu = new QMenu(menuBtn);

    QAction *wordCloudAction = menu->addAction("高频词统计");
    QAction *relationAction = menu->addAction("用户关系分析");
    QAction *exportPdfAction = menu->addAction("导出聊天记录为 PDF");
    QAction *timelineAction=menu->addAction("聊天记录时间轴");

    menu->setStyleSheet(R"(
        QMenu {
            font-size: 11px;
            background-color: #ffffff;
            border-radius: 10px;
            padding: 5px;
        }

        QMenu::item {
            padding: 5px;
            min-height: 30px;
            font-size: 11px;
            border-radius: 5px;
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

    toolBar = new QToolBar(this);
    toolBar->addWidget(menuBtn);
    toolBar->setMovable(false);
    toolBar->setFloatable(false);

    connect(wordCloudAction, &QAction::triggered, this, &FeatureMenuWidget::wordCloudRequested);
    connect(relationAction, &QAction::triggered, this, &FeatureMenuWidget::relationAnalysisRequested);
    connect(exportPdfAction, &QAction::triggered, this, &FeatureMenuWidget::exportChatToPdfRequested);
    connect(timelineAction,&QAction::triggered,this,&FeatureMenuWidget::generateTimelineRequested);
}

QToolBar* FeatureMenuWidget::getToolBar() {
    return toolBar;
}
