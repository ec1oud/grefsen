#ifndef ICONPROVIDER_H
#define ICONPROVIDER_H

#include <QQuickImageProvider>
#include <QDir>

class IconProvider : public QQuickImageProvider
{
public:
    IconProvider();

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) Q_DECL_OVERRIDE;

protected:
    QStringList m_iconDirs;
    QDir m_usrShareIcons;
    QDir m_usrSharePixmaps;
};

#endif // ICONPROVIDER_H
