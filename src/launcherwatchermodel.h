
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

#ifndef LAUNCHERWATCHERMODEL_H
#define LAUNCHERWATCHERMODEL_H

#include <QObject>
#include <QStringList>
#include <QFileSystemWatcher>

#include "qobjectlistmodel.h"
#include "lipstickglobal.h"

class LIPSTICK_EXPORT LauncherWatcherModel : public QObjectListModel
{
    Q_OBJECT
    Q_DISABLE_COPY(LauncherWatcherModel)

    Q_PROPERTY(QStringList filePaths READ filePaths WRITE setFilePaths NOTIFY filePathsChanged)

    QFileSystemWatcher _fileSystemWatcher;

private slots:
    void monitoredFileChanged(const QString &changedPath);

public:
    explicit LauncherWatcherModel(QObject *parent = 0);
    virtual ~LauncherWatcherModel();

    QStringList filePaths();
    void setFilePaths(QStringList);

signals:
    void filePathsChanged();
};

#endif // LAUNCHERWATCHERMODEL_H
