import QtQuick 2.5

MouseArea {
    id: root
    width: parent.width
    height: width

    property string icon: ""
    property Component popover: undefined
    Image {
        id: icon
        source: "image://icon/" + icon
        sourceSize.width: 64
        sourceSize.height: 64
        anchors.centerIn: parent
    }

    Loader {
        id: loader
        anchors.left: root.right
        anchors.top: root.top
        onItemChanged: if (item) item.pointLeftToY = root.height / 2
    }

    onClicked: {
        if (loader.sourceComponent == undefined)
            loader.sourceComponent = popover
        else
            loader.sourceComponent = undefined
    }
}
