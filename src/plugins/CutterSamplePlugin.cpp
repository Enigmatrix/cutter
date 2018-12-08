#include <QLabel>
#include <QAction>
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
    QAction *createSessionAction = new QAction(tr("Create Session"));
    QAction *joinSessionAction = new QAction(tr("Join Session"));
    QAction *endSessionAction = new QAction(tr("End Session"));

    QMenu* pluginsMenu = main->menuBar()->findChild<QMenu*>("menuPlugins");
    QMenu* collabMenu = pluginsMenu->addMenu(tr("Collab"));

    collabMenu->addAction(createSessionAction);
    collabMenu->addAction(joinSessionAction);
    collabMenu->addAction(endSessionAction);

    connect(createSessionAction, &QAction::triggered, this, &CutterSamplePluginWidget::createSession);
    connect(joinSessionAction, &QAction::triggered, this, &CutterSamplePluginWidget::joinSession);
    connect(endSessionAction, &QAction::triggered, this, &CutterSamplePluginWidget::endSession);

    connect(Core(), &CutterCore::seekChanged, this, &CutterSamplePluginWidget::seekChanged);
    connect(Core(), &CutterCore::functionRenamed, this, &CutterSamplePluginWidget::functionRenamed);
    connect(Core(), &CutterCore::commentsAdded, this, &CutterSamplePluginWidget::commentsAdded);
    connect(Core(), &CutterCore::commentsRemoved, this, &CutterSamplePluginWidget::commentsRemoved);

    this->popUp = new PopUp();
}

void CutterSamplePluginWidget::showNotificationPopup(QString message)
{
    this->popUp->setPopupText(message);
    this->popUp->show();
}

QString generateToken(){
    char str[11];
    for (int i = 0 ; i < 10; i++){
        str[i] = (random() % 26) + 'A' + (random() % 2 ? 0x20 : 0);
    }
    str[10] = 0;
    return QString::fromLocal8Bit(str);
}

void CutterSamplePluginWidget::createSession()
{
    auto token = generateToken();

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(token);
    showNotificationPopup(QString(tr("Token copied to clipboard.")));

    QString message = QString(tr("Collab session created. Send the token below to your collaborators.\n\n"
                              "%1"))
            .arg(token);
    QMessageBox::information(this, tr("Create Collab Session"), message);
    this->client = new Client(token);
}

void CutterSamplePluginWidget::joinSession()
{
    bool ok;
    QString token = QInputDialog::getText(0, tr("Join Collab Session"),
                                             tr("Session token:"), QLineEdit::Normal,
                                             "", &ok);

    if (ok && !token.isEmpty())
    {
        this->client = new Client(token);
        showNotificationPopup(QString("Joined session %1.").arg(token));
    }
}

void CutterSamplePluginWidget::endSession()
{
    if(!this->client) return;
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("End Collab Session"), tr("Are you sure you want to terminate this collab session?"),
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        delete this->client;
        this->client = nullptr;
        showNotificationPopup(QString(tr("Collab session ended.")));
    }
}

void CutterSamplePluginWidget::seekChanged(RVA addr)
{
    showNotificationPopup("Seek " + QString::number(addr));
}

void CutterSamplePluginWidget::commentsAdded(RVA addr, const QString &cmt){
    showNotificationPopup("Added "+QString::number(addr) + ": " + cmt);
    if(!this->client) return;
    this->client->commentsAdded(addr, cmt);
}

void CutterSamplePluginWidget::functionRenamed(const QString &oldName, const QString &newName){
    showNotificationPopup("Renamed fn " + oldName + " -> " + newName);
    if(!this->client) return;
    //this->client->functionRenamed(oldName, newName);
}

void CutterSamplePluginWidget::commentsRemoved(RVA addr){
    showNotificationPopup("Removed " +QString::number(addr));
    if(!this->client) return;
    this->client->commentsDeleted(addr);
}
