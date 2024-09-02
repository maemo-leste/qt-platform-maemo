/*
 * qmaemotapandhold.C
 *
 * Copyright (C) 2024 Ivaylo Dimitrov <ivo.g.dimitrov.75@gmail.com>
 *
 * This library is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library. If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "qmaemotapandhold.h"

#include <QCursor>
#include <QTapAndHoldGesture>
#include <QApplication>
#include <QTimer>

#include <qpa/qwindowsysteminterface.h>
#include <qpa/qplatformcursor.h>

Q_GLOBAL_STATIC(QMaemoTapAndHold, tapAndHold);

void QMaemoTapAndHold::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == timerId) {
        killTimer(timerId);
        timerId = 0;

        if (window.isNull())
            return;

        const QPoint globalPos = window->screen()->handle()->cursor()->pos();

        // Exits if the cursor is not in the surrounding area
        // of the press event
        const QPoint deltaPos = this->globalPos - globalPos;
        if (qAbs(deltaPos.x()) >= LONG_TAP_MAX_DIST ||
                qAbs(deltaPos.y()) >= LONG_TAP_MAX_DIST){
            return;
        }

        QPoint pos = window->mapFromGlobal(globalPos);
        QContextMenuEvent e(QContextMenuEvent::Mouse, pos, globalPos, modifiers);
        if (QWidget *widget = QApplication::widgetAt(globalPos))
            QApplication::sendEvent((QObject *)widget, &e);
    }
}

QMaemoTapAndHold *
QMaemoTapAndHold::instance()
{
    return tapAndHold;
}

void QMaemoTapAndHold::start(QWindow *window, Qt::KeyboardModifiers modifiers)
{
    this->window = window;
    this->globalPos = window->screen()->handle()->cursor()->pos();
    this->modifiers = modifiers;
    timerId = startTimer(QTapAndHoldGesture::timeout());
}

void QMaemoTapAndHold::stop()
{
    if (timerId)
        killTimer(timerId);

    timerId = 0;
}
