#include "iconprovider.h"

#include <QDir>

IconProvider::IconProvider()
  : QQuickImageProvider(QQuickImageProvider::Pixmap)
  , m_usrShareIcons("/usr/share/icons")
  , m_usrSharePixmaps("/usr/share/pixmaps")
{
    m_usrSharePixmaps.setNameFilters({"*.png", "*.jpg", "*.xpm", "*.svg"});
    m_iconDirs = m_usrShareIcons.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    qDebug() << m_iconDirs;
}

QPixmap IconProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    qDebug() << id << requestedSize;
    QPixmap ret;

    if (id.startsWith('/')) {
        ret = QPixmap(id);
    } else {
        QStringList p = m_usrSharePixmaps.entryList({id}, QDir::Files);
        if (!p.isEmpty())
            ret = QPixmap(m_usrSharePixmaps.filePath(p.first()));
    }

    if (ret.width() > requestedSize.width() || ret.height() > requestedSize.height())
        ret = ret.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    if (!ret.isNull())
        return ret;

    return QPixmap("/usr/share/icons/oxygen/64x64/apps/" + id + ".png");
}
