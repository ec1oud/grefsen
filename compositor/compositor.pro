QT += gui qml quick
CONFIG += link_pkgconfig
QMAKE_CXXFLAGS += -std=c++11
TARGET = ../grefsen

SOURCES += *.cpp

HEADERS += *.h

INCLUDEPATH += /usr/include/glib-2.0 /usr/lib/glib-2.0/include

OTHER_FILES = \
    qml/main.qml \
    qml/Screen.qml \
    qml/Chrome.qml

RESOURCES += grefsen.qrc

OBJECTS_DIR = .obj
MOC_DIR = .moc
RCC_DIR = .rcc

PKGCONFIG += glib-2.0

sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS grefsen.pro
