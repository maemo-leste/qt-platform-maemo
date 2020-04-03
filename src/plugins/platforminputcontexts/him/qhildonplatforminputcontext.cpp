#include "qhildonplatforminputcontext.h"

#include <QtCore/QCoreApplication>
#include <QGuiApplication>

#include <QtGui/QKeyEvent>
#include <QtCore/QDebug>

#include <iostream>

QHildonInputContext::QHildonInputContext()
{
    std::cerr << "QHildonInputContext init!" << std::endl;
    eventFilter = new MyXcbEventFilter();
    QGuiApplication *currentApp = qGuiApp;

    if (currentApp) {
        currentApp->installNativeEventFilter((QAbstractNativeEventFilter*)eventFilter);
    } else {
        std::cerr << "No application!" << std::endl;
    }

}

QHildonInputContext::~QHildonInputContext() {
}

bool QHildonInputContext::filterEvent(const QEvent *event)
{
    Q_UNUSED(event);
    return false;
}

bool QHildonInputContext::isValid() const
{
    return true;
}

void QHildonInputContext::setFocusObject(QObject *object)
{
    Q_UNUSED(object);
}

void QHildonInputContext::reset()
{
}

void QHildonInputContext::update(Qt::InputMethodQueries q)
{
    QPlatformInputContext::update(q);
}


QT_END_NAMESPACE
