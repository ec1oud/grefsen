QT += gui qml
CONFIG += link_pkgconfig

SOURCES += src/*.cpp

HEADERS += src/*.h

INCLUDEPATH += /usr/local/include/mlite5 /usr/include/glib-2.0 /usr/lib/glib-2.0/include

OTHER_FILES = \
    qml/main.qml \
    qml/Screen.qml \
    qml/Chrome.qml

RESOURCES += grefsen.qrc

PKGCONFIG += mlite5 glib-2.0

sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS grefsen.pro
