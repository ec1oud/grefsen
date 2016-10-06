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
import QtQuick.Window 2.2
import QtWayland.Compositor 1.0
import Grefsen 1.0

WaylandOutput {
    id: output
    property alias surfaceArea: compositorArea // Chrome instances are parented to compositorArea
    window: Window {
        id: screen

        property QtObject output
        property Item customizedBackground: desktopLoader.item

        width: 1024
        height: 768
        color: "black"

        WaylandMouseTracker {
            id: mouseTracker
            anchors.fill: parent
            windowSystemCursorEnabled: true

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
            }
            /*
            Loader {
                anchors.fill: parent
                source: "Keyboard.qml"
            }
            WaylandCursorItem {
                id: cursor
                inputEventsEnabled: false
                x: mouseTracker.mouseX - hotspotX
                y: mouseTracker.mouseY - hotspotY

                inputDevice: output.compositor.defaultInputDevice
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
