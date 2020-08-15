#include "qxcbwindowfilter.h"
#include "qxcbwindow.h"

QT_BEGIN_NAMESPACE

bool QXcbWindowPropertyFilter::eventFilter(QObject *obj, QEvent *event)
{

    if (event->type() == QEvent::DynamicPropertyChange) {
        QDynamicPropertyChangeEvent *pe = static_cast<QDynamicPropertyChangeEvent *>(event);

        if (this->m_xcbwindow->windowDynamicPropertyChanged(obj, pe->propertyName()))
            return true;
    }
    return QObject::eventFilter(obj, event);
}

QXcbWindowPropertyFilter::QXcbWindowPropertyFilter(QWindow* window, QXcbWindow* xcbwindow) {
    this->m_xcbwindow = xcbwindow;

    this->m_qwindow = window;
    this->m_qwindow->installEventFilter(this);
}

QXcbWindowPropertyFilter::~QXcbWindowPropertyFilter() {
    this->m_qwindow->removeEventFilter(this);
    this->m_qwindow = 0;
}


QT_END_NAMESPACE
