/****************************************************************************
**
** Copyright (C) 2015 Shawn Rutledge
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

#include <QCommandLineParser>
#include <QDebug>
#include <QDebug>
#include <QDir>
#include <QElapsedTimer>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QScreen>
#include <QUrl>
#include <QWindow>

#include <QtQml/qqml.h>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickItem>

#include "processlauncher.h"
#include "stackableitem.h"

#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static QLatin1String glassPaneName("glassPane");
static QByteArray grefsonExecutablePath;
static qint64 grefsonPID;
static void *signalHandlerStack;
static QString logFilePath;
static QElapsedTimer sinceStartup;

QString grefsenConfigDirPath(QDir::homePath() + "/.config/grefsen/");

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
        fprintf(stderr, "crashed (PID %lld SIG %d): respawn %s\n", grefsonPID, signal, grefsonExecutablePath.constData());
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

void qtMsgLog(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QFile log(logFilePath);
    if (!log.isOpen())
        log.open(QIODevice::WriteOnly | QIODevice::Text);
    char typeChar = ' ';
    switch (type) {
    case QtDebugMsg:
        typeChar = 'd';
        break;
    case QtInfoMsg:
        typeChar = 'i';
        break;
    case QtWarningMsg:
        typeChar = 'W';
        break;
    case QtCriticalMsg:
        typeChar = '!';
        break;
    case QtFatalMsg:
        typeChar = 'F';
    }
    char buf[64];
    qint64 ts = sinceStartup.elapsed();
    snprintf(buf, 64, "[%6lld.%03lld %c] ", ts / 1000, ts % 1000, typeChar);
    log.write(buf);
    if (context.function) {
        snprintf(buf, 64, "%s:%d: ", context.function, context.line);
        log.write(buf);
    }
    log.write(msg.toUtf8());
    log.write("\n");
    if (type == QtFatalMsg)
        abort();
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
    sinceStartup.start();
    if (!qEnvironmentVariableIsSet("QT_XCB_GL_INTEGRATION"))
        qputenv("QT_XCB_GL_INTEGRATION", "xcb_egl"); // use xcomposite-glx if no EGL
    if (!qEnvironmentVariableIsSet("QT_WAYLAND_DISABLE_WINDOWDECORATION"))
        qputenv("QT_WAYLAND_DISABLE_WINDOWDECORATION", "1");
    if (!qEnvironmentVariableIsSet("QT_LABS_CONTROLS_STYLE"))
        qputenv("QT_LABS_CONTROLS_STYLE", "Universal");
    QGuiApplication app(argc, argv);
    //QCoreApplication::setApplicationName("grefsen"); // defaults to name of the executable
    QCoreApplication::setApplicationVersion("0.1");
//    app.setAttribute(Qt::AA_DisableHighDpiScaling); // better use the env variable... but that's not enough on eglfs
    grefsonExecutablePath = app.applicationFilePath().toLocal8Bit();
    grefsonPID = QCoreApplication::applicationPid();

    QCommandLineParser parser;
    parser.setApplicationDescription("Grefsen Qt/Wayland compositor");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption respawnOption(QStringList() << "r" << "respawn",
            QCoreApplication::translate("main", "respawn grefsen after a crash"));
    parser.addOption(respawnOption);

    QCommandLineOption logFileOption(QStringList() << "l" << "log",
            QCoreApplication::translate("main", "redirect all debug/warning/error output to a log file"),
            QCoreApplication::translate("main", "file path"));
    parser.addOption(logFileOption);

    QCommandLineOption configDirOption(QStringList() << "c" << "config",
            QCoreApplication::translate("main", "load config files from the given directory (default is ~/.config/grefsen)"),
            QCoreApplication::translate("main", "directory path"));
    parser.addOption(configDirOption);

    parser.process(app);
    if (parser.isSet(respawnOption))
        setupSignalHandler();
    if (parser.isSet(configDirOption))
        grefsenConfigDirPath = parser.value(configDirOption);
    if (parser.isSet(logFileOption)) {
        logFilePath = parser.value(logFileOption);
        qInstallMessageHandler(qtMsgLog);
    }

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
