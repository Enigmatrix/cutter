HEADERS        += CutterPlugin.h \
    model_generated.h \
    client.h \
    popup.h \
    CutterSamplePlugin.h
INCLUDEPATH    += ../ ../../build
SOURCES        += client.cpp \
    popup.cpp \
    CutterSamplePlugin.cpp \

QMAKE_POST_LINK += "mkdir -p $${_PRO_FILE_PWD_}/../../build/plugins/ && cp $${_PRO_FILE_PWD_}/*.so $${_PRO_FILE_PWD_}/../../build/plugins/$$escape_expand(\n\t)"

# Needed for r_core include TODO cross platform
unix:exists(/usr/include/libr) {
    INCLUDEPATH += /usr/include/libr
}

LIBS 		+= -lflatbuffers
TEMPLATE        = lib
CONFIG         += plugin
QT             += widgets network
TARGET          = PluginSample

DISTFILES += model.fbs
