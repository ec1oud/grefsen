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

import QtQuick 2.6
import QtWayland.Compositor 1.1
import Qt.labs.settings 1.0

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
        id: moveItemComponent
        Item {
            property bool moving: false
        }
    }

    QtWindowManager {
        id: qtWindowManager
        onShowIsFullScreenChanged: console.debug("Show is fullscreen hint for Qt applications:", showIsFullScreen)
    }

    WlShell {
        onWlShellSurfaceCreated: handleShellSurfaceCreated(null, shellSurface, shellSurface, true)
    }

    XdgShellV5 {
        onXdgSurfaceCreated: handleShellSurfaceCreated(null, xdgSurface, xdgSurface, true)
        onXdgPopupCreated: handleShellSurfaceCreated(xdgPopup.parentSurface, xdgPopup, xdgPopup, false)
    }

    XdgShellV6 {
        onToplevelCreated: handleShellSurfaceCreated(null, xdgSurface, toplevel, true)
        onPopupCreated: handleShellSurfaceCreated(popup.parentXdgSurface.surface, xdgSurface, popup, false)
    }

    TextInputManager {
    }

    defaultSeat.keymap {
        layout: keymapSettings.layout
        variant: keymapSettings.variant
        options: keymapSettings.options
        rules: keymapSettings.rules
        model: keymapSettings.model
    }
    Settings {
        id: keymapSettings
        category: "keymap"
        property string layout: "us"
        property string variant: "intl"
        property string options: "grp:shifts_toggle,compose:ralt,ctrl:nocaps"
        property string rules: ""
        property string model: ""
    }

    function createShellSurfaceItem(parentSurface, shellSurface, topLevel, moveItem, output, decorate) {
        var parentSurfaceItem = output.viewsBySurface[parentSurface];
        var parent = parentSurfaceItem || output.surfaceArea;
        var item = chromeComponent.createObject(parent, {
            "shellSurface": shellSurface,
            "topLevel": topLevel,
            "moveItem": moveItem,
            "output": output,
            "screenName": output.targetScreen.name,
            "decorationVisible": decorate
        });
        if (parentSurfaceItem) {
            item.x += output.position.x;
            item.y += output.position.y;
        }
        output.viewsBySurface[shellSurface.surface] = item;
    }

    function handleShellSurfaceCreated(parentSurface, shellSurface, topLevel, decorate) {
        var moveItem = moveItemComponent.createObject(defaultOutput.surfaceArea, {
            "x": screens.objectAt(0).position.x,
            "y": screens.objectAt(0).position.y,
            "width": Qt.binding(function() { return shellSurface.surface.width; }),
            "height": Qt.binding(function() { return shellSurface.surface.height; })
        });
        for (var i = 0; i < screens.count; ++i)
            createShellSurfaceItem(parentSurface, shellSurface, topLevel, moveItem, screens.objectAt(i), decorate);
    }
}
