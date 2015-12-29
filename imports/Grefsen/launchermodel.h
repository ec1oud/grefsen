#ifndef LAUNCHERMODEL_H
#define LAUNCHERMODEL_H

#include <QDomElement>
#include <QJSValue>
#include <QObject>

class XdgDesktopFile;

class LauncherModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QJSValue allApplications READ allApplications NOTIFY applicationsChanged)
    Q_PROPERTY(QJSValue applicationMenu READ applicationMenu NOTIFY applicationsChanged)

public:
    explicit LauncherModel(QJSEngine *engine, QObject *parent = 0);
    QJSValue allApplications();
    QJSValue applicationMenu() { return m_list.property(QStringLiteral("items")); }

signals:
    void applicationsChanged();
    void execFailed(QString error);

public slots:
    void reset();
    void select(QJSValue sel);
    void exec(QString desktopFilePath);
    void openSubmenu(QString title);

protected:
    void build(QJSValue in, const QDomElement& xml);
    void appendMenu(QJSValue in, const QDomElement& xml);
    void appendApp(QJSValue in, const QDomElement &xml);
    QJSValue findFirst(QString key, QString value, QJSValue array);

protected:
//    static QList<XdgDesktopFile *> m_allFiles;
    QJSEngine *m_engine;
    QDomElement m_dom;
    QJSValue m_root;
    QJSValue m_list;
};

#endif // LAUNCHERMODEL_H
