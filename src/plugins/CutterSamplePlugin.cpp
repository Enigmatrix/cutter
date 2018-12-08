#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>

#include "CutterSamplePlugin.h"
#include "common/TempConfig.h"
#include "common/Configuration.h"

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
QPushButton * button;
CutterSamplePluginWidget::CutterSamplePluginWidget(MainWindow *main, QAction *action) :
    CutterDockWidget(main, action)
{
    this->setObjectName("CutterSamplePluginWidget");
    this->setWindowTitle("Sample Plugin");
    QWidget *content = new QWidget();
    this->setWidget(content);

    QVBoxLayout *layout = new QVBoxLayout(this);
    content->setLayout(layout);
    text = new QLabel(content);
    text->setFont(Config()->getFont());
    text->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    layout->addWidget(text);

    button = new QPushButton(content);
    button->setText("Want a fortune?");
    button->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    button->setMaximumHeight(50);
    button->setMaximumWidth(200);
    layout->addWidget(button);
    layout->setAlignment(button, Qt::AlignHCenter);

    connect(Core(), &CutterCore::seekChanged, this, &CutterSamplePluginWidget::on_seekChanged);
    connect(Core(), &CutterCore::commentsAdded, this, &CutterSamplePluginWidget::commentsAdded);
    connect(Core(), &CutterCore::commentsRemoved, this, &CutterSamplePluginWidget::commentsRemoved);
    connect(button, &QPushButton::clicked, this, &CutterSamplePluginWidget::on_buttonClicked);
}

void CutterSamplePluginWidget::on_seekChanged(RVA addr)
{
    Q_UNUSED(addr);
    QString res;
    {
        TempConfig tempConfig;
        tempConfig.set("scr.color", 0);
        res = Core()->cmd("?E `pi 1`");
    }
    text->setText(res);
}

void CutterSamplePluginWidget::on_buttonClicked()
{
    QString res = Core()->cmd("?E `fo`");
    text->setText(res);
}

void CutterSamplePluginWidget::commentsAdded(RVA addr, const QString &cmt){
    button->setText("Added "+QString::number(addr) + ": " + cmt);
}


void CutterSamplePluginWidget::commentsRemoved(RVA addr){
    button->setText("Removed " +QString::number(addr));
}
