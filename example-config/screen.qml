import QtQuick 2.6
import Grefsen 1.0 as Grefsen

Image {
    fillMode: Image.PreserveAspectCrop
    source: Grefsen.env.home + ".config/grefsen/Oslo_mot_Grefsentoppen_fra_Ekeberg.jpg"
    //source: "qrc:/images/background.jpg"

    Grefsen.LeftSlidePanel {
        id: leftPanel
        Text { color: "white"; text: "foo" }
        Rectangle {
            width: parent.width
            height: 40
            color: ma.containsPress ? "red" : ma.containsMouse ? "blue" : "white"
            MouseArea {
                id: ma
                anchors.fill: parent
                hoverEnabled: true
                onClicked: console.log("clicked")
            }
        }
        Text { color: "white"; text: "bar" }

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
