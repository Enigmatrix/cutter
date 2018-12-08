HEADERS        += CutterSamplePlugin.h CutterPlugin.h \
    client.h \
    types.h
INCLUDEPATH    += ../
SOURCES        += CutterSamplePlugin.cpp \
    client.cpp

QMAKE_POST_LINK += "mkdir -p $${_PRO_FILE_PWD_}/../../build/plugins/ && cp $${_PRO_FILE_PWD_}/*.so $${_PRO_FILE_PWD_}/../../build/plugins/$$escape_expand(\n\t)"

# Needed for r_core include TODO cross platform
unix:exists(/usr/include/libr) {
    INCLUDEPATH += /usr/include/libr
}

# macOS
unix:exists(/usr/local/include/libr) {
    INCLUDEPATH += /usr/local/include/libr
}

TEMPLATE        = lib
CONFIG         += plugin
QT             += widgets network
TARGET          = PluginSample
