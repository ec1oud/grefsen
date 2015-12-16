import QtQuick 2.5
import org.nemomobile.lipstick 0.1
import Qt.labs.controls 1.0
//import Grefson 1.0

Rectangle {
    border.color: "black"
    color: "transparent"
    anchors.margins: 10
    radius: 10
    TextField {
        id: searchField
        width: parent.width - 12
        x: 6; y: 6
        focus: true
//        onVisibleChanged: if (visible) forceActiveFocus()
    }
    GridView {
        anchors.fill: parent
        anchors.topMargin: searchField.height + 6
//        model: LauncherModel { id: launcherModel }
        property LauncherModel launcherModel: LauncherModel { id: launcherModel }
        model: LauncherFilterModel {
            id: launcherFilterModel
            sourceModel: launcherModel
            filterSubstring: searchField.text
        }
        delegate: Item {
            width: 100
            height: 100
            Rectangle {
                radius: 10
                anchors.fill: parent
                anchors.margins: 5
                opacity: 0.35
            }
            LauncherIcon {
                icon: object.iconId
                path: object.exec
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.margins: 6
                text: object.title
                Component.onCompleted: console.log(index + " " + object.entryType)
            }
        }
    }

    Component.onCompleted: {
        console.log("lm has " + launcherModel.rowCount())
    }
}
