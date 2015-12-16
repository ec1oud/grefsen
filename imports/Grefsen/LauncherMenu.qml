import QtQuick 2.5
import org.nemomobile.lipstick 0.1

Rectangle {
    border.color: "black"
    color: "transparent"
    anchors.margins: 10
    radius: 10
    GridView {
        anchors.fill: parent
        model: LauncherModel { id: launcherModel }
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
                Component.onCompleted: console.log(index + " " + JSON.stringify(object))
            }
        }
    }

    Component.onCompleted: {
        console.log("lm has " + launcherModel.rowCount())
    }
}
