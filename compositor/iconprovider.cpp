#include "iconprovider.h"
#include <qt5xdg/XdgIcon>

#include <QDir>
#include <QDirIterator>

IconProvider::IconProvider()
  : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
    XdgIcon::setThemeName(QStringLiteral("oxygen"));
    qDebug() << "theme is" << XdgIcon::themeName() << "default icon" << XdgIcon::defaultApplicationIconName();
}

QPixmap IconProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    QIcon icon = XdgIcon::fromTheme({id});
    // fall back to /usr/share/pixmaps if nothing found yet
    /* shouldn't be necessary - see qiconloader.cpp
    if (icon.isNull()) {
        QStringList p = m_usrSharePixmaps.entryList({id + "*"}, QDir::Files);
        if (!p.isEmpty()) {
            QPixmap ret(m_usrSharePixmaps.filePath(p.first()));
            if (ret.width() > requestedSize.width() || ret.height() > requestedSize.height())
                ret = ret.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            if (size)
                *size = ret.size();
            return ret;
        }
    }
    */
    if (icon.isNull())
        icon = XdgIcon::defaultApplicationIcon(); // TODO doesn't seem to work
    QPixmap ret = icon.pixmap(requestedSize);
    if (size)
        *size = ret.size();
    return ret;
}
