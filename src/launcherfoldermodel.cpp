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

#include "launcheritem.h"
#include "launcherfoldermodel.h"
#include "launchermodel.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryFile>
#include <QStandardPaths>
#include <QDir>
#include <QStack>
#include <mdesktopentry.h>
#include <glib.h>
#include <QDebug>

static const int FOLDER_MODEL_SAVE_TIMER_MS = 1000;
static const QString CONFIG_FOLDER_SUBDIRECTORY("/lipstick/");
static const QString CONFIG_MENU_FILENAME("applications.menu");
static const QString DEFAULT_ICON_ID("icon-launcher-folder-01");

static QString configDir()
{
    return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + CONFIG_FOLDER_SUBDIRECTORY;
}

static QString absoluteConfigPath(const QString &fileName)
{
    return configDir() + fileName;
}

// This is modeled after the freedesktop.org menu files http://standards.freedesktop.org/menu-spec/latest/
// but handles only the basic elements, i.e. no merging, filtering, layout, etc. is supported.

LauncherFolderItem::LauncherFolderItem(QObject *parent)
    : QObjectListModel(parent), mIconId(DEFAULT_ICON_ID)
{
    connect(this, SIGNAL(itemRemoved(QObject*)), this, SLOT(handleRemoved(QObject*)));
    connect(this, SIGNAL(itemAdded(QObject*)), this, SLOT(handleAdded(QObject*)));
    connect(this, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)), this, SIGNAL(saveNeeded()));
}

LauncherModel::ItemType LauncherFolderItem::type() const
{
    return LauncherModel::Folder;
}

const QString &LauncherFolderItem::title() const
{
    return mTitle;
}

void LauncherFolderItem::setTitle(const QString &title)
{
    if (title == mTitle)
        return;

    mTitle = title;
    emit titleChanged();
    emit saveNeeded();
}

const QString &LauncherFolderItem::iconId() const
{
    return mIconId;
}

void LauncherFolderItem::setIconId(const QString &icon)
{
    if (icon == mIconId)
        return;

    mIconId = icon;
    saveDirectoryFile();
    emit iconIdChanged();
}

bool LauncherFolderItem::isUpdating() const
{
    LauncherFolderItem *me = const_cast<LauncherFolderItem*>(this);
    for (int i = 0; i < rowCount(); ++i) {
        const LauncherItem *launcherItem = qobject_cast<const LauncherItem*>(me->get(i));
        if (launcherItem && launcherItem->isUpdating())
            return true;
    }

    return false;
}

int LauncherFolderItem::updatingProgress() const
{
    int updatingCount = 0;
    int updatingTotal = 0;
    LauncherFolderItem *me = const_cast<LauncherFolderItem*>(this);
    for (int i = 0; i < rowCount(); ++i) {
        const LauncherItem *launcherItem = qobject_cast<const LauncherItem*>(me->get(i));
        if (launcherItem && launcherItem->isUpdating()) {
            int progress = launcherItem->updatingProgress();
            if (progress < 0 || progress > 100)
                return progress;
            ++updatingCount;
            updatingTotal += progress;
        }
    }

    return updatingCount ? updatingTotal / updatingCount : 0;
}

LauncherFolderItem *LauncherFolderItem::parentFolder() const
{
    return mParentFolder;
}

void LauncherFolderItem::setParentFolder(LauncherFolderItem *parent)
{
    if (parent == mParentFolder)
        return;

    mParentFolder = parent;
    emit parentFolderChanged();
}

// Creates a folder and moves the item at that index into the folder
LauncherFolderItem *LauncherFolderItem::createFolder(int index, const QString &name)
{
    if (index < 0 || index > rowCount())
        return 0;

    LauncherFolderItem *folder = new LauncherFolderItem(this);
    folder->setTitle(name);
    folder->setParentFolder(this);
    QObject *item = get(index);
    insertItem(index, folder);
    if (item) {
        removeItem(item);
        folder->addItem(item);
    }

    emit saveNeeded();

    return folder;
}

void LauncherFolderItem::destroyFolder()
{
    if (itemCount() != 0)
        qWarning() << "Removing a folder that is not empty.";
    if (mParentFolder)
        mParentFolder->removeItem(this);
    if (!mDirectoryFile.isEmpty()) {
        QFile file(mDirectoryFile);
        file.remove();
    }

    emit saveNeeded();

    deleteLater();
}

LauncherFolderItem *LauncherFolderItem::findContainer(QObject *item)
{
    LauncherFolderItem *me = const_cast<LauncherFolderItem*>(this);
    for (int i = 0; i < rowCount(); ++i) {
        QObject *obj = me->get(i);
        if (obj == item) {
            return this;
        } else if (LauncherFolderItem *subFolder = qobject_cast<LauncherFolderItem*>(obj)) {
            LauncherFolderItem *folder = subFolder->findContainer(item);
            if (folder)
                return folder;
        }
    }

    return 0;
}

QString LauncherFolderItem::directoryFile() const
{
    return mDirectoryFile;
}

void LauncherFolderItem::loadDirectoryFile(const QString &filename)
{
    mDirectoryFile = filename;
    if (!mDirectoryFile.startsWith('/')) {
        mDirectoryFile = absoluteConfigPath(mDirectoryFile);
    }

    GKeyFile *keyfile = g_key_file_new();
    GError *err = NULL;

    if (g_key_file_load_from_file(keyfile, mDirectoryFile.toLatin1(), G_KEY_FILE_NONE, &err)) {
        mIconId = QString::fromLatin1(g_key_file_get_string(keyfile, "Desktop Entry", "Icon", &err));
        emit iconIdChanged();
    }

    if (err != NULL) {
        qWarning() << "Failed to load .directory file" << err->message;
        g_error_free(err);
    }

    g_key_file_free(keyfile);
}

void LauncherFolderItem::saveDirectoryFile()
{
    QScopedPointer<QFile> dirFile;
    if (mDirectoryFile.isEmpty()) {
        QTemporaryFile *tempFile = new QTemporaryFile(absoluteConfigPath("FolderXXXXXX.directory"));
        dirFile.reset(tempFile);
        tempFile->open();
        tempFile->setAutoRemove(false);
        mDirectoryFile = tempFile->fileName();
        emit directoryFileChanged();
        emit saveNeeded();
    } else {
        dirFile.reset(new QFile(mDirectoryFile));
        dirFile.data()->open(QIODevice::WriteOnly);
    }

    if (!dirFile.data()->isOpen()) {
        qWarning() << "Cannot open" << mDirectoryFile;
        return;
    }

    GKeyFile *keyfile = g_key_file_new();
    GError *err = NULL;

    g_key_file_load_from_file(keyfile, mDirectoryFile.toLatin1(), G_KEY_FILE_NONE, &err);
    g_key_file_set_string(keyfile, "Desktop Entry", "Icon", mIconId.toLatin1());

    gchar *data = g_key_file_to_data(keyfile, NULL, &err);
    dirFile.data()->write(data);
    dirFile.data()->close();
    g_free(data);

    g_key_file_free(keyfile);
}

void LauncherFolderItem::clear()
{
    for (int i = 0; i < rowCount(); ++i) {
        QObject *item = get(i);
        LauncherItem *launcherItem = qobject_cast<LauncherItem*>(item);
        LauncherFolderItem *folder = qobject_cast<LauncherFolderItem*>(item);

        if (launcherItem) {
            disconnect(item, SIGNAL(isTemporaryChanged()), this, SIGNAL(saveNeeded()));
        } else if (folder) {
            disconnect(item, SIGNAL(saveNeeded()), this, SIGNAL(saveNeeded()));
        }

        if (launcherItem || folder) {
            disconnect(item, SIGNAL(isUpdatingChanged()), this, SIGNAL(isUpdatingChanged()));
            disconnect(item, SIGNAL(updatingProgressChanged()), this, SIGNAL(updatingProgressChanged()));
        }
        if (folder) {
            folder->clear();
            folder->deleteLater();
        }
    }
    reset();
}

void LauncherFolderItem::handleAdded(QObject *item)
{
    const LauncherItem *launcherItem = qobject_cast<const LauncherItem*>(item);
    const LauncherFolderItem *folder = qobject_cast<const LauncherFolderItem*>(item);

    if (launcherItem) {
        if (launcherItem->isUpdating()) {
            emit isUpdatingChanged();
            emit updatingProgressChanged();
        }
        connect(item, SIGNAL(isTemporaryChanged()), this, SIGNAL(saveNeeded()));
    } else if (folder) {
        if (folder->isUpdating()) {
            emit isUpdatingChanged();
            emit updatingProgressChanged();
        }
        connect(item, SIGNAL(saveNeeded()), this, SIGNAL(saveNeeded()));
    }

    if (launcherItem || folder) {
        connect(item, SIGNAL(isUpdatingChanged()), this, SIGNAL(isUpdatingChanged()));
        connect(item, SIGNAL(updatingProgressChanged()), this, SIGNAL(updatingProgressChanged()));
    }

    emit saveNeeded();
}

void LauncherFolderItem::handleRemoved(QObject *item)
{
    const LauncherItem *launcherItem = qobject_cast<const LauncherItem*>(item);
    const LauncherFolderItem *folder = qobject_cast<const LauncherFolderItem*>(item);

    if (launcherItem) {
        if (launcherItem->isUpdating()) {
            emit isUpdatingChanged();
            emit updatingProgressChanged();
        }
        disconnect(item, SIGNAL(isTemporaryChanged()), this, SIGNAL(saveNeeded()));
    } else if (folder) {
        if (folder->isUpdating()) {
            emit isUpdatingChanged();
            emit updatingProgressChanged();
        }
        disconnect(item, SIGNAL(saveNeeded()), this, SIGNAL(saveNeeded()));
    }

    if (launcherItem || folder) {
        disconnect(item, SIGNAL(isUpdatingChanged()), this, SIGNAL(isUpdatingChanged()));
        disconnect(item, SIGNAL(updatingProgressChanged()), this, SIGNAL(updatingProgressChanged()));
    }

    emit saveNeeded();
}

class DeferredLauncherModel : public LauncherModel
{
public:
    explicit DeferredLauncherModel(QObject *parent = 0)
        : LauncherModel(DeferInitialization, parent)
    {
    }

    using LauncherModel::initialize;
};

//============

LauncherFolderModel::LauncherFolderModel(QObject *parent)
    : LauncherFolderItem(parent)
    , mLauncherModel(new DeferredLauncherModel(this))
    , mLoading(false)
    , mInitialized(false)
{
    connect(mLauncherModel, &LauncherModel::directoriesChanged, this, &LauncherFolderModel::directoriesChanged);
    connect(mLauncherModel, &LauncherModel::iconDirectoriesChanged, this, &LauncherFolderModel::iconDirectoriesChanged);
    connect(mLauncherModel, &LauncherModel::categoriesChanged, this, &LauncherFolderModel::categoriesChanged);

    initialize();
}

LauncherFolderModel::LauncherFolderModel(InitializationMode, QObject *parent)
    : LauncherFolderItem(parent)
    , mLauncherModel(new DeferredLauncherModel(this))
    , mLoading(false)
    , mInitialized(false)
{
    connect(mLauncherModel, &LauncherModel::directoriesChanged, this, &LauncherFolderModel::directoriesChanged);
    connect(mLauncherModel, &LauncherModel::iconDirectoriesChanged, this, &LauncherFolderModel::iconDirectoriesChanged);
    connect(mLauncherModel, &LauncherModel::categoriesChanged, this, &LauncherFolderModel::categoriesChanged);
}

void LauncherFolderModel::initialize()
{
    if (mInitialized)
        return;
    mInitialized = true;

    mLauncherModel->initialize();

    mSaveTimer.setSingleShot(true);
    connect(mLauncherModel, SIGNAL(itemRemoved(QObject*)),
            this, SLOT(appRemoved(QObject*)));
    connect(mLauncherModel, SIGNAL(itemAdded(QObject*)),
            this, SLOT(appAdded(QObject*)));
    connect(mLauncherModel, (void (LauncherModel::*)(LauncherItem *))&LauncherModel::notifyLaunching,
            this, &LauncherFolderModel::notifyLaunching);
    connect(&mSaveTimer, SIGNAL(timeout()), this, SLOT(save()));

    QDir config;
    config.mkpath(configDir());

    load();

    connect(this, SIGNAL(saveNeeded()), this, SLOT(scheduleSave()));
}

QString LauncherFolderModel::scope() const
{
    return mLauncherModel->scope();
}

void LauncherFolderModel::setScope(const QString &scope)
{
    if (mLauncherModel->scope() != scope) {
        mLauncherModel->setScope(scope);
        emit scopeChanged();

        if (mInitialized) {
            load();
        }
    }
}

QStringList LauncherFolderModel::directories() const
{
    return mLauncherModel->directories();
}

void LauncherFolderModel::setDirectories(QStringList dirs)
{
    mLauncherModel->setDirectories(dirs);
}

QStringList LauncherFolderModel::iconDirectories() const
{
    return mLauncherModel->iconDirectories();
}

void LauncherFolderModel::setIconDirectories(QStringList dirs)
{
    mLauncherModel->setIconDirectories(dirs);
}

QStringList LauncherFolderModel::categories() const
{
    return mLauncherModel->categories();
}

void LauncherFolderModel::setCategories(const QStringList &categories)
{
    mLauncherModel->setCategories(categories);
}

// Move item to folder at index. If index < 0 the item will be appended.
bool LauncherFolderModel::moveToFolder(QObject *item, LauncherFolderItem *folder, int index)
{
    if (!item || !folder)
        return false;

    LauncherFolderItem *source = findContainer(item);
    if (!source)
        return false;

    source->removeItem(item);

    if (index >= 0)
        folder->insertItem(index, item);
    else
        folder->addItem(item);
    if (LauncherFolderItem *f = qobject_cast<LauncherFolderItem*>(item))
        f->setParentFolder(folder);

    scheduleSave();

    return true;
}

// An app removed from system
void LauncherFolderModel::appRemoved(QObject *item)
{
    LauncherFolderItem *folder = findContainer(item);
    if (folder) {
        folder->removeItem(item);
        scheduleSave();
    }
}

// An app added to system
void LauncherFolderModel::appAdded(QObject *item)
{
    addItem(item);
    scheduleSave();
}

void LauncherFolderModel::import()
{
    for (int i = 0; i < mLauncherModel->rowCount(); ++i) {
        addItem(mLauncherModel->get(i));
    }
}

void LauncherFolderModel::scheduleSave()
{
    if (!mLoading)
        mSaveTimer.start(FOLDER_MODEL_SAVE_TIMER_MS);
}

QString LauncherFolderModel::configFile()
{
    return configDir() + CONFIG_MENU_FILENAME;
}

static QString configurationFileForScope(const QString &scope)
{
    return !scope.isEmpty()
            ? configDir() + scope + QStringLiteral(".menu")
            : LauncherFolderModel::configFile();
}

void LauncherFolderModel::save()
{
    mSaveTimer.stop();
    QFile file(configurationFileForScope(mLauncherModel->scope()));
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to save apps menu" << configFile();
        return;
    }

    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    saveFolder(xml, this);
    xml.writeEndDocument();
}

void LauncherFolderModel::saveFolder(QXmlStreamWriter &xml, LauncherFolderItem *folder)
{
    xml.writeStartElement("Menu");
    xml.writeTextElement("Name", folder->title());
    if (!folder->directoryFile().isEmpty())
        xml.writeTextElement("Directory", folder->directoryFile());

    for (int i = 0; i < folder->rowCount(); ++i) {
        LauncherItem *item = qobject_cast<LauncherItem*>(folder->get(i));
        if (item) {
            if (!item->isTemporary())
                xml.writeTextElement("Filename", item->filename());
        } else if (LauncherFolderItem *subFolder = qobject_cast<LauncherFolderItem*>(folder->get(i))) {
            saveFolder(xml, subFolder);
        }
    }
    xml.writeEndElement();
}

void LauncherFolderModel::load()
{
    mLoading = true;
    clear();

    QFile file(configurationFileForScope(mLauncherModel->scope()));
    if (!file.open(QIODevice::ReadOnly)) {
        // We haven't saved a folder model yet - import all apps.
        import();
        mLoading = false;
        return;
    }

    QVector<bool> loadedItems(mLauncherModel->itemCount());
    loadedItems.fill(false);
    QStack<LauncherFolderItem*> menus;
    QString textData;
    int loadedCount = 0;

    QXmlStreamReader xml(&file);
    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement()) {
            if (xml.name() == QLatin1String("Menu")) {
                LauncherFolderItem *folder = 0;
                if (menus.isEmpty())
                    folder = this;
                else
                    folder = new LauncherFolderItem(this);
                if (!menus.isEmpty()) {
                    folder->setParentFolder(menus.top());
                    menus.top()->addItem(folder);
                }
                menus.push(folder);
            }
        } else if (xml.isEndElement()) {
            if (xml.name() == QLatin1String("Menu")) {
                menus.pop();
            } else if (xml.name() == QLatin1String("Name")) {
                if (!menus.isEmpty()) {
                    menus.top()->setTitle(textData);
                }
            } else if (xml.name() == QLatin1String("Directory")) {
                if (!menus.isEmpty()) {
                    LauncherFolderItem *folder = menus.top();
                    folder->loadDirectoryFile(textData);
                }
            } else if (xml.name() == QLatin1String("Filename")) {
                if (!menus.isEmpty()) {
                    int idx = mLauncherModel->indexInModel(textData);
                    if (idx >= 0) {
                        loadedItems[idx] = true;
                        LauncherItem *item = qobject_cast<LauncherItem*>(mLauncherModel->get(idx));
                        if (item) {
                            loadedCount++;
                            LauncherFolderItem *folder = menus.top();
                            folder->addItem(item);
                        }
                    }
                }
            }
            textData.clear();
        } else if (xml.isCharacters()) {
            textData = xml.text().toString();
        }
    }

    for (int i = 0; i < loadedItems.count(); ++i) {
        if (!loadedItems.at(i)) {
            addItem(mLauncherModel->get(i));
        }
    }

    mLoading = false;
}
