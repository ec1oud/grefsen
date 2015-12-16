
// This file is part of lipstick, a QML desktop library
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation
// and appearing in the file LICENSE.LGPL included in the packaging
// of this file.
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
//
// Copyright (c) 2011, Robin Burchell
// Copyright (c) 2012, Timur Kristóf <venemo@fedoraproject.org>

#include <QDebug>
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QSettings>
#include <QTimerEvent>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <mdesktopentry.h>

#ifdef HAVE_CONTENTACTION
#include <contentaction.h>
#endif

#include "launcheritem.h"
#include "launchermodel.h"

LauncherItem::LauncherItem(const QString &filePath, QObject *parent)
    : QObject(parent)
    , _isLaunching(false)
    , _isUpdating(false)
    , _isTemporary(false)
    , _packageName("")
    , _updatingProgress(-1)
    , _customTitle("")
    , _customIconFilename("")
    , _serial(0)
{
    if (!filePath.isEmpty()) {
        setFilePath(filePath);
    }

    // TODO: match the PID of the window thumbnails with the launcher processes
    // Launching animation will be disabled if the window of the launched app shows up
}

LauncherItem::LauncherItem(const QString &packageName, const QString &label,
        const QString &iconPath, const QString &desktopFile, QObject *parent)
    : QObject(parent)
    , _isLaunching(false)
    , _isUpdating(false)
    , _isTemporary(false)
    , _packageName(packageName)
    , _updatingProgress(-1)
    , _customTitle(label)
    , _customIconFilename(iconPath)
    , _serial(0)
{
    if (!desktopFile.isEmpty()) {
        setFilePath(desktopFile);
    }
}

LauncherItem::~LauncherItem()
{
}

LauncherModel::ItemType LauncherItem::type() const
{
    return LauncherModel::Application;
}

void LauncherItem::setFilePath(const QString &filePath)
{
    if (!filePath.isEmpty() && QFile(filePath).exists()) {
        _desktopEntry = QSharedPointer<MDesktopEntry>(new MDesktopEntry(filePath));
    } else {
        _desktopEntry.clear();
    }

    emit this->itemChanged();
}

QString LauncherItem::filePath() const
{
    return !_desktopEntry.isNull() ? _desktopEntry->fileName() : QString();
}

QString LauncherItem::fileID() const
{
    if (_desktopEntry.isNull()) {
        return QString();
    }

    // Retrieve the file ID according to
    // http://standards.freedesktop.org/desktop-entry-spec/latest/ape.html
    QRegularExpression re(".*applications/(.*.desktop)");
    QRegularExpressionMatch match = re.match(_desktopEntry->fileName());
    if (!match.hasMatch()) {
        return filename();
    }

    QString id = match.captured(1);
    id.replace('/', '-');
    return id;
}

QString LauncherItem::filename() const
{
    QString filename = filePath();
    int sep = filename.lastIndexOf('/');
    if (sep == -1)
        return QString();

    return filename.mid(sep+1);
}

QString LauncherItem::exec() const
{
    return !_desktopEntry.isNull() ? _desktopEntry->exec() : QString();
}

QString LauncherItem::title() const
{
    if (_isTemporary) {
        return _customTitle;
    }

    return !_desktopEntry.isNull() ? _desktopEntry->name() : QString();
}

QString LauncherItem::entryType() const
{
    return !_desktopEntry.isNull() ? _desktopEntry->type() : QString();
}

QString LauncherItem::iconId() const
{
    if (!_customIconFilename.isEmpty()) {
        return QString("%1#serial=%2").arg(_customIconFilename).arg(_serial);
    }

    return getOriginalIconId();
}

QStringList LauncherItem::desktopCategories() const
{
    return !_desktopEntry.isNull() ? _desktopEntry->categories() : QStringList();
}

QString LauncherItem::titleUnlocalized() const
{
    if (_isTemporary) {
        return _customTitle;
    }

    return !_desktopEntry.isNull() ? _desktopEntry->nameUnlocalized() : QString();
}

bool LauncherItem::shouldDisplay() const
{
    return !_desktopEntry.isNull() ? !_desktopEntry->noDisplay() : _isTemporary;
}

bool LauncherItem::isValid() const
{
    return !_desktopEntry.isNull() ? _desktopEntry->isValid() : _isTemporary;
}

bool LauncherItem::isLaunching() const
{
    return _isLaunching;
}

void LauncherItem::setIsLaunching(bool isLaunching)
{
    if (isLaunching) {
        // This is a failsafe to allow launching again after 5 seconds in case the application crashes on startup and no window is ever created
        _launchingTimeout.start(5000, this);
    } else {
        _launchingTimeout.stop();
    }
    if (_isLaunching != isLaunching) {
        _isLaunching = isLaunching;
        emit this->isLaunchingChanged();
    }
}

void LauncherItem::setIsUpdating(bool isUpdating)
{
    if (_isUpdating != isUpdating) {
        _isUpdating = isUpdating;
        emit isUpdatingChanged();
    }
}

void LauncherItem::setIsTemporary(bool isTemporary)
{
    if (_isTemporary != isTemporary) {
        _isTemporary = isTemporary;
        emit isTemporaryChanged();
    }
}

void LauncherItem::launchApplication()
{
    if (_isUpdating) {
        LauncherModel *model = static_cast<LauncherModel *>(parent());
        model->requestLaunch(_packageName);
        return;
    }

    if (_desktopEntry.isNull())
        return;

#if defined(HAVE_CONTENTACTION)
    LAUNCHER_DEBUG("launching content action for" << _desktopEntry->name());
    ContentAction::Action action = ContentAction::Action::launcherAction(_desktopEntry, QStringList());
    action.trigger();
#else
    LAUNCHER_DEBUG("launching exec line for" << _desktopEntry->name());

    // Get the command text from the desktop entry
    QString commandText = _desktopEntry->exec();

    // Take care of the freedesktop standards things

    commandText.replace(QRegExp("%k"), filePath());
    commandText.replace(QRegExp("%c"), _desktopEntry->name());
    commandText.remove(QRegExp("%[fFuU]"));

    if (!_desktopEntry->icon().isEmpty())
        commandText.replace(QRegExp("%i"), QString("--icon ") + _desktopEntry->icon());

    // DETAILS: http://standards.freedesktop.org/desktop-entry-spec/latest/index.html
    // DETAILS: http://standards.freedesktop.org/desktop-entry-spec/latest/ar01s06.html

    // Launch the application
    QProcess::startDetached(commandText);
#endif

    setIsLaunching(true);
}

bool LauncherItem::isStillValid()
{
    if (_isTemporary) {
        return true;
    }

    // Force a reload of _desktopEntry
    setFilePath(filePath());
    return isValid();
}

QString LauncherItem::getOriginalIconId() const
{
    return !_desktopEntry.isNull() ? _desktopEntry->icon() : QString();
}

void LauncherItem::setIconFilename(const QString &path)
{
    _customIconFilename = path;
    if (!path.isEmpty()) {
        _serial++;
    }
    emit itemChanged();
}

QString LauncherItem::iconFilename() const
{
    return _customIconFilename;
}

void LauncherItem::setPackageName(QString packageName)
{
    if (_packageName != packageName) {
        _packageName = packageName;
        emit packageNameChanged();
    }
}

void LauncherItem::setUpdatingProgress(int updatingProgress)
{
    if (_updatingProgress != updatingProgress) {
        _updatingProgress = updatingProgress;
        emit updatingProgressChanged();
    }
}

void LauncherItem::setCustomTitle(QString customTitle)
{
    if (_customTitle != customTitle) {
        _customTitle = customTitle;
        emit itemChanged();
    }
}

QString LauncherItem::readValue(const QString &key) const
{
    if (_desktopEntry.isNull())
        return QString();

    return _desktopEntry->value("Desktop Entry", key);
}

void LauncherItem::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == _launchingTimeout.timerId()) {
        setIsLaunching(false);
    } else {
        QObject::timerEvent(event);
    }
}
