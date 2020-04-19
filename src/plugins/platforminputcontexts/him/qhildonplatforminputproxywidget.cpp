#include "qhildonplatforminputproxywidget.h"

QMap<QWidget *, QHIMProxyWidget *> QHIMProxyWidget::proxies;

QHIMProxyWidget::QHIMProxyWidget(QWidget *widget)
    : QWidget(0), w(widget)
{
    setAttribute(Qt::WA_InputMethodEnabled);
    setAttribute(Qt::WA_NativeWindow);
    createWinId();
    connect(w, SIGNAL(destroyed()), this, SLOT(widgetWasDestroyed()));
}

QHIMProxyWidget::~QHIMProxyWidget()
{
}

QWidget *QHIMProxyWidget::widget() const
{
    return w;
}

QHIMProxyWidget *QHIMProxyWidget::proxyFor(QWidget *w)
{
    QHIMProxyWidget *proxy = qobject_cast<QHIMProxyWidget *>(w);

    if (!proxy)
        proxy = proxies.value(w);

    if (!proxy) {
        proxy = new QHIMProxyWidget(w);
        proxies.insert(w, proxy);
    }
    //qWarning() << "Using HIM Proxy widget" << proxy << "for widget" << w << "isnative: " << proxy->testAttribute(Qt::WA_NativeWindow) << " / " << w->testAttribute(Qt::WA_NativeWindow);

    return proxy;
}

void QHIMProxyWidget::widgetWasDestroyed()
{
    proxies.remove(w);
    delete this;
}

