#include "qxcbwidgetfilter.h"
#include "qxcbwindow.h"

#include <QApplication>
#include <QWidget>

QT_BEGIN_NAMESPACE

bool QXcbWidgetPropertyFilter::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::DynamicPropertyChange) {
        QDynamicPropertyChangeEvent *pe = static_cast<QDynamicPropertyChangeEvent *>(event);
        if (this->m_xcbwindow->windowDynamicPropertyChanged(obj, pe->propertyName()))
            return true;
    }
    return QObject::eventFilter(obj, event);
}

QXcbWidgetPropertyFilter::QXcbWidgetPropertyFilter(QWidget* widget, QXcbWindow* xcbwindow) {
    //qCDebug(lcQpaXcb) << "Property filter constructor: " << widget;
    this->m_qwidget = widget;
    this->m_xcbwindow = xcbwindow;

    this->m_qwidget->installEventFilter(this);

    // XXX: This is a hack: but just trigger them now.
    this->m_xcbwindow->windowDynamicPropertyChanged(m_qwidget, "X-Maemo-Progress");
    this->m_xcbwindow->windowDynamicPropertyChanged(m_qwidget, "X-Maemo-NotComposited");
    this->m_xcbwindow->windowDynamicPropertyChanged(m_qwidget, "X-Maemo-StackedWindow");
}

QXcbWidgetPropertyFilter::~QXcbWidgetPropertyFilter() {
    this->m_qwidget->removeEventFilter(this);
    this->m_qwidget = 0;

    this->m_xcbwindow = 0;
}


QT_END_NAMESPACE
