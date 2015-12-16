import QtQuick 2.6
import Grefsen 1.0 as Grefsen

Image {
    fillMode: Image.PreserveAspectCrop
    source: Grefsen.env.home + ".config/grefsen/Oslo_mot_Grefsentoppen_fra_Ekeberg.jpg"
    //source: "qrc:/images/background.jpg"

    Grefsen.LeftSlidePanel {
        id: leftPanel

        Grefsen.LauncherIcon {
            // TODO need an image provider for icons, use desktop file etc.
            path: "/usr/bin/konsole"
            icon: "utilities-terminal"
        }

        Shortcut {
            sequence: "Meta+A" // maybe not the best one... or maybe we don't need it at all
            onActivated: leftPanel.toggle()
        }
    }

    Grefsen.LauncherMenu {
        anchors.fill: parent
    }

    Shortcut {
        sequence: "Ctrl+Alt+Backspace"
        onActivated: Qt.quit()
    }
}
