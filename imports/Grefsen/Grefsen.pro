TEMPLATE = lib
TARGET  = grefsenplugin
TARGETPATH = Grefsen
QT += qml quick xml
CONFIG += link_pkgconfig
QMAKE_CXXFLAGS += -std=c++11
PKGCONFIG += glib-2.0 Qt5Xdg

SOURCES += \
    plugin.cpp \
    iconprovider.cpp \
    launchermodel.cpp

HEADERS += \
    iconprovider.h \
    launchermodel.h

OTHER_FILES += *.qml
