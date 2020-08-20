#ifndef QXCBWIDGETFILTER_H
#define QXCBWIDGETFILTER_H

#include <qpa/qplatformwindow.h>
#include <QtPlatformHeaders/qxcbwindowfunctions.h>
#include "qxcbwidgetfilter.h"
#include "qxcbconnection.h"

QT_BEGIN_NAMESPACE

class QXcbWidgetPropertyFilter: public QObject
{
    Q_OBJECT

public:
    QXcbWidgetPropertyFilter(QWidget* widget, QXcbWindow *xcbwindow);
    ~QXcbWidgetPropertyFilter();
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
private:
    QWidget* m_qwidget = 0;
    QXcbWindow* m_xcbwindow = 0;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QXcbWidgetPropertyFilter*)

#endif
