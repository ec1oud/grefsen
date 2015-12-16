/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt-project.org/legal
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/


import QtQuick 2.6
import QtQuick.Window 2.2
import QtWayland.Compositor 1.0
import Grefsen 1.0 as Grefsen

WaylandOutput {
    id: output
    property alias surfaceArea: background
    window: Window {
        id: screen

        property QtObject output

        width: 1024; height: 768
        color: "black"
        visible: true

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
            WaylandCursorItem {
                id: cursor
                inputEventsEnabled: false
                x: mouseTracker.mouseX - hotspotX
                y: mouseTracker.mouseY - hotspotY

                inputDevice: output.compositor.defaultInputDevice
            }
        }
        // TODO remove this and make it work from ~/.config/screen.qml
        Shortcut {
            sequence: "Ctrl+Alt+Backspace"
            onActivated: Qt.quit()
        }
    }
}
