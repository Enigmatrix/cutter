#include <QLabel>
#include<QAction>
#include <QHBoxLayout>
#include <QApplication>
#include <QPushButton>
#include <QMenuBar>
#include <QMainWindow>
#include <QMessageBox>
#include <QUuid>
#include <QClipboard>
#include <QInputDialog>

#include "CutterSamplePlugin.h"
#include "common/TempConfig.h"
#include "common/Configuration.h"
#include "MainWindow.h"
#include "client.h"

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

    connect(createSessionAction, &QAction::triggered, this, &CutterSamplePluginWidget::createSessionClicked);
    connect(joinSessionAction, &QAction::triggered, this, &CutterSamplePluginWidget::joinSessionClicked);
    connect(endSessionAction, &QAction::triggered, this, &CutterSamplePluginWidget::endSessionClicked);

    connect(Core(), &CutterCore::seekChanged, this, &CutterSamplePluginWidget::seekChanged);
    connect(Core(), &CutterCore::functionRenamed, this, &CutterSamplePluginWidget::functionRenamed);
    connect(Core(), &CutterCore::commentsAdded, this, &CutterSamplePluginWidget::commentsAdded);
    connect(Core(), &CutterCore::commentsRemoved, this, &CutterSamplePluginWidget::commentsRemoved);
}

QString generateToken(){
    char str[11];
    for (int i = 0 ; i < 10; i++){
        str[i] = (random() % 26) + 'A' + (random() % 2 ? 0x20 : 0);
    }
    str[10] = 0;
    return QString::fromLocal8Bit(str);
}

void CutterSamplePluginWidget::createSessionClicked(){
    auto uuid = generateToken();

    QMessageBox msg;
    msg.setText("Session created. Send this token to your teammates: "+uuid);
    auto copyBtn = msg.addButton(tr("Copy"), QMessageBox::NoRole);
    connect(copyBtn, &QPushButton::clicked, this, [&uuid](){
        auto clipboard = QApplication::clipboard();
        clipboard->setText(uuid);
    });
    msg.exec();
    this->client = new Client(uuid);
}

void CutterSamplePluginWidget::joinSessionClicked(){
    bool ok;
    QString token = QInputDialog::getText(nullptr, "Join Session",
                                             "Session Token: ", QLineEdit::Normal,
                                             "", &ok);
    if(ok && !token.isEmpty()){
        // CLIENT connect with this token
    }
    this->client = new Client(token);
}

void CutterSamplePluginWidget::endSessionClicked(){
    delete this->client;
    this->client = nullptr;

    QMessageBox msg;
    msg.setText("Session ended");
    msg.exec();
}

void CutterSamplePluginWidget::seekChanged(RVA addr)
{
    text->setText("Seek " + QString::number(addr));
}

void CutterSamplePluginWidget::commentsAdded(RVA addr, const QString &cmt){
    text->setText("Added "+QString::number(addr) + ": " + cmt);
    if(!this->client) return;
    this->client->commentsAdded(addr, cmt);
}

void CutterSamplePluginWidget::functionRenamed(const QString &oldName, const QString &newName){
    text->setText("Renamed fn " + oldName + " -> " + newName);
    if(!this->client) return;
    //this->client->functionRenamed(oldName, newName);
}

void CutterSamplePluginWidget::commentsRemoved(RVA addr){
    text->setText("Removed " +QString::number(addr));
    if(!this->client) return;
    this->client->commentsDeleted(addr);
}
