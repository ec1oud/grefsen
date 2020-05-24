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

import QtQuick 2.8
import QtQuick.Window 2.3
import QtWayland.Compositor 1.0
import Grefsen 1.0

WaylandOutput {
    id: output
    property variant viewsBySurface: ({})
    property alias surfaceArea: compositorArea // Chrome instances are parented to compositorArea
    property alias targetScreen: win.screen
    sizeFollowsWindow: true

    window: Window {
        id: win
        property Item customizedBackground: desktopLoader.item
        color: "black"
        title: "Grefsen on " + Screen.name

        WaylandMouseTracker {
            id: mouseTracker
            objectName: "wmt on " + Screen.name
            anchors.fill: parent
            windowSystemCursorEnabled: false

            Item {
                id: background
                anchors.fill: parent
                Loader {
                    id: desktopLoader
                    anchors.fill: parent
                    source: "file://" + Env.grefsenconfig + "screen.qml"
                }
            }
            Item {
                id: compositorArea
                anchors.fill: parent
            }
            WaylandCursorItem {
                id: cursor
                inputEventsEnabled: false
                x: mouseTracker.mouseX
                y: mouseTracker.mouseY
                seat: output.compositor.defaultSeat
                visible: mouseTracker.containsMouse
            }
            /*
            Loader {
                anchors.fill: parent
                source: "Keyboard.qml"
            }
            */
            Item {
                id: glassPane
                objectName: "glassPane"
                anchors.fill: parent
            }
        }
    }
}
