/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
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

import QtQml 2.2
import QtQuick 2.6
import QtWayland.Compositor 1.0

WaylandCompositor {
    id: comp

    Instantiator {
        id: screens
        model: Qt.application.screens

        delegate: Screen {
            compositor: comp
            targetScreen: modelData
            Component.onCompleted: if (!comp.defaultOutput) comp.defaultOutput = this
            position: Qt.point(virtualX, virtualY)
        }
    }

    Component {
        id: chromeComponent
        Chrome {
        }
    }

    Component {
        id: surfaceComponent
        WaylandSurface {
        }
    }

    QtWindowManager {
        id: qtWindowManager
        onShowIsFullScreenChanged: console.debug("Show is fullscreen hint for Qt applications:", showIsFullScreen)
    }

    WlShell {
        onWlShellSurfaceCreated: handleShellSurfaceCreated(shellSurface)
    }

    XdgShellV5 {
        onXdgSurfaceCreated: handleShellSurfaceCreated(xdgSurface)
        onXdgPopupCreated: handleShellSurfaceCreated(xdgPopup)
    }

    TextInputManager {
    }

    function createShellSurfaceItem(shellSurface, moveItem, output) {
        var parentSurfaceItem = output.viewsBySurface[shellSurface.parentSurface];
        var parent = parentSurfaceItem || output.surfaceArea;
        var item = chromeComponent.createObject(parent, {
            "shellSurface": shellSurface,
            "moveItem": moveItem,
            "output": output,
            "decorationVisible": true
        });
        if (parentSurfaceItem) {
            item.x += output.position.x;
            item.y += output.position.y;
        }
        output.viewsBySurface[shellSurface.surface] = item;
    }

    function handleShellSurfaceCreated(shellSurface) {
        var moveItem = moveItemComponent.createObject(defaultOutput.surfaceArea, {
            "x": screens.objectAt(0).position.x,
            "y": screens.objectAt(0).position.y,
            "width": Qt.binding(function() { return shellSurface.surface.width; }),
            "height": Qt.binding(function() { return shellSurface.surface.height; })
        });
        for (var i = 0; i < screens.count; ++i)
            createShellSurfaceItem(shellSurface, moveItem, screens.objectAt(i));
    }
}
