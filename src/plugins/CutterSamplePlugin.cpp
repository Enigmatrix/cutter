#include <QLabel>
#include <QAction>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMenuBar>
#include <QMainWindow>
#include <QInputDialog>
#include <QMessageBox>
#include <QClipboard>

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
    QAction *createSessionAction = new QAction(tr("Create Session"));
    QAction *joinSessionAction = new QAction(tr("Join Session"));
    QAction *endSessionAction = new QAction(tr("End Session"));

    QMenu* pluginsMenu = main->menuBar()->findChild<QMenu *>("menuPlugins");

    pluginsMenu->addAction(createSessionAction);
    pluginsMenu->addAction(joinSessionAction);
    pluginsMenu->addAction(endSessionAction);

    connect(createSessionAction, &QAction::triggered, this, &CutterSamplePluginWidget::createSession);
    connect(joinSessionAction, &QAction::triggered, this, &CutterSamplePluginWidget::joinSession);
    connect(endSessionAction, &QAction::triggered, this, &CutterSamplePluginWidget::endSession);

    QWidget *content = new QWidget();
    this->setWidget(content);

    QVBoxLayout *layout = new QVBoxLayout(this);
    content->setLayout(layout);
    text = new QLabel(content);

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

void CutterSamplePluginWidget::createSession()
{
    // TODO: generate token
    char* token = "pls_replace_this";

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(token);
    showNotificationPopup(QString(tr("Token copied to clipboard.")));

    QString message = QString(tr("Collab session created. Send the token below to your collaborators.\n\n"
                              "%1"))
            .arg(token);
    QMessageBox::information(this, tr("Create Collab Session"), message);
}

void CutterSamplePluginWidget::joinSession()
{
    bool ok;
    QString token = QInputDialog::getText(0, tr("Join Collab Session"),
                                             tr("Session token:"), QLineEdit::Normal,
                                             "", &ok);

    if (ok && !token.isEmpty())
    {
        // TODO: connect to server
        showNotificationPopup(QString("Joined session %1.").arg(token));
    }
}

void CutterSamplePluginWidget::endSession()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("End Collab Session"), tr("Are you sure you want to terminate this collab session?"),
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        // TODO: handle terminate
        showNotificationPopup(QString(tr("Collab session ended.")));
    }
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
