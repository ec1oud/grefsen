#include "launchermodel.h"
#include <QDebug>
#include <QJSEngine>
#include <XmlHelper>
#include <XdgDesktopFile>
#include <XdgMenu>

LauncherModel::LauncherModel(QJSEngine *engine, QObject *parent)
  : QObject(parent)
  , m_engine(engine)
  , m_root(engine->newObject())
{
    QString menuFile = XdgMenu::getMenuFileName();
    XdgMenu xdgMenu;
    xdgMenu.setLogDir("/tmp");
    xdgMenu.setEnvironments(QStringList() << "X-GREFSEN" << "Grefsen");  // TODO what's that for?
//    xdgMenu.setEnvironments(QStringList() << "X-LXQT" << "LXQt");
    bool res = xdgMenu.read(menuFile);
    if (!res)
        qWarning() << "Parse error" << xdgMenu.errorString();
    m_dom = xdgMenu.xml().documentElement();
    qDebug() << "read XML:" << menuFile << xdgMenu.menuFileName() << m_dom.tagName();

    build(m_root, m_dom);
    m_list = m_root;
}

void LauncherModel::reset()
{
    m_list = m_root;
    emit applicationsChanged();
}

void LauncherModel::select(QJSValue sel)
{
    QString title = sel.property(QStringLiteral("title")).toString();
    qDebug() << title;
    if (sel.hasOwnProperty(QStringLiteral("exec"))) {
//qDebug() << "exec" << sel.property(QStringLiteral("exec")).toString();
        exec(sel.property(QStringLiteral("desktopFile")).toString());
        reset();
    } else {
        openSubmenu(title);
    }
}

void LauncherModel::exec(QString desktopFilePath)
{
    XdgDesktopFile* dtf = XdgDesktopFileCache::getFile(desktopFilePath);
//qDebug() << desktopFilePath << dtf;
    if (dtf) {
        bool ok = dtf->startDetached();
        if (Q_UNLIKELY(!ok))
            emit execFailed(tr("failed to exec '%s'", dtf->value(QStringLiteral("exec")).toString().toLocal8Bit().constData()));
    } else
        emit execFailed(tr("failed to find desktop file '%s'", desktopFilePath.toLocal8Bit().constData()));
}

void LauncherModel::openSubmenu(QString title)
{
    m_list = findFirst(QStringLiteral("title"), title, applicationMenu());
    emit applicationsChanged();
}

void LauncherModel::build(QJSValue in, const QDomElement &xml)
{
    QJSValue array = m_engine->newArray();
    in.setProperty(QStringLiteral("items"), array);
    DomElementIterator it(xml, QString());
    while(it.hasNext())
    {
        QDomElement xml = it.next();

        if (xml.tagName() == "Menu")
            appendMenu(array, xml);

        else if (xml.tagName() == "AppLink")
            appendApp(array, xml);

        else if (xml.tagName() == "Separator")
            qDebug() << "separator";
    }
}

void LauncherModel::appendMenu(QJSValue in, const QDomElement &xml)
{
    QString title = xml.attribute(QStringLiteral("title"));
    QString icon = xml.attribute(QStringLiteral("icon"));
    int idx = in.property(QStringLiteral("length")).toInt();
//    qDebug() << in.property(QStringLiteral("title")).toString() << ":" << title << icon << idx;
    QJSValue menu = m_engine->newObject();
    menu.setProperty(QStringLiteral("title"), title);
    menu.setProperty(QStringLiteral("icon"), icon);
    in.setProperty(idx, menu);
    build(menu, xml);
}

void LauncherModel::appendApp(QJSValue in, const QDomElement &xml)
{
    QString title = xml.attribute(QStringLiteral("title"));
    QString icon = xml.attribute(QStringLiteral("icon"));
    int idx = in.property(QStringLiteral("length")).toInt();
//    qDebug() << in.property(QStringLiteral("title")).toString() << ":" << title << icon << idx;
    QJSValue item = m_engine->newObject();
    item.setProperty(QStringLiteral("title"), title);
    item.setProperty(QStringLiteral("icon"), icon);
    item.setProperty(QStringLiteral("exec"), xml.attribute(QStringLiteral("exec")));
    item.setProperty(QStringLiteral("desktopFile"), xml.attribute(QStringLiteral("desktopFile")));
    in.setProperty(idx, item);
}

QJSValue LauncherModel::findFirst(QString key, QString value, QJSValue array)
{
    int i = 0;
    QJSValue next = array.property(i);
    while (!next.isUndefined()) {
        if (next.property(key).toString() == value)
            return next;
        next = array.property(++i);
    }
    return QJSValue();
}
