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
#include <QQmlContext>
#include <QQuickItem>
#include <QScreen>
#include <QWindow>

#include "processlauncher.h"
#include "stackableitem.h"

#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/prctl.h>

static QLatin1String glassPaneName("glassPane");

static QByteArray grefsonExecutablePath;
static qint64 grefsonPID;
static void *signalHandlerStack;

extern "C" void signalHandler(int signal)
{
#ifdef Q_WS_X11
    // Kill window since it's frozen anyway.
    if (QX11Info::display())
        close(ConnectionNumber(QX11Info::display()));
#endif
    pid_t pid = fork();
    switch (pid) {
    case -1: // error
        break;
    case 0: // child
        kill(grefsonPID, 9);
        fprintf(stderr, "crashed (PID %d): respawn %s\n", grefsonPID, grefsonExecutablePath.constData());
        execl(grefsonExecutablePath.constData(), grefsonExecutablePath.constData(), (char *) 0);
        _exit(EXIT_FAILURE);
    default: // parent
        prctl(PR_SET_PTRACER, pid, 0, 0, 0);
        waitpid(pid, 0, 0);
        _exit(EXIT_FAILURE);
        break;
    }
}

static void setupSignalHandler()
{
    // Setup an alternative stack for the signal handler. This way we are able to handle SIGSEGV
    // even if the normal process stack is exhausted.
    stack_t ss;
    ss.ss_sp = signalHandlerStack = malloc(SIGSTKSZ); // Usual requirements for alternative signal stack.
    if (ss.ss_sp == 0) {
        qWarning("Warning: Could not allocate space for alternative signal stack (%s).", Q_FUNC_INFO);
        return;
    }
    ss.ss_size = SIGSTKSZ;
    ss.ss_flags = 0;
    if (sigaltstack(&ss, 0) == -1) {
        qWarning("Warning: Failed to set alternative signal stack (%s).", Q_FUNC_INFO);
        return;
    }

    // Install signal handler.
    struct sigaction sa;
    if (sigemptyset(&sa.sa_mask) == -1) {
        qWarning("Warning: Failed to empty signal set (%s).", Q_FUNC_INFO);
        return;
    }
    sa.sa_handler = &signalHandler;
    // SA_RESETHAND - Restore signal action to default after signal handler has been called.
    // SA_NODEFER - Don't block the signal after it was triggered (otherwise blocked signals get
    // inherited via fork() and execve()). Without this the signal will not be delivered to the
    // restarted Qt Creator.
    // SA_ONSTACK - Use alternative stack.
    sa.sa_flags = SA_RESETHAND | SA_NODEFER | SA_ONSTACK;
    // See "man 7 signal" for an overview of signals.
    // Do not add SIGPIPE here, QProcess and QTcpSocket use it.
    const int signalsToHandle[] = { SIGILL, SIGABRT, SIGFPE, SIGSEGV, SIGBUS, 0 };
    for (int i = 0; signalsToHandle[i]; ++i)
        if (sigaction(signalsToHandle[i], &sa, 0) == -1)
            qWarning("Failed to install signal handler for signal \"%s\"", strsignal(signalsToHandle[i]));
}

static void registerTypes()
{
    qmlRegisterType<WaylandProcessLauncher>("com.theqtcompany.wlprocesslauncher", 1, 0, "ProcessLauncher");
    qmlRegisterType<StackableItem>("com.theqtcompany.wlcompositor", 1, 0, "StackableItem");
}

static void screenCheck()
{
    foreach (const QScreen *scr, QGuiApplication::screens()) {
        qDebug() << "Screen" << scr->name() << scr->geometry() << scr->physicalSize()
                 << "DPI: log" << scr->logicalDotsPerInch() << "phys" << scr->physicalDotsPerInch();
    }
}

int main(int argc, char *argv[])
{
    if (!qEnvironmentVariableIsSet("QT_XCB_GL_INTEGRATION"))
        qputenv("QT_XCB_GL_INTEGRATION", "xcb_egl"); // use xcomposite-glx if no EGL
    if (!qEnvironmentVariableIsSet("QT_WAYLAND_DISABLE_WINDOWDECORATION"))
        qputenv("QT_WAYLAND_DISABLE_WINDOWDECORATION", "1");
    if (!qEnvironmentVariableIsSet("QT_LABS_CONTROLS_STYLE"))
        qputenv("QT_LABS_CONTROLS_STYLE", "Universal");
    QGuiApplication app(argc, argv);
//    app.setAttribute(Qt::AA_DisableHighDpiScaling); // better use the env variable... but that's not enough on eglfs
    grefsonExecutablePath = app.applicationFilePath().toLocal8Bit();
    grefsonPID = QCoreApplication::applicationPid();

    if (app.arguments().contains(QLatin1String("-r"))) // respawn on crash
        setupSignalHandler();
    screenCheck();

    {
        QFontDatabase fd;
        if (!fd.families().contains(QLatin1String("FontAwesome")))
            if (QFontDatabase::addApplicationFont(":/fonts/FontAwesome.otf"))
                qWarning("failed to load FontAwesome from resources");
    }

    registerTypes();
    qputenv("QT_QPA_PLATFORM", "wayland"); // not for grefsen but for child processes

    QQmlApplicationEngine appEngine;
    appEngine.addImportPath(app.applicationDirPath() + QLatin1String("/imports"));
    appEngine.load(QUrl("qrc:///qml/main.qml"));
    appEngine.rootContext()->setContextProperty(glassPaneName,
        appEngine.rootObjects().first()->findChild<QQuickItem*>(glassPaneName));

//    if (app.arguments().contains(QLatin1String("-f"))) ... TODO find the window, make it fullscreen

    return app.exec();
}
