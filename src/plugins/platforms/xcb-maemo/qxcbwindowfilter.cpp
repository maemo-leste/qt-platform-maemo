#include "qxcbwindowfilter.h"
#include "qxcbwindow.h"

#include <QApplication>
#include <QWidget>

QT_BEGIN_NAMESPACE

bool QXcbWindowPropertyFilter::eventFilter(QObject *obj, QEvent *event)
{
    //qCWarning(lcQpaXcb) << "eventFilter: " << event->type();

    if (event->type() == QEvent::PlatformSurface) {
        QPlatformSurfaceEvent *se = static_cast<QPlatformSurfaceEvent *>(event);

        //qCDebug(lcQpaXcb) << "eventFilter surface event: " << event->type() << " type " << se->surfaceEventType();
        if (se->surfaceEventType() == QPlatformSurfaceEvent::SurfaceCreated) {

            if (m_widgetfilter != 0) {
                qCWarning(lcQpaXcb) << "m_widgetfilter != 0 but received SurfaceCreated!";
            } else {
                const QWidgetList topLevelWidgets = QApplication::topLevelWidgets();
                for (QWidget *widget : topLevelWidgets) {
                    //qCWarning(lcQpaXcb) << "Checking widget: " << widget;
                    if (widget->isWindow() && widget->testAttribute(Qt::WA_WState_Created) && widget->winId() == m_qwindow->winId()) {
                        //qCDebug(lcQpaXcb) << "Found widget for window: " << widget;
                        m_widgetfilter = new QXcbWidgetPropertyFilter(widget, this->m_xcbwindow);
                        break;
                    }
                }
            }

            // XXX: When we find a widget, let's also make sure all the
            // properties (if any) are checked, we might have missed the
            // DynamicPropertyChanged event.

        } else if (se->surfaceEventType() == QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed) {
            if (m_widgetfilter == 0) {
                // Not all windows are top level windows, and thus do not have
                // m_widgetfilter assigned.
            } else {
                // Unregister
                delete m_widgetfilter;
                m_widgetfilter = 0;
            }
        }

    } else if (event->type() == QEvent::DynamicPropertyChange) {
        QDynamicPropertyChangeEvent *pe = static_cast<QDynamicPropertyChangeEvent *>(event);

        if (this->m_xcbwindow->windowDynamicPropertyChanged(obj, pe->propertyName()))
            return true;
    }

    return QObject::eventFilter(obj, event);
}

QXcbWindowPropertyFilter::QXcbWindowPropertyFilter(QWindow* window, QXcbWindow* xcbwindow) {
    //qCDebug(lcQpaXcb) << "Propery filter constructor: " << window;

    this->m_xcbwindow = xcbwindow;
    this->m_qwindow = window;
    this->m_qwindow->installEventFilter(this);
    this->m_widgetfilter = 0;
}

QXcbWindowPropertyFilter::~QXcbWindowPropertyFilter() {
    if (this->m_widgetfilter) {
        delete this->m_widgetfilter;
    }
    this->m_qwindow = 0;
    this->m_xcbwindow = 0;
}


QT_END_NAMESPACE
