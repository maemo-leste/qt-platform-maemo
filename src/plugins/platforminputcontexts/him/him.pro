TARGET = himplatforminputcontextplugin

QT += core-private gui-private

SOURCES += qhildonplatforminputcontextmain.cpp \
           qhildonplatforminputcontext.cpp

HEADERS += qhildonplatforminputcontext.h

OTHER_FILES += him.json

PLUGIN_TYPE = platforminputcontexts
PLUGIN_EXTENDS = -
PLUGIN_CLASS_NAME = QHildonPlatformInputContextPlugin
load(qt_plugin)
