HEADERS        += CutterSamplePlugin.h CutterPlugin.h \
    client.pb.h \
    client.h \
    popup.h \
    client.pb.h
INCLUDEPATH    += ../ ../../build
SOURCES        += CutterSamplePlugin.cpp \
    client.pb.cc \
    client.cpp \
    popup.cpp \
    client.pb.cc

QMAKE_POST_LINK += "mkdir -p $${_PRO_FILE_PWD_}/../../build/plugins/ && cp $${_PRO_FILE_PWD_}/*.so $${_PRO_FILE_PWD_}/../../build/plugins/$$escape_expand(\n\t)"

# Needed for r_core include TODO cross platform
unix:exists(/usr/include/libr) {
    INCLUDEPATH += /usr/include/libr
}

TEMPLATE        = lib
CONFIG         += plugin
QT             += widgets network
TARGET          = PluginSample
