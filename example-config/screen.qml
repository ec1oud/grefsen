import QtQuick 2.6
import Grefsen 1.0 as Grefsen // workaround for QTBUG-50219
import Grefsen 1.0

Image {
    fillMode: Image.PreserveAspectCrop
    source: Grefsen.env.home + ".config/grefsen/Oslo_mot_Grefsentoppen_fra_Ekeberg.jpg"
    //source: "qrc:/images/background.jpg"

    property bool fullscreen: true // TODO doesn't really fit here
    // TODO set the icon theme

    LeftSlidePanel {
        id: leftPanel

        LauncherMenuIcon { }

        LauncherIcon {
            path: "/usr/bin/konsole"
            icon: "utilities-terminal"
        }

        LauncherIcon {
            path: "/usr/bin/qtcreator"
            icon: "QtProject-qtcreator"
        }

        LauncherIcon {
            path: "fancybrowser"
            icon: "internet-web-browser"
        }

        LauncherIcon {
            path: "quasselclient"
            icon: "quassel"
        }

        PopoverTrayIcon {
            popover: ConnmanPopover { }
            icon: "preferences-system-network"
        }

        QuitButton { }

        Shortcut {
            sequence: "Meta+A" // maybe not the best one... or maybe we don't need it at all
            onActivated: leftPanel.toggle()
        }
    }

    Shortcut {
        sequence: "Ctrl+Alt+Backspace"
        onActivated: Qt.quit()
    }
}
