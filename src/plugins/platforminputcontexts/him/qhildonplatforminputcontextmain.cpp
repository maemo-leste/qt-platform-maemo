#include <qpa/qplatforminputcontextplugin_p.h>

#include <QtCore/QStringList>

#include "qhildonplatforminputcontext.h"

QT_BEGIN_NAMESPACE

class QHildonPlatformInputContextPlugin : public QPlatformInputContextPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QPlatformInputContextFactoryInterface_iid FILE "him.json")

public:
    QHildonInputContext *create(const QString &, const QStringList &) override;
};

QHildonInputContext *QHildonPlatformInputContextPlugin::create(const QString &system, const QStringList &paramList)
{
    Q_UNUSED(paramList);

    if (system.compare(system, QLatin1String("him"), Qt::CaseInsensitive) == 0)
        return new QHildonInputContext;
    return 0;
}

QT_END_NAMESPACE

// XXX: no moc?
#include "qhildonplatforminputcontextmain.moc"
//#include "qhildonplatforminputcontextmain.moc"
