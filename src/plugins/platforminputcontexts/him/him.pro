TARGET = himplatforminputcontextplugin

QT += core-private gui-private core

SOURCES += qhildonplatforminputcontextmain.cpp \
           qhildonplatforminputcontext.cpp \
           qhildonplatforminputeventfilter.cpp

HEADERS += qhildonplatforminputcontext.h \
           qhildonplatforminputeventfilter.h \


OTHER_FILES += him.json

PLUGIN_TYPE = platforminputcontexts
PLUGIN_EXTENDS = -
PLUGIN_CLASS_NAME = QHildonPlatformInputContextPlugin
load(qt_plugin)

QMAKE_USE += xcb_xlib

!qtConfig(system-xcb) {
    QMAKE_USE += xcb-static xcb
} else {
    qtConfig(xkb): QMAKE_USE += xcb_xkb
    qtConfig(xcb-render): QMAKE_USE += xcb_render
    QMAKE_USE += xcb_syslibs
}

QMAKE_LFLAGS = $(QMAKE_LFLAGS) -lQt5XcbQpa
QMAKE_CXXFLAGS = $(QMAKE_CXXFLAGS) -Ixcb
