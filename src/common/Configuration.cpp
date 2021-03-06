#include "Configuration.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QFontDatabase>
#include <QFile>
#include <QApplication>

#include "common/ColorSchemeFileSaver.h"

const QList<CutterQtTheme> kCutterQtThemesList = {
    { "Default", LightFlag },
    { "Dark",    DarkFlag }
};

Configuration *Configuration::mPtr = nullptr;

/*!
 * \brief All asm.* options saved as settings. Values are the default values.
 */
static const QHash<QString, QVariant> asmOptions = {
    { "asm.esil",           false },
    { "asm.pseudo",         false },
    { "asm.offset",         true },
    { "asm.describe",       false },
    { "asm.stackptr",       false },
    { "asm.slow",           true },
    { "asm.lines",          true },
    { "asm.lines.fcn",      true },
    { "asm.flags.offset",   false },
    { "asm.emu",            false },
    { "asm.cmt.right",      true },
    { "asm.cmt.col",        35 },
    { "asm.var.summary",    false },
    { "asm.bytes",          false },
    { "asm.size",           false },
    { "asm.bytespace",      false },
    { "asm.lbytes",         true },
    { "asm.nbytes",         10 },
    { "asm.syntax",         "intel" },
    { "asm.ucase",          false },
    { "asm.bbline",         false },
    { "asm.capitalize",     false },
    { "asm.var.sub",        true },
    { "asm.var.subonly",    true },
    { "asm.tabs",           5 },
    { "asm.tabs.off",       5 },
    { "asm.marks",          false },
    { "esil.breakoninvalid",   true },
    { "graph.offset",       false}
};


Configuration::Configuration() : QObject()
{
    mPtr = this;
    loadInitial();
}

Configuration *Configuration::instance()
{
    if (!mPtr)
        mPtr = new Configuration();
    return mPtr;
}

void Configuration::loadInitial()
{
    setTheme(getTheme());
    setColorTheme(getColorTheme());
    applySavedAsmOptions();
}

QString Configuration::getDirProjects()
{
    auto projectsDir = s.value("dir.projects").toString();
    if (projectsDir == "") {
        projectsDir = Core()->getConfig("dir.projects");
        setDirProjects(projectsDir);
    }

    return QDir::toNativeSeparators(projectsDir);
}

void Configuration::setDirProjects(const QString &dir)
{
    s.setValue("dir.projects", QDir::toNativeSeparators(dir));
}

/**
 * @brief Configuration::setFilesTabLastClicked
 * Set the new file dialog last clicked tab
 * @param lastClicked
 */
void Configuration::setNewFileLastClicked(int lastClicked)
{
    s.setValue("newFileLastClicked", lastClicked);
}

int Configuration::getNewFileLastClicked()
{
    return s.value("newFileLastClicked").toInt();
}

void Configuration::resetAll()
{
    Core()->cmd("e-");
    Core()->setSettings();
    // Delete the file so no extra configuration is in it.
    QFile settingsFile(s.fileName());
    settingsFile.remove();
    s.clear();

    loadInitial();
    emit fontsUpdated();
}

QLocale Configuration::getCurrLocale() const
{
    return s.value("locale", QLocale().system()).toLocale();
}

void Configuration::setLocale(const QLocale &l)
{
    s.setValue("locale", l);
}

void Configuration::loadDefaultTheme()
{
    /* Load Qt Theme */
    qApp->setStyleSheet("");

    /* Images */
    logoFile = QString(":/img/cutter_plain.svg");

    /* Colors */
    // GUI
    setColor("gui.cflow",   QColor(0, 0, 0));
    setColor("gui.dataoffset", QColor(0, 0, 0));
    setColor("gui.border",  QColor(0, 0, 0));
    setColor("highlight",   QColor(210, 210, 255, 150));
    setColor("highlightWord", QColor(210, 210, 255));
    // RIP line selection in debug
    setColor("highlightPC", QColor(214, 255, 210));
    // Windows background
    setColor("gui.background", QColor(255, 255, 255));
    setColor("gui.disass_selected", QColor(255, 255, 255));
    // Disassembly nodes background
    setColor("gui.alt_background", QColor(245, 250, 255));
    // Custom
    setColor("gui.imports", QColor(50, 140, 255));
    setColor("gui.main", QColor(0, 128, 0));
    setColor("gui.navbar.err", QColor(255, 0, 0));
    setColor("gui.navbar.code", QColor(104, 229, 69));
    setColor("gui.navbar.str", QColor(69, 104, 229));
    setColor("gui.navbar.sym", QColor(229, 150, 69));
    setColor("gui.navbar.empty", QColor(100, 100, 100));
    setColor("gui.breakpoint_background", QColor(233, 143, 143));
}

void Configuration::loadBaseDark()
{
    /* Load Qt Theme */
    QFile f(":qdarkstyle/style.qss");
    if (!f.exists()) {
        qWarning() << "Can't find dark theme stylesheet.";
    } else {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        QString stylesheet = ts.readAll();
#ifdef Q_OS_MACX
        // see https://github.com/ColinDuquesnoy/QDarkStyleSheet/issues/22#issuecomment-96179529
        stylesheet += "QDockWidget::title"
                      "{"
                      "    background-color: #31363b;"
                      "    text-align: center;"
                      "    height: 12px;"
                      "}";
#endif
        qApp->setStyleSheet(stylesheet);
    }

    /* Images */
    logoFile = QString(":/img/cutter_white_plain.svg");

    /* Colors */
    // GUI
    setColor("gui.cflow",   QColor(255, 255, 255));
    setColor("gui.dataoffset", QColor(255, 255, 255));
    // Custom
    setColor("gui.imports", QColor(50, 140, 255));
    setColor("gui.main", QColor(0, 128, 0));

    // GUI: navbar
    setColor("gui.navbar.err", QColor(233, 86, 86));
    setColor("gui.navbar.code", QColor(130, 200, 111));
    setColor("angui.navbar.str", QColor(111, 134, 216));
    setColor("gui.navbar.sym", QColor(221, 163, 104));
    setColor("gui.navbar.empty", QColor(100, 100, 100));

    // RIP line selection in debug
    setColor("highlightPC", QColor(87, 26, 7));
    setColor("gui.breakpoint_background", QColor(140, 76, 76));
}

void Configuration::loadDarkTheme()
{
    loadBaseDark();
    setColor("gui.border",  QColor(100, 100, 100));
    // Windows background
    setColor("gui.background", QColor(37, 40, 43));
    // Disassembly nodes background
    setColor("gui.alt_background", QColor(28, 31, 36));
    // Disassembly nodes background when selected
    setColor("gui.disass_selected", QColor(31, 34, 40));
    // Disassembly line selected
    setColor("highlight", QColor(21, 29, 29, 150));
    setColor("highlightWord", QColor(100, 100, 100));

}

const QFont Configuration::getFont() const
{
    QFont font = s.value("font", QFont("Inconsolata", 11)).value<QFont>();
    return font;
}

void Configuration::setFont(const QFont &font)
{
    s.setValue("font", font);
    emit fontsUpdated();
}

QString Configuration::getLastThemeOf(const CutterQtTheme &currQtTheme) const
{
    return s.value("lastThemeOf." + currQtTheme.name,
                   Config()->getColorTheme()).toString();
}

void Configuration::setTheme(int theme)
{
    if (theme >= kCutterQtThemesList.size() ||
        theme < 0) {
        theme = 0;
    }
    s.setValue("ColorPalette", theme);
    QString themeName = kCutterQtThemesList[theme].name;

    if (themeName == "Default") {
        loadDefaultTheme();
    } else if (themeName == "Dark") {
        loadDarkTheme();
    } else {
        loadDefaultTheme();
    }

    emit colorsUpdated();
}


QString Configuration::getLogoFile()
{
    return logoFile;
}

/*!
 * \brief Configuration::setColor sets the local Cutter configuration color
 * \param name Color Name
 * \param color The color you want to set
 */
void Configuration::setColor(const QString &name, const QColor &color)
{
    s.setValue("colors." + name, color);
}

void Configuration::setLastThemeOf(const CutterQtTheme &currQtTheme, const QString &theme)
{
    s.setValue("lastThemeOf." + currQtTheme.name, theme);
}

const QColor Configuration::getColor(const QString &name) const
{
    if (s.contains("colors." + name)) {
        return s.value("colors." + name).value<QColor>();
    } else {
        return s.value("colors.other").value<QColor>();
    }
}

void Configuration::setColorTheme(const QString &theme)
{
    if (theme == "default") {
        Core()->cmd("ecd");
        s.setValue("theme", "default");
    } else {
        Core()->cmd(QStringLiteral("eco %1").arg(theme));
        s.setValue("theme", theme);
    }
    // Duplicate interesting colors into our Cutter Settings
    // Dirty fix for arrow colors, TODO refactor getColor, setColor, etc.
    QJsonDocument colors = Core()->cmdj("ecj");
    QJsonObject colorsObject = colors.object();

    for (auto it = colorsObject.constBegin(); it != colorsObject.constEnd(); it++) {
        QJsonArray rgb = it.value().toArray();
        if (rgb.size() != 3) {
            continue;
        }
        s.setValue("colors." + it.key(), QColor(rgb[0].toInt(), rgb[1].toInt(), rgb[2].toInt()));
    }

    QMap<QString, QColor> cutterSpecific = ColorSchemeFileWorker().getCutterSpecific();
    for (auto &it : cutterSpecific.keys())
        setColor(it, cutterSpecific[it]);

    if (!ColorSchemeFileWorker().isCustomScheme(theme)) {
        setTheme(getTheme());
    }

    emit colorsUpdated();
}

void Configuration::resetToDefaultAsmOptions()
{
    for (auto it = asmOptions.cbegin(); it != asmOptions.cend(); it++) {
        setConfig(it.key(), it.value());
    }
}

void Configuration::applySavedAsmOptions()
{
    for (auto it = asmOptions.cbegin(); it != asmOptions.cend(); it++) {
        Core()->setConfig(it.key(), s.value(it.key(), it.value()));
    }
}

QVariant Configuration::getConfigVar(const QString &key)
{
    QHash<QString, QVariant>::const_iterator it = asmOptions.find(key);
    if (it != asmOptions.end()) {
        switch (it.value().type()) {
        case QVariant::Type::Bool:
            return Core()->getConfigb(key);
        case QVariant::Type::Int:
            return Core()->getConfigi(key);
        default:
            return Core()->getConfig(key);
        }
    }
    return QVariant();
}


bool Configuration::getConfigBool(const QString &key)
{
    return getConfigVar(key).toBool();
}

int Configuration::getConfigInt(const QString &key)
{
    return getConfigVar(key).toInt();
}

QString Configuration::getConfigString(const QString &key)
{
    return getConfigVar(key).toString();
}

/**
 * @brief Configuration::setConfig
 * Set radare2 configuration value (e.g. "asm.lines")
 * @param key
 * @param value
 */
void Configuration::setConfig(const QString &key, const QVariant &value)
{
    if (asmOptions.contains(key)) {
        s.setValue(key, value);
    }

    Core()->setConfig(key, value);
}
