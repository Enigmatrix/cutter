#ifndef CUTTERSAMPLEPLUGIN_H
#define CUTTERSAMPLEPLUGIN_H

#include <QObject>
#include <QtPlugin>
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
    QLabel* text;
    Client* client;

private slots:
    void seekChanged(RVA addr);
    void functionRenamed(const QString& oldName, const QString& newName);
    void commentsAdded(RVA addr, const QString& cmt);
    void commentsRemoved(RVA addr);
    void createSessionClicked();
    void joinSessionClicked();
    void endSessionClicked();
};


#endif // CUTTERSAMPLEPLUGIN_H
