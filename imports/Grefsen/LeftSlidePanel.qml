import QtQuick 2.1
import QtGraphicalEffects 1.0

/*!
    A panel which slides out from the left edge of the screen.

    You can do that with your finger on the touchscreen, by mouseover,
    or by activating one of the open/close/toggle functions.

    Intended to be Fitts Law compliant: the content Item goes just to the
    screen edges, so that a button whose MouseArea goes to the top and/or left
    edge can be activated by slamming the mouse cursor into the edge and
    clicking.
*/
Flickable {
    id: root
    width: 100
    height: 480 // rect.childrenRect.height + 20
    anchors.left: parent.left
    y: -10
    z: 10000
    contentWidth: width * 2
    contentHeight: height

    function open() {
        contentX = 0
    }

    function close() {
        contentX = width
    }

    function toggle() {
        if (contentX < width / 2)
            contentX = width
        else
            contentX = 0
    }

    default property alias __content: contentContainer.data
    onDraggingChanged: if (!dragging) {
       if (horizontalVelocity > 0)
           flick(-1500, 0)
       else
           flick(1500, 0)
    }
    flickableDirection: Flickable.HorizontalFlick
    rebound: Transition {
        NumberAnimation {
            properties: "x"
            duration: 300
            easing.type: Easing.OutBounce
        }
    }

    RectangularGlow {
        id: effect
        width: root.width
        height: root.height
        glowRadius: 10
        spread: 0.2
        color: "cyan"
        cornerRadius: rect.radius + glowRadius

        Rectangle {
            id: rect
            anchors.fill: parent
            anchors.leftMargin: -200
            color: "black"
            radius: 10
            antialiasing: true

            Item {
                id: contentContainer
                anchors.fill: parent
                anchors.leftMargin: 200
                anchors.topMargin: 10
                anchors.margins: 6
            }
        }
        MouseArea {
            anchors.left: rect.right
            height: rect.height
            width: 10
            hoverEnabled: true
            onEntered: root.open()
        }
    }
}
