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

    extensions: [
        Shell {
            id: defaultShell


            onCreateShellSurface: {
                var item = chromeComponent.createObject(defaultOutput.surfaceArea, { "surface": surface } );
                item.shellSurface.initialize(defaultShell, surface, resource);
                item.findPositionForWindow()
            }

            Component.onCompleted: {
                initialize();
            }
        }
    ]

    onCreateSurface: {
        var surface = surfaceComponent.createObject(comp, { } );
        surface.initialize(comp, client, id, version);

    }
}
