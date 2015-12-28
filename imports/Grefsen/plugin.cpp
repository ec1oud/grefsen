#include <QDir>
#include <QJSEngine>
#include <QLoggingCategory>
#include <QQmlEngine>
#include <QQmlExtensionPlugin>
#include <QtQml>

#include "hoverarea.h"
#include "iconprovider.h"
#include "launchermodel.h"

Q_LOGGING_CATEGORY(lcRegistration, "grefsen.registration")

static const char *ModuleName = "Grefsen";
static QJSValue launcherModelSingleton;

static QJSValue environmentSingletonProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)

    QJSValue v = scriptEngine->newObject();
    v.setProperty("home", QDir::homePath() + "/");
    return v;
}

static QJSValue launcherModelSingletonProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)

    if (launcherModelSingleton.isUndefined())
        launcherModelSingleton = scriptEngine->newQObject(new LauncherModel(scriptEngine));
    return launcherModelSingleton;
}

class GrefsenPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface/1.0")

public:
    GrefsenPlugin() : QQmlExtensionPlugin() { }

    virtual void initializeEngine(QQmlEngine *engine, const char * uri) {
        Q_UNUSED(engine)
        qCDebug(lcRegistration) << uri;
        engine->addImageProvider(QLatin1String("icon"), new IconProvider);
    }

    virtual void registerTypes(const char *uri) {
        qCDebug(lcRegistration) << uri;
        Q_ASSERT(uri == QLatin1String(ModuleName));
        qmlRegisterType<HoverArea>(uri, 1, 0, "HoverArea");
        qmlRegisterSingletonType(ModuleName, 1, 0, "env", environmentSingletonProvider);
        qmlRegisterSingletonType(ModuleName, 1, 0, "launcherModel", launcherModelSingletonProvider);
    }
};

QT_END_NAMESPACE

#include "plugin.moc"
