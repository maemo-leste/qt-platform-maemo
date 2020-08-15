#ifndef QXCBWINDOWFILTER_H
#define QXCBWINDOWFILTER_H

#include <qpa/qplatformwindow.h>
#include <QtPlatformHeaders/qxcbwindowfunctions.h>
#include "qxcbconnection.h"


QT_BEGIN_NAMESPACE

class QXcbWindowPropertyFilter: public QObject
{
    Q_OBJECT

public:
    QXcbWindowPropertyFilter(QWindow* window, QXcbWindow *xcbwindow);
    ~QXcbWindowPropertyFilter();
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
private:
    QWindow* m_qwindow;
    QXcbWindow* m_xcbwindow;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QXcbWindowPropertyFilter*)

#endif
