QT += gui qml
CONFIG += link_pkgconfig

SOURCES += src/*.cpp

HEADERS += src/*.h

INCLUDEPATH += /usr/local/include/mlite5 /usr/include/glib-2.0 /usr/lib/glib-2.0/include

OTHER_FILES = \
    qml/main.qml \
    qml/Screen.qml \
    qml/Chrome.qml \
    images/background.jpg \

RESOURCES += pure-qml.qrc

PKGCONFIG += mlite5 glib-2.0

target.path = $$[QT_INSTALL_EXAMPLES]/wayland/pure-qml
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS pure-qml.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/wayland/pure-qml
INSTALLS += target sources
