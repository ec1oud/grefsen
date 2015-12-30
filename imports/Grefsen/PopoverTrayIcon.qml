import QtQuick 2.5

MouseArea {
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
        anchors.left: icon.right
        anchors.top: icon.top
    }

    onClicked: {
        if (loader.sourceComponent == undefined)
            loader.sourceComponent = popover
        else
            loader.sourceComponent = undefined
    }
}
