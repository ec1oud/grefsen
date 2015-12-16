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
import Grefsen 1.0 as Grefsen

WaylandOutput {
    id: output
    property alias surfaceArea: background // Chrome instances are parented to background
    window: Window {
        id: screen

        property QtObject output
        property Item customizedBackground: desktopLoader.item

        width: 1024
        height: 768
        color: "black"

        Component.onCompleted: {
            if (customizedBackground.hasOwnProperty("fullscreen") && customizedBackground.fullscreen)
                visibility = Window.FullScreen
        }

        WaylandMouseTracker {
            id: mouseTracker
            anchors.fill: parent
            enableWSCursor: true

            Item {
                id: background
                anchors.fill: parent
                Loader {
                    id: desktopLoader
                    anchors.fill: parent
                    source: "file://" + Grefsen.env.home + ".config/grefsen/screen.qml"
                }
            }

            Item {
                id: glassPane
                objectName: "glassPane"
                anchors.fill: parent
            }
        }
    }
}
