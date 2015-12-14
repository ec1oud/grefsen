
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
// Copyright (c) 2014, Sami Kananoja <sami.kananoja@jolla.com>

#include <QFile>

#include "launcherwatchermodel.h"
#include "launcheritem.h"

LauncherWatcherModel::LauncherWatcherModel(QObject *parent) :
    QObjectListModel(parent),
    _fileSystemWatcher()
{
    connect(&_fileSystemWatcher, SIGNAL(fileChanged(QString)), this, SLOT(monitoredFileChanged(QString)));
}

LauncherWatcherModel::~LauncherWatcherModel()
{
}

void LauncherWatcherModel::monitoredFileChanged(const QString &changedPath)
{
    if (!QFile(changedPath).exists()) {
        foreach (LauncherItem *item, *getList<LauncherItem>()) {
            if (item->filePath() == changedPath) {
                removeItem(item);
                emit filePathsChanged();
                break;
            }
        }
    }
}

QStringList LauncherWatcherModel::filePaths()
{
    QStringList paths;
    foreach (LauncherItem *item, *getList<LauncherItem>()) {
        paths.append(item->filePath());
    }
    return paths;
}

void LauncherWatcherModel::setFilePaths(QStringList paths)
{
    const QStringList oldPaths = filePaths();

    int insertIndex = 0;
    for (int i = 0; i < paths.count(); ++i) {
        const QString path = paths.at(i);
        bool duplicate = false;
        for (int j = 0; j < i; ++j) {
            if ((duplicate = paths.at(j) == path)) {
                break;
            }
        }
        if (duplicate) {
            continue;
        }

        int removeIndex = -1;
        for (int j = insertIndex; j < itemCount(); ++j) {
            if (static_cast<LauncherItem *>(get(j))->filePath() == path) {
                removeIndex = j;
                break;
            }
        }

        if (removeIndex > insertIndex) {
            move(removeIndex, insertIndex);
        } else if (removeIndex != insertIndex) {
            LauncherItem *item = new LauncherItem(path, this);
            if (item->isValid()) {
                insertItem(insertIndex, item);
                _fileSystemWatcher.addPath(path);
            } else {
                delete item;
                continue;
            }
        }
        ++insertIndex;
    }

    while (insertIndex < itemCount()) {
        LauncherItem *item = static_cast<LauncherItem *>(get(insertIndex));
        _fileSystemWatcher.removePath(item->filePath());
        removeItem(insertIndex);
        delete item;
    }

    if (filePaths() != oldPaths) {
        emit filePathsChanged();
    }
}
