#include "iconprovider.h"

QPixmap IconProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    qDebug() << id << requestedSize;

    return QPixmap("/usr/share/icons/oxygen/64x64/apps/" + id + ".png");
}
