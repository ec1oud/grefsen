/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt-project.org/legal
**
** This file is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 3 as published by the Free Software Foundation
** and appearing in the file LICENSE included in the packaging
** of this file.
**
** This code is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU Lesser General Public License for more details.
**
****************************************************************************/

import QtQuick 2.6
import Qt.labs.handlers 1.0
import QtWayland.Compositor 1.0
import QtGraphicalEffects 1.0
import com.theqtcompany.wlcompositor 1.0

StackableItem {
    id: rootChrome
    property alias shellSurface: surfaceItem.shellSurface
    property alias moveItem: surfaceItem.moveItem
    property bool decorationVisible: false
    property alias destroyAnimation : destroyAnimationImpl

    property int marginWidth : surfaceItem.isFullscreen ? 0 : (surfaceItem.isPopup ? 1 : 6)
    property int titlebarHeight : surfaceItem.isPopup || surfaceItem.isFullscreen ? 0 : 25
    property string screenName: ""

    x: surfaceItem.moveItem.x - surfaceItem.output.geometry.x
    y: surfaceItem.moveItem.y - surfaceItem.output.geometry.y
    height: surfaceItem.height + marginWidth + titlebarHeight
    width: surfaceItem.width + 2 * marginWidth
    visible: surfaceItem.valid

    Rectangle {
        id: decoration
        anchors.fill: parent
        border.width: 1
        radius: marginWidth
        border.color: (resizeArea.pressed || resizeArea.containsMouse) ? "#ffc02020" :"#305070a0"
        color: "#50ffffff"
        visible: rootChrome.decorationVisible && !surfaceItem.isFullscreen

        MouseArea {
            id: resizeArea
            anchors.fill: parent
            hoverEnabled: true
            //cursorShape: Qt.SizeFDiagCursor
            property int pressX
            property int pressY
            property int startW
            property int startH

            //bitfield: top, left, bottom, right
            property int edges
            onPressed: {
                edges = 0
                pressX = mouse.x; pressY = mouse.y
                startW = rootChrome.width; startH = rootChrome.height
                if (mouse.y > rootChrome.height - titlebarHeight)
                    edges |= 4 //bottom edge
                if (mouse.x > rootChrome.width - titlebarHeight)
                    edges |= 8 //right edge
            }
            onMouseXChanged: {
                if (pressed) {
                    var w = startW
                    var h = startH
                    if (edges & 8)
                        w += mouse.x - pressX
                    if (edges & 4)
                        h += mouse.y - pressY
                    rootChrome.requestSize(w, h)
                    console.log("resize " + rootChrome + " " + rootChrome.x + ", ", rootChrome.y)
                }
            }
        }

        Item {
            id: titlebar
            anchors.margins: marginWidth
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: titlebarHeight - marginWidth
            visible: !surfaceItem.isPopup

            LinearGradient {
                anchors.fill: parent
                start: Qt.point(0, 0)
                end: Qt.point(0, height)
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "#50ffffff" }
                    GradientStop { position: 1.0; color: "#e0ffffff" }
                }
            }

            Text {
                color: "gray"
                text: surfaceItem.shellSurface ? surfaceItem.shellSurface.title : ""
                anchors.margins: marginWidth

                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
            }

            MouseArea {
                id: moveArea
                anchors.fill: parent
                drag.target: surfaceItem.moveItem
                hoverEnabled: true
                acceptedButtons: Qt.LeftButton | Qt.MiddleButton |Qt.RightButton
                onPressed: {
                    surfaceItem.moveItem.moving = true
                    if (mouse.button === Qt.LeftButton) {
                        rootChrome.raise()
                    } else if (mouse.button === Qt.RightButton) {
                        //console.log("right button")
                        // TODO add menu
                    } else if (mouse.button === Qt.MiddleButton) {
                        rootChrome.lower()
                    }
                }
                onReleased: surfaceItem.moveItem.moving = false
                //cursorShape: Qt.OpenHandCursor
            }

            MouseArea {
                id: closeButton
                visible: !surfaceItem.isTransient
                height: 20
                width: 25
                anchors.margins: marginWidth
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                onClicked: shellSurface.surface.client.close()
                hoverEnabled: true
                RectangularGlow {
                    id: effect
                    anchors.fill: closeIcon
                    anchors.margins: 2
                    glowRadius: 5
                    cornerRadius: glowRadius
                    spread: 0.4
                    color: "red"
                    opacity: parent.containsMouse ? 0.5 : 0
                }
                Text {
                    id: closeIcon
                    anchors.centerIn: parent
                    font.pixelSize: parent.height
                    font.family: "FontAwesome"
                    text: "\uf00d"
                }
            }
        }
    }
    function requestSize(w, h) {
        //console.log("request size " + w + ", " + h)
        surfaceItem.shellSurface.sendConfigure(Qt.size(w - 2 * marginWidth, h - titlebarHeight - marginWidth), WlShellSurface.DefaultEdge)
    }

    SequentialAnimation {
        id: destroyAnimationImpl
        ParallelAnimation {
            NumberAnimation { target: scaleTransform; property: "yScale"; to: 2/height; duration: 150 }
            NumberAnimation { target: scaleTransform; property: "xScale"; to: 0.4; duration: 150 }
        }
        NumberAnimation { target: scaleTransform; property: "xScale"; to: 0; duration: 150 }
        ScriptAction { script: { rootChrome.destroy(); } }
    }

    ParallelAnimation {
        id: createAnimationImpl
        NumberAnimation { target: scaleTransform; property: "yScale"; from: 0; to: 1; duration: 150 }
        NumberAnimation { target: scaleTransform; property: "xScale"; from: 0; to: 1; duration: 150 }
    }

    SequentialAnimation {
        id: receivedFocusAnimation
        ParallelAnimation {
            NumberAnimation { target: scaleTransform; property: "yScale"; to: 1.02; duration: 100; easing.type: Easing.OutQuad }
            NumberAnimation { target: scaleTransform; property: "xScale"; to: 1.02; duration: 100; easing.type: Easing.OutQuad }
        }
        ParallelAnimation {
            NumberAnimation { target: scaleTransform; property: "yScale"; to: 1; duration: 100; easing.type: Easing.InOutQuad }
            NumberAnimation { target: scaleTransform; property: "xScale"; to: 1; duration: 100; easing.type: Easing.InOutQuad }
        }
    }

    transform: [
        Scale {
            id:scaleTransform
            origin.x: rootChrome.width / 2
            origin.y: rootChrome.height / 2

        }
    ]

    ShellSurfaceItem {
        id: surfaceItem
        property bool valid: false
        property bool isPopup: false
        property bool isTransient: false
        property bool isFullscreen: false

        opacity: surfaceItem.moveItem.moving ? 0.5 : 1.0
        inputEventsEnabled: !pinch3.active && !metaDragHandler.active && !altDragHandler.active

        x: marginWidth
        y: titlebarHeight

        DragHandler {
            id: metaDragHandler
            acceptedModifiers: Qt.MetaModifier
            target: surfaceItem.moveItem
            property var movingBinding: Binding {
                target: surfaceItem.moveItem
                property: "moving"
                value: metaDragHandler.active
            }
        }

        DragHandler {
            id: altDragHandler
            acceptedModifiers: Qt.AltModifier
            target: surfaceItem.moveItem
            property var movingBinding: Binding {
                target: surfaceItem.moveItem
                property: "moving"
                value: altDragHandler.active
            }
        }

        Connections {
            target: shellSurface
            ignoreUnknownSignals: true

            onActivatedChanged: { // xdg_shell only
                if (shellSurface.activated)
                    receivedFocusAnimation.start();
            }
            onSetPopup: {
                surfaceItem.isPopup = true
                decoration.visible = false
            }
            onSetTransient: {
                surfaceItem.isTransient = true
            }
            onSetFullScreen: {
                surfaceItem.isFullscreen = true
                rootChrome.x = 0
                rootChrome.y = 0
            }
        }

        onSurfaceDestroyed: {
            destroyAnimationImpl.start();
        }

        onWidthChanged: {
            valid =  !surface.cursorSurface && surface.size.width > 0 && surface.size.height > 0
        }

        onValidChanged: if (valid) {
            if (isFullscreen) {
                rootChrome.requestSize(output.geometry.width, output.geometry.height)
            } else {
                createAnimationImpl.start()
            }
        }
    }

    PinchHandler {
        id: pinch3
        objectName: "3-finger pinch"
        minimumPointCount: 3
        maximumPointCount: 3
        minimumScale: 0.1
        minimumRotation: 0
        maximumRotation: 0
        onActiveChanged: if (!active) {
            rootChrome.requestSize(width * scale, height * scale)
            rootChrome.scale = 1
        }
    }

    Rectangle {
        visible: surfaceItem.moveItem.moving
        border.color: "white"
        color: "black"
        radius: 5
        anchors.centerIn: parent
        width: height * 10
        height: moveGeometryText.implicitHeight * 1.5
        Text {
            id: moveGeometryText
            color: "white"
            anchors.centerIn: parent
            text: Math.round(rootChrome.x) + "," + Math.round(rootChrome.y) + " on " + rootChrome.screenName
        }
    }
}
