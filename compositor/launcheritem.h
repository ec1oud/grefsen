
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

#ifndef LAUNCHERITEM_H
#define LAUNCHERITEM_H

#include <QObject>
#include <QStringList>
#include <QSharedPointer>
#include <QBasicTimer>

// Define this if you'd like to see debug messages from the launcher
#ifdef DEBUG_LAUNCHER
#include <QDebug>
#define LAUNCHER_DEBUG(things) qDebug() << Q_FUNC_INFO << things
#else
#define LAUNCHER_DEBUG(things)
#endif

#include "launchermodel.h"
#include "lipstickglobal.h"

class MDesktopEntry;

class LIPSTICK_EXPORT LauncherItem : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(LauncherItem)

    Q_PROPERTY(LauncherModel::ItemType type READ type CONSTANT)
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath NOTIFY itemChanged)
    Q_PROPERTY(QString fileID READ fileID NOTIFY itemChanged)
    Q_PROPERTY(QString exec READ exec NOTIFY itemChanged)
    Q_PROPERTY(QString title READ title NOTIFY itemChanged)
    Q_PROPERTY(QString entryType READ entryType NOTIFY itemChanged)
    Q_PROPERTY(QString iconId READ iconId NOTIFY itemChanged)
    Q_PROPERTY(QStringList desktopCategories READ desktopCategories NOTIFY itemChanged)
    Q_PROPERTY(QString titleUnlocalized READ titleUnlocalized NOTIFY itemChanged)
    Q_PROPERTY(bool shouldDisplay READ shouldDisplay NOTIFY itemChanged)
    Q_PROPERTY(bool isValid READ isValid NOTIFY itemChanged)
    Q_PROPERTY(bool isLaunching READ isLaunching WRITE setIsLaunching NOTIFY isLaunchingChanged)
    Q_PROPERTY(bool isUpdating READ isUpdating WRITE setIsUpdating NOTIFY isUpdatingChanged)
    Q_PROPERTY(bool isTemporary READ isTemporary WRITE setIsTemporary NOTIFY isTemporaryChanged)
    Q_PROPERTY(QString packageName READ packageName WRITE setPackageName NOTIFY packageNameChanged)
    Q_PROPERTY(int updatingProgress READ updatingProgress WRITE setUpdatingProgress NOTIFY updatingProgressChanged)

    QSharedPointer<MDesktopEntry> _desktopEntry;
    QBasicTimer _launchingTimeout;
    bool _isLaunching;
    bool _isUpdating;
    bool _isTemporary;
    QString _packageName;
    int _updatingProgress;
    QString _customTitle;
    QString _customIconFilename;
    int _serial;

public slots:
    void setIsLaunching(bool isLaunching = false);

public:
    explicit LauncherItem(const QString &filePath = QString(), QObject *parent = 0);
    explicit LauncherItem(const QString &packageName, const QString &label,
            const QString &iconPath, const QString &desktopFile, QObject *parent);
    virtual ~LauncherItem();

    LauncherModel::ItemType type() const;
    void setFilePath(const QString &filePath);
    QString filePath() const;
    QString fileID() const;
    QString filename() const;
    QString exec() const;
    QString title() const;
    QString entryType() const;
    QString iconId() const;
    QStringList desktopCategories() const;
    QString titleUnlocalized() const;
    bool shouldDisplay() const;
    bool isValid() const;
    bool isLaunching() const;
    bool isStillValid();

    QString getOriginalIconId() const;
    void setIconFilename(const QString &path);
    QString iconFilename() const;

    Q_INVOKABLE void launchApplication();

    bool isUpdating() const { return _isUpdating; }
    void setIsUpdating(bool isUpdating);

    bool isTemporary() const { return _isTemporary; }
    void setIsTemporary(bool isTemporary);

    QString packageName() const { return _packageName; }
    void setPackageName(QString packageName);

    int updatingProgress() const { return _updatingProgress; }
    void setUpdatingProgress(int updatingProgress);

    void setCustomTitle(QString customTitle);

    Q_INVOKABLE QString readValue(const QString &key) const;

signals:
    void itemChanged();
    void isLaunchingChanged();
    void isUpdatingChanged();
    void isTemporaryChanged();
    void packageNameChanged();
    void updatingProgressChanged();

protected:
    void timerEvent(QTimerEvent *event);
};

#endif // LAUNCHERITEM_H
