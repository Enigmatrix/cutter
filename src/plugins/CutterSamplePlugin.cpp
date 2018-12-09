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
#include <functional>

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
    this->main = main;

    this->setObjectName("CutterSamplePluginWidget");
    this->setWindowTitle("Collab");
    QWidget *content = new QWidget();
    this->setWidget(content);

    QVBoxLayout *layout = new QVBoxLayout(this);
    content->setLayout(layout);

    this->informationText = new QLabel(content);
    this->informationText->setFont(Config()->getFont());
    this->informationText->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    this->informationText->setAlignment(Qt::AlignTop|Qt::AlignLeft);
    this->informationText->setWordWrap(true);
    layout->addWidget(this->informationText);

    this->clearSessionInformation();

    this->client = nullptr;
    this->createSessionAction = new QAction(tr("Create Session"));
    this->joinSessionAction = new QAction(tr("Join Session"));
    this->endSessionAction = new QAction(tr("End Session"));

    QMenu* pluginsMenu = main->menuBar()->findChild<QMenu*>("menuPlugins");
    this->collabMenu = pluginsMenu->addMenu(tr("Collab"));

    this->collabMenu->addAction(this->createSessionAction);
    this->collabMenu->addAction(this->joinSessionAction);

    connect(this->createSessionAction, &QAction::triggered, this, &CutterSamplePluginWidget::createSession);
    connect(this->joinSessionAction, &QAction::triggered, this, &CutterSamplePluginWidget::joinSession);
    connect(this->endSessionAction, &QAction::triggered, this, &CutterSamplePluginWidget::endSession);

    connect(Core(), &CutterCore::seekChanged, this, &CutterSamplePluginWidget::seekChanged);
    connect(Core(), &CutterCore::functionRenamed, this, &CutterSamplePluginWidget::functionRenamed);
    connect(Core(), &CutterCore::commentsAdded, this, &CutterSamplePluginWidget::commentsAdded);
    connect(Core(), &CutterCore::commentsRemoved, this, &CutterSamplePluginWidget::commentsRemoved);

    this->popUp = new PopUp();
}

void CutterSamplePluginWidget::updateSessionInformation(QString token, QList<QString> names)
{
    QString info = QString("Connected to session with token %1. \n\n"
                           "Users online:\n").arg(token);
    foreach (QString name, names)
    {
        info.append(QString("%1\n").arg(name));
    }

    this->informationText->setText(info);
}

void CutterSamplePluginWidget::clearSessionInformation()
{
    this->informationText->setText("Not connected to any sessions. \n\n"
                                   "Go to Plugins->Collab->Create Session to create a new session, "
                                   "or Plugins->Collab->Join Session to join an existing one.");
}

void CutterSamplePluginWidget::showNotificationPopup(QString message)
{
    this->popUp->setPopupText(message);
    this->popUp->show();
}

void CutterSamplePluginWidget::showUserNotificationPopup(QString message, QString user)
{
    this->popUp->setPopupText(message, user);
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

QString getNickNamePopup(){

    bool ok;
    QString token = QInputDialog::getText(0, "Enter Nick Name",
                                             "Nick Name : ", QLineEdit::Normal,
                                             "", &ok);

    if (ok && !token.isEmpty())
        return token;
    return nullptr;
}

void CutterSamplePluginWidget::addEndSessionMenuAction()
{
    this->collabMenu->removeAction(this->createSessionAction);
    this->collabMenu->removeAction(this->joinSessionAction);
    this->collabMenu->addAction(this->endSessionAction);
}

void CutterSamplePluginWidget::removeEndSessionMenuAction()
{
    this->collabMenu->removeAction(this->endSessionAction);
    this->collabMenu->addAction(this->createSessionAction);
    this->collabMenu->addAction(this->joinSessionAction);
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
    QString nick = getNickNamePopup();
    qDebug() << "new session: ";
    if(nick == nullptr) return;
    qDebug() << nick;

    setupClient(token, nick);
    this->addEndSessionMenuAction();

    QList<QString> names({nick, "daniel", "ambrose", "akash"});
    this->updateSessionInformation(token, names);
}

void CutterSamplePluginWidget::joinSession()
{
    bool ok;
    QString token = QInputDialog::getText(0, tr("Join Collab Session"),
                                             tr("Session token:"), QLineEdit::Normal,
                                             "", &ok);
    if (ok && !token.isEmpty())
    {
        auto nick = getNickNamePopup();
        if(nick == nullptr) return;
        setupClient(token, nick);
        showNotificationPopup(QString("Joined session %1.").arg(token));
        this->addEndSessionMenuAction();

        QList<QString> names({nick, "daniel, ambrose, akash"});
        this->updateSessionInformation(token, names);
    }
}

void CutterSamplePluginWidget::endSession()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("End Collab Session"), tr("Are you sure you want to terminate this collab session?"),
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        delete this->client;
        this->client = nullptr;
        showNotificationPopup(QString(tr("Collab session ended.")));
        this->removeEndSessionMenuAction();
        this->clearSessionInformation();
    }
}

void CutterSamplePluginWidget::seekChanged(RVA addr)
{
    showNotificationPopup("U Seek " + QString::number(addr));
}

void CutterSamplePluginWidget::commentsAdded(RVA addr, const QString &cmt){
    showUserNotificationPopup("0x"+QString::number(addr, 16)+": \""+cmt+"\"", "Akash");
    if(this->client == nullptr) return;
    this->client->commentsAdded(addr, cmt);
}

void CutterSamplePluginWidget::functionRenamed(const QString &oldName, const QString &newName){
    showNotificationPopup("U Renamed fn " + oldName + " -> " + newName);
    if(this->client == nullptr) return;
    //this->client->functionRenamed(oldName, newName);
}

void CutterSamplePluginWidget::commentsRemoved(RVA addr){
    showNotificationPopup("U Removed " +QString::number(addr));
    if(this->client == nullptr) return;
    this->client->commentsDeleted(addr);
}

void CutterSamplePluginWidget::setupClient(QString token, QString nick){
    this->client = new Client(token, nick);
    this->client->onCommentsDeleted = std::bind(&CutterSamplePluginWidget::onCommentsRemoved, this, std::placeholders::_1, std::placeholders::_2);
    this->client->onCommentsAdded = std::bind(&CutterSamplePluginWidget::onCommentsAdded, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    //this->client->onFunctionRenamed = std::bind(&CutterSamplePluginWidget::onFunctionRenamed, this, std::placeholders::_1, std::placeholders::_2);
}


void CutterSamplePluginWidget::onCommentsRemoved(QString nick, RVA addr){
    showUserNotificationPopup("0x"+QString::number(addr, 16)+": <Cmt Rmved>", nick);
    Core()->delCommentWithoutSignal(addr);
}

void CutterSamplePluginWidget::onCommentsAdded(QString nick, RVA addr, QString cmt){
    showUserNotificationPopup("0x"+QString::number(addr, 16)+": \""+cmt+"\"", nick);
    Core()->setCommentWithoutSignal(addr, cmt);
}

void CutterSamplePluginWidget::onFunctionRenamed(QString nick, QString oldName, QString newName){
    showUserNotificationPopup("Renamed "+oldName+" to "+newName, nick);
    Core()->renameFunctionWithoutSignal(oldName, newName);
}
