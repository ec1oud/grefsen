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
import QtWayland.Compositor 1.0

WaylandCompositor {
    id: comp

    property var primarySurfacesArea: null

    Screen {
        compositor: comp
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
        onWlShellSurfaceCreated: {
            // Qt programs have been told not to decorate themselves
            chromeComponent.createObject(defaultOutput.surfaceArea, { "shellSurface": shellSurface, "decorationVisible": true } );
        }
    }

    XdgShellV5 {
        property variant viewsBySurface: ({})
        onXdgSurfaceCreated: {
            var item = chromeComponent.createObject(defaultOutput.surfaceArea, { "shellSurface": xdgSurface } );
            viewsBySurface[xdgSurface.surface] = item;
        }
        onXdgPopupCreated: {
            var parentView = viewsBySurface[xdgPopup.parentSurface];
            var item = chromeComponent.createObject(parentView, { "shellSurface": xdgPopup } );
            viewsBySurface[xdgPopup.surface] = item;
        }
    }

    TextInputManager {
    }

    onSurfaceRequested: {
        var surface = surfaceComponent.createObject(comp, { } );
        surface.initialize(comp, client, id, version);
    }
}
