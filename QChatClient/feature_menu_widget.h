#ifndef FEATURE_MENU_WIDGET_H
#define FEATURE_MENU_WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMenu>
#include <QAction>
#include <QToolBar>

class FeatureMenuWidget : public QWidget {
    Q_OBJECT
public:
    explicit FeatureMenuWidget(QWidget *parent=nullptr);
    QToolBar *getToolBar();

    QPushButton *menuBtn;

signals:
    void wordCloudRequested();
    void relationAnalysisRequested();
    void exportChatToPdfRequested();
    void generateTimelineRequested();
    void feedbackRequested();

private:
    QToolBar *toolBar;
};

#endif
