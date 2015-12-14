// This file is part of lipstick, a QML desktop library
//
// Copyright (c) 2014 Jolla Ltd.
// Contact: Martin Jones <martin.jones@jolla.com>
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

#ifndef LAUNCHERFOLDERMODEL_H
#define LAUNCHERFOLDERMODEL_H

#include <QObject>
#include <QStringList>
#include <QPointer>
#include <QSharedPointer>
#include <QTimer>
#include "qobjectlistmodel.h"
#include "launchermodel.h"
#include "lipstickglobal.h"

class LauncherModel;
class QXmlStreamWriter;
class MDesktopEntry;
class LauncherItem;

class LIPSTICK_EXPORT LauncherFolderItem : public QObjectListModel
{
    Q_OBJECT
    Q_PROPERTY(LauncherModel::ItemType type READ type CONSTANT)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString iconId READ iconId WRITE setIconId NOTIFY iconIdChanged)
    Q_PROPERTY(bool isUpdating READ isUpdating NOTIFY isUpdatingChanged)
    Q_PROPERTY(int updatingProgress READ updatingProgress NOTIFY updatingProgressChanged)
    Q_PROPERTY(LauncherFolderItem *parentFolder READ parentFolder NOTIFY parentFolderChanged)

public:
    LauncherFolderItem(QObject *parent = 0);

    LauncherModel::ItemType type() const;

    const QString &title() const;
    void setTitle(const QString &name);

    const QString &iconId() const;
    void setIconId(const QString &icon);

    bool isUpdating() const;
    int updatingProgress() const;

    LauncherFolderItem *parentFolder() const;
    void setParentFolder(LauncherFolderItem *parent);

    Q_INVOKABLE LauncherFolderItem *createFolder(int index, const QString &name);
    Q_INVOKABLE void destroyFolder();

    LauncherFolderItem *findContainer(QObject *item);

    QString directoryFile() const;
    void loadDirectoryFile(const QString &filename);
    void saveDirectoryFile();
    void clear();

signals:
    void titleChanged();
    void iconIdChanged();
    void isUpdatingChanged();
    void updatingProgressChanged();
    void parentFolderChanged();
    void directoryFileChanged();
    void saveNeeded();

private slots:
    void handleAdded(QObject*);
    void handleRemoved(QObject*);

private:
    QString mTitle;
    QString mIconId;
    QString mDirectoryFile;
    QSharedPointer<MDesktopEntry> mDesktopEntry;
    QPointer<LauncherFolderItem> mParentFolder;
};

class DeferredLauncherModel;
class LIPSTICK_EXPORT LauncherFolderModel : public LauncherFolderItem
{
    Q_OBJECT
    Q_PROPERTY(QString scope READ scope WRITE setScope NOTIFY scopeChanged)
    Q_PROPERTY(QStringList directories READ directories WRITE setDirectories NOTIFY directoriesChanged)
    Q_PROPERTY(QStringList iconDirectories READ iconDirectories WRITE setIconDirectories NOTIFY iconDirectoriesChanged)
    Q_PROPERTY(QStringList categories READ categories WRITE setCategories NOTIFY categoriesChanged)
public:
    LauncherFolderModel(QObject *parent = 0);

    QString scope() const;
    void setScope(const QString &scope);

    QStringList directories() const;
    void setDirectories(QStringList);

    QStringList iconDirectories() const;
    void setIconDirectories(QStringList);

    QStringList categories() const;
    void setCategories(const QStringList &categories);

    Q_INVOKABLE bool moveToFolder(QObject *item, LauncherFolderItem *folder, int index = -1);

    void import();

    static QString configFile();

public slots:
    void load();
    void save();

signals:
    void scopeChanged();
    void directoriesChanged();
    void iconDirectoriesChanged();
    void categoriesChanged();
    void notifyLaunching(LauncherItem *item);

protected:
    enum InitializationMode {
        DeferInitialization
    };

    explicit LauncherFolderModel(InitializationMode, QObject *parent = 0);

    void initialize();

private slots:
    void scheduleSave();
    void appRemoved(QObject *item);
    void appAdded(QObject *item);

private:
    void saveFolder(QXmlStreamWriter &xml, LauncherFolderItem *folder);

    DeferredLauncherModel *mLauncherModel;
    QTimer mSaveTimer;
    bool mLoading;
    bool mInitialized;
};

#endif
