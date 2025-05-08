#include "feature_menu_widget.h"
#include <QPushButton>
#include <QMenu>
#include <QAction>
#include <QToolBar>

FeatureMenuWidget::FeatureMenuWidget(QWidget *parent):QWidget(parent) {
    QPushButton *menuBtn=new QPushButton("菜单");
    menuBtn->setFixedSize(60,30);

    QMenu *menu=new QMenu(menuBtn);
    QAction *wordCloudAction=menu->addAction("高频词统计");
    QAction *relationAction=menu->addAction("用户关系分析");
    menu->addAction("暂定");
    menu->addAction("暂定");

    menuBtn->setMenu(menu);

    toolBar=new QToolBar;
    toolBar->addWidget(menuBtn);
    toolBar->setMovable(false);
    toolBar->setFloatable(false);

    connect(wordCloudAction,&QAction::triggered,this,&FeatureMenuWidget::wordCloudRequested);
    connect(relationAction,&QAction::triggered,this,&FeatureMenuWidget::relationAnalysisRequested);  // 新连接
}

QToolBar* FeatureMenuWidget::getToolBar() {
    return toolBar;
}
