/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/QUrl>
#include <QtCore/QDebug>

#include <QtGui/QGuiApplication>

#include <QtQml/QQmlApplicationEngine>
#include <QtQml/qqml.h>
#include <QDebug>
#include <QDir>
#include <QFontDatabase>

#include "iconprovider.h"
#include "launcherfiltermodel.h"
#include "launcherfoldermodel.h"
#include "launcheritem.h"
#include "launchermodel.h"
#include "launchermonitor.h"
#include "launcherwatchermodel.h"
#include "processlauncher.h"
#include "qobjectlistmodel.h"

#include "stackableitem.h"

static void registerTypes()
{
    qmlRegisterType<WaylandProcessLauncher>("com.theqtcompany.wlprocesslauncher", 1, 0, "ProcessLauncher");
    qmlRegisterType<StackableItem>("com.theqtcompany.wlcompositor", 1, 0, "StackableItem");

    qmlRegisterType<LauncherModel>("org.nemomobile.lipstick", 0, 1, "LauncherModel");
    qmlRegisterType<LauncherWatcherModel>("org.nemomobile.lipstick", 0, 1, "LauncherWatcherModel");
    qmlRegisterType<LauncherItem>("org.nemomobile.lipstick", 0, 1, "LauncherItem");
    qmlRegisterType<LauncherFilterModel>("org.nemomobile.lipstick", 0, 1, "LauncherFilterModel"); // TODO not part of lipstick
    qmlRegisterType<LauncherFolderModel>("org.nemomobile.lipstick", 0, 1, "LauncherFolderModel");
    qmlRegisterType<LauncherFolderItem>("org.nemomobile.lipstick", 0, 1, "LauncherFolderItem");
    qmlRegisterType<QObjectListModel>();
}

int main(int argc, char *argv[])
{
    if (!qEnvironmentVariableIsSet("QT_XCB_GL_INTEGRATION"))
        qputenv("QT_XCB_GL_INTEGRATION", "xcb_egl"); // use xcomposite-glx if no EGL
    QGuiApplication app(argc, argv);

    if (QFontDatabase::addApplicationFont(":/fonts/FontAwesome.otf"))
        qWarning("failed to load FontAwesome from resources - "
                 "maybe OK if you have it installed as a system font");

    registerTypes();

    QQmlApplicationEngine appEngine;
    // TODO make this work...
    // for now you still need QML2_IMPORT_PATH=imports
    appEngine.addPluginPath(QDir::current().filePath(QStringLiteral("imports")));
    appEngine.addImageProvider(QLatin1String("icon"), new IconProvider);
    appEngine.load(QUrl("qrc:///qml/main.qml"));

//    if (app.arguments().contains(QLatin1String("-f"))) ... TODO find the window, make it fullscreen

    return app.exec();
}
