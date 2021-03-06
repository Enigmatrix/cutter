#ifndef CUTTERSAMPLEPLUGIN_H
#define CUTTERSAMPLEPLUGIN_H

#include <QObject>
#include <QMenu>
#include <QAction>
#include <QtPlugin>
#include "popup.h"
#include "CutterPlugin.h"
#include "client.h"

class CutterSamplePlugin : public QObject, CutterPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.radare.cutter.plugins.CutterPlugin")
    Q_INTERFACES(CutterPlugin)

public:
    void setupPlugin(CutterCore *core) override;
    CutterDockWidget* setupInterface(MainWindow *main, QAction *action = nullptr) override;
};

class CutterSamplePluginWidget : public CutterDockWidget
{
    Q_OBJECT

public:
    explicit CutterSamplePluginWidget(MainWindow *main, QAction *action);

private:
    PopUp* popUp;
    Client* client;
    MainWindow* main;
    QMenu* collabMenu;
    QAction* createSessionAction;
    QAction* joinSessionAction;
    QAction* endSessionAction;
    QLabel* informationText;
    void setupClient(QString token, QString nick);
    void showNotificationPopup(QString);
    void showUserNotificationPopup(QString, QString);
    void addEndSessionMenuAction();
    void removeEndSessionMenuAction();
    void updateSessionInformation(QString, QList<QString>);
    void clearSessionInformation();

private slots:
    void createSession();
    void joinSession();
    void endSession();

    void seekChanged(RVA addr);
    void functionRenamed(const QString& oldName, const QString& newName);
    void commentsAdded(RVA addr, const QString& cmt);
    void commentsRemoved(RVA addr);
    void createSessionClicked();
    void joinSessionClicked();
    void endSessionClicked();
    void onCommentsAdded(RVA, QString);
    void onFunctionRenamed(QString, QString);
    void onCommentsRemoved(RVA);
};


#endif // CUTTERSAMPLEPLUGIN_H
