#include <QLabel>
#include<QAction>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMenuBar>
#include <QMainWindow>

#include "CutterSamplePlugin.h"
#include "common/TempConfig.h"
#include "common/Configuration.h"
#include "MainWindow.h"

void CutterSamplePlugin::setupPlugin(CutterCore *core)
{
    this->core = core;
    this->name = "CollabCutter";
    this->description = "Collaborative Reversing in Cutter/Radare2";
    this->version = "0.01-ALPHA1";
    this->author = "OSI Layer 8";
}

CutterDockWidget* CutterSamplePlugin::setupInterface(MainWindow *main, QAction* actions)
{
    // Instantiate dock widget
    dockable = new CutterSamplePluginWidget(main, actions);
    return dockable;
}

CutterSamplePluginWidget::CutterSamplePluginWidget(MainWindow *main, QAction *action) :
    CutterDockWidget(main, action)
{
    auto createSessionAction = new QAction("Create Session");
    auto joinSessionAction = new QAction("Join Session");
    auto endSessionAction = new QAction("End Session");

    QWidget *content = new QWidget();
    this->setWidget(content);

    QVBoxLayout *layout = new QVBoxLayout(this);
    content->setLayout(layout);
    text = new QLabel(content);

    main->addMenuFileAction(createSessionAction);
    main->addMenuFileAction(joinSessionAction);
    main->addMenuFileAction(endSessionAction);

    connect(Core(), &CutterCore::seekChanged, this, &CutterSamplePluginWidget::seekChanged);
    connect(Core(), &CutterCore::functionRenamed, this, &CutterSamplePluginWidget::functionRenamed);
    connect(Core(), &CutterCore::commentsAdded, this, &CutterSamplePluginWidget::commentsAdded);
    connect(Core(), &CutterCore::commentsRemoved, this, &CutterSamplePluginWidget::commentsRemoved);
}

void CutterSamplePluginWidget::seekChanged(RVA addr)
{
    text->setText("Seek " + QString::number(addr));
}

void CutterSamplePluginWidget::commentsAdded(RVA addr, const QString &cmt){
    text->setText("Added "+QString::number(addr) + ": " + cmt);
}

void CutterSamplePluginWidget::functionRenamed(const QString &oldName, const QString &newName){
    text->setText("Renamed fn " + oldName + " -> " + newName);
}

void CutterSamplePluginWidget::commentsRemoved(RVA addr){
    text->setText("Removed " +QString::number(addr));
}
