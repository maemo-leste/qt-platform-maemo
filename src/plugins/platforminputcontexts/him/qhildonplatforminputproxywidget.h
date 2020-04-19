#ifndef QHILDONPLATFORMINPUTEVENTPROXY_H
#define QHILDONPLATFORMINPUTEVENTPROXY_H

#include <QMap>
#include <QWidget>

class QHIMProxyWidget : public QWidget {
    Q_OBJECT
public:
    QHIMProxyWidget(QWidget *widget);
    virtual ~QHIMProxyWidget();
    QWidget *widget() const;
    static QHIMProxyWidget *proxyFor(QWidget *w);
private Q_SLOTS:
    void widgetWasDestroyed();
private:
    static QMap<QWidget *, QHIMProxyWidget *> proxies;
    QWidget *w;
};

#endif // QHILDONPLATFORMINPUTEVENTPROXY_H
