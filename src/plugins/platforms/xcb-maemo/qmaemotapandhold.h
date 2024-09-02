/*
 * qmaemotapandhold.h
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

#ifndef QMAEMOTAPANDHOLD_H
#define QMAEMOTAPANDHOLD_H

#include <QObject>
#include <QWindow>
#include <QPointer>

class QMaemoTapAndHold : public QObject
{
    Q_OBJECT
public:
    explicit QMaemoTapAndHold(QObject *parent = 0) :
        QObject(parent),
        timerId(0)
    { }

public:
    static QMaemoTapAndHold *instance();
    void start(QWindow *window, Qt::KeyboardModifiers modifiers);
    void stop();

    enum { LONG_TAP_MAX_DIST = 15 };

    int timerId;
    QPoint globalPos;
    Qt::KeyboardModifiers modifiers;
    QPointer<QWindow> window;
protected:
     void timerEvent(QTimerEvent *event);
};

#endif // QMAEMOTAPANDHOLD_H
