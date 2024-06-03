/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QMENU_MAEMO5_P_H
#define QMENU_MAEMO5_P_H

#include <QLayout>
#include <QtCore/qpair.h>
#include <QtCore/qlist.h>
#include <QtCore/qmap.h>
#include <QtCore/qpointer.h>
#include <QtWidgets/qdialog.h>
#include <QtWidgets/qmenubar.h>
#include <QtWidgets/qaction.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QAction;
class QMenuBar;
class QGridLayout;
class QActionGroup;
class QButtonGroup;
class QHBoxLayout;
class QAbstractButton;

class GroupLayout
{
public:
    explicit GroupLayout() {
        vl = new QVBoxLayout();
        vl->setSpacing(0);
        vl->setContentsMargins(0, 0, 0, 0);

        hl = new QHBoxLayout();
        hl->addLayout(vl);
    }
    QHBoxLayout *addRow() {
        QHBoxLayout *row = new QHBoxLayout();

        row->setSpacing(0);
        row->setContentsMargins(0, 0, 0, 0);

        rows.append(row);
        vl->addLayout(row);

        return row;
    }
    ~GroupLayout() {
        delete hl;
    }

    QHBoxLayout *hl;
    QVBoxLayout *vl;
    QList<QHBoxLayout *> rows;
};

class QMaemo5ApplicationMenu : public QDialog
{
    Q_OBJECT

public:
    QMaemo5ApplicationMenu(QMenuBar *menubar);

    bool isEmpty() const;
    QAction *selectedAction() const;


protected:
#if 0
    void paintEvent(QPaintEvent *);
#endif
    void showEvent(QShowEvent *);

private slots:
    void buttonClicked(bool);
    void actionChanged();
    void actionGroupDestroyed(QObject *actionGroup);

private:
    void updateMenuActions();
    void updateRootMenubar();
    void buildActions(const QList<QAction *> &actions, QGridLayout *grid, int &row, int &col, int maxcol);
    bool addToActionsLayout(QAbstractButton *button, QAction *action);

    QMap<QWidget *, QPointer<QAction> > m_actions;
    typedef QMap<QActionGroup *, QPair<GroupLayout *, QButtonGroup *> > GroupMap;
    GroupMap m_groups;
    QPointer<QAction> m_selected;

    /* Root menu bar, as passed to us */
    QWidget * m_menuBar;

    /* Current (nested) menu */
    QWidget* m_currentMenu;

    /* Our nested menu path */
    QList<QMenu *> m_menu_nest;

    /* Store this for matching later */
    QAction *m_backAction = nullptr;
};


QT_END_NAMESPACE

QT_END_HEADER

#endif // QMENU_MAEMO_5_H
