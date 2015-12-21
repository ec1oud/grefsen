import QtQuick 2.5
import Qt.labs.controls 1.0
import Grefsen 1.0 as Grefsen

Grefsen.HoverArea {
    id: root
    width: 612
    height: 1080
    signal close
    onExited: root.close()

    Rectangle {
        anchors.fill: parent
        border.color: "black"
        color: "#555"
        radius: 10
        opacity: 0.5
    }

    TextField {
        id: searchField
        width: parent.width - 12 - height - 6
        x: 6; y: 6
        focus: true
    }
    MouseArea {
        height: searchField.height
        width: height
        anchors.right: parent.right
        anchors.rightMargin: 6
        y: 6
        onClicked: root.close()
        Text {
            id: closeIcon
            anchors.centerIn: parent
            font.pixelSize: parent.height - 6
            font.family: "FontAwesome"
            text: "\uf00d"
            color: "#555"
        }
    }

    GridView {
        anchors.fill: parent
        anchors.topMargin: searchField.height + 6
        anchors.bottomMargin: 0
        anchors.margins: 6
        clip: true
//        property LauncherModel launcherModel: LauncherModel { id: launcherModel }
//        model: LauncherFilterModel {
//            sourceModel: launcherModel
//            filterSubstring: searchField.text
//        }
        delegate: Item {
            width: 100
            height: 100
            Rectangle {
                radius: 10
                anchors.fill: parent
                anchors.margins: 5
                opacity: 0.35
            }
//            LauncherIcon {
//                icon: object.iconId
//                path: object.exec
//                width: 80
//                anchors.horizontalCenter: parent.horizontalCenter
//            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.margins: 6
                elide: Text.ElideRight
                text: object.title
                width: 80
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }
}
