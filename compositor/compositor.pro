QT += gui qml quick
CONFIG += link_pkgconfig
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

PKGCONFIG += glib-2.0 Qt5Xdg

sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS grefsen.pro
