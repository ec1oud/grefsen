import QtQuick 2.5
import Qt.labs.controls 1.0
import Grefsen 1.0 as Grefsen

Grefsen.HoverArea {
    id: root
    width: 200
//    height: Math.min(1000, list.implicitHeight)
    height: 500
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

    ListView {
        id: list
        anchors.fill: parent
        anchors.topMargin: searchField.height + 6
        anchors.bottomMargin: 0
        anchors.margins: 6
        clip: true
//        property var launcherModel: Grefsen.LauncherModel { id: launcherModel }
//        model: LauncherFilterModel {
//            sourceModel: launcherModel
//            filterSubstring: searchField.text
//        }
        model: Grefsen.launcherModel.applicationMenu
        delegate: MouseArea {
            width: parent.width
            height: 32
//            property bool isApp: modelData.hasOwnProperty("exec")
//            onClicked: {
//                if (isApp)
//                    Grefsen.launcherModel.launch(modelData)
//                else
//                    Grefsen.launcherModel.openSubmenu(modelData.title)
//            }
            onClicked: Grefsen.launcherModel.select(modelData)

            Rectangle {
                radius: 2
                anchors.fill: parent
                anchors.margins: 1
                opacity: 0.35
            }
            Image {
                source: "image://icon/" + modelData.icon
                sourceSize.width: 24
                sourceSize.height: 24
                anchors.verticalCenter: parent.verticalCenter
                x: 4
            }
            Text {
                anchors.verticalCenter: parent.verticalCenter
                x: 30
                elide: Text.ElideRight
                text: modelData.title
//                Component.onCompleted: console.log(" "+ JSON.stringify(modelData))
                width: parent.width - x - 4
            }
        }
    }
}
