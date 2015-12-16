import QtQuick 2.6
import Grefsen 1.0 as Grefsen

Image {
    fillMode: Image.PreserveAspectCrop
    source: Grefsen.env.home + ".config/grefsen/Oslo_mot_Grefsentoppen_fra_Ekeberg.jpg"
    //source: "qrc:/images/background.jpg"

    // TODO doesn't seem to work
    Shortcut {
        sequence: "Ctrl+Alt+Backspace"
        onActivated: Qt.quit()
    }
}
