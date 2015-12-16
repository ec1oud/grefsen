#include "iconprovider.h"

#include <QDir>
#include <QDirIterator>

IconProvider::IconProvider()
  : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
    m_usrSharePixmaps.setNameFilters(m_nameFilters);
    m_iconDirs = m_usrShareIcons.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    qDebug() << m_iconDirs;
}

QPixmap IconProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
//    qDebug() << id << requestedSize;
    QPixmap ret;

    // TODO check resources

    if (id.startsWith('/')) {
        // if a full path is given, use it
        ret = QPixmap(id);
    } else {
        // try grefsen config dir
        QDirIterator it(QDir::homePath() + QStringLiteral("/.config/grefsen"),
                        m_nameFilters, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext() && ret.isNull()) {
            if (it.next().contains(id, Qt::CaseInsensitive))
                ret = QPixmap(it.filePath());
        }

        if (ret.isNull()) {
            // try looking in by-size directories (and others) under /usr/share/icons
            // TODO if we want 64x64 but the largest is found in e.g. 48x48, use that instead
            // /usr/share/icons/hicolor contains a 64x64 dir but not all apps have icons there
            // so just use QDirIterator, find matches by name, then find the closest size among them
            foreach (QString tp, m_iconDirs) {
                QDir tpd(m_usrShareIcons);
                tpd.cd(tp);
                QString sizeDir;
                int size = 0;
                // Look at by-size directories and find the closest size match
                foreach (QString s, tpd.entryList(QDir::AllDirs | QDir::NoDotAndDotDot)) {
                    QStringList xByY = s.split('x');
                    if (xByY.isEmpty() && sizeDir.isEmpty())
                        sizeDir = s; // e.g. "scalable"
                    else {
                        bool ok = false;
                        int sz = xByY.first().toInt(&ok);
                        if (ok && qAbs(sz - requestedSize.width()) < qAbs(size - requestedSize.width())) {
                            size = sz;
                            sizeDir = s;
                        }
                    }
                }
                if (sizeDir.isEmpty())
                    continue;
                QDir spd(tpd);
                spd.cd(sizeDir);
                QDirIterator spdsubs(spd, QDirIterator::Subdirectories);
                while (spdsubs.hasNext()) {
                    if (spdsubs.next().contains(id, Qt::CaseInsensitive)) {
                        ret = QPixmap(spdsubs.filePath());
                        break;
                    }
                }
                if (!ret.isNull())
                    break;
            }

            // fall back to /usr/share/pixmaps if nothing found yet
            if (ret.isNull()) {
                QStringList p = m_usrSharePixmaps.entryList({id + "*"}, QDir::Files);
                if (!p.isEmpty())
                    ret = QPixmap(m_usrSharePixmaps.filePath(p.first()));
            }
        }
    }

    if (ret.width() > requestedSize.width() || ret.height() > requestedSize.height())
        ret = ret.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    return ret;
}
