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

#include <QLayout>
#include <QAction>
#include <QWidgetAction>
#include <QActionGroup>
#include <QButtonGroup>
#include <QPushButton>
#include <QRadioButton>
#include <QApplication>
#include <QDesktopWidget>
#include <QVarLengthArray>
#include <QMaemo5Style>

//#include "qx11info_x11.h"

//#include <private/qgtkpainter_p.h>
//#include <private/qmenubar_p.h>
#include <qmenu_maemo.h>
//#include <private/qt_x11_p.h>


#include <qxcbintegration.h>
#include <qxcbconnection.h>

QT_BEGIN_NAMESPACE

#if 0
void QMenuBarPrivate::maemoResetApplicationMenuAtom(QWidget *w)
{
    if (!w)
        return;

    QWidget *window = w->window();
    bool has_menubar = (window->findChild<QMenuBar *>());

    Atom *oldAtoms = 0;
    int oldCount = 0;

    if (!XGetWMProtocols(QX11Info::display(), window->winId(), &oldAtoms, &oldCount))
        return;

    Atom customContext = XInternAtom(QX11Info::display(), "_NET_WM_CONTEXT_CUSTOM", false);

    // re-create the Atom list with or without the customContext Atom
    QVarLengthArray<Atom> newAtoms(oldCount + 1);
    int newCount = 0;
    bool found = false;

    for (int i = 0; i < oldCount; ++i) {
        if (oldAtoms[i] == customContext) {
            found = true;
            if (!has_menubar) // no menubar anymore -> remove atom
                continue;
        }
        newAtoms[newCount++] = oldAtoms[i];
    }
    if (!found && has_menubar) // first menubar in window -> add atom
        newAtoms[newCount++] = customContext;

    XFree(oldAtoms);
    XSetWMProtocols(QX11Info::display(), window->winId(), newAtoms.data(), newCount);

    // also show the indicator (without this, the menu would work, but the
    // little indicator arrow in the titlebar would be missing)
    if (has_menubar) {
        int on = 1;
        XChangeProperty(X11->display, window->winId(), ATOM(_HILDON_WM_WINDOW_MENU_INDICATOR), XA_INTEGER, 32,
                        PropModeReplace, (unsigned char *) &on, 1);
    } else {
        XDeleteProperty(X11->display, window->winId(), ATOM(_HILDON_WM_WINDOW_MENU_INDICATOR));
    }
}
#endif

class GroupLayout
{
public:
    explicit GroupLayout() {
        vl = new QVBoxLayout();
        vl->setSpacing(0);
        vl->setContentsMargins(0, 0, 0, 0);

        hl = new QHBoxLayout();
        hl->addLayout(vl);

        addRow();
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

void clearWidgets(QLayout * layout) {
    if (! layout)
        return;
    while (auto item = layout->takeAt(0)) {
        delete item->widget();
        clearWidgets(item->layout());
        delete item;
    }
}

QMaemo5ApplicationMenu::QMaemo5ApplicationMenu(QMenuBar *menubar)
    : QDialog(menubar->window())
{
    // just close the menu in case the orientation changes
    connect(QApplication::desktop(), SIGNAL(resized(int)), this, SLOT(close()));

    m_menuBar = menubar;
    updateRootMenubar();
    m_currentMenu = m_menuBar;

    updateMenuActions();
}

void QMaemo5ApplicationMenu::updateRootMenubar() {
    /* If we can only find a single menu in the QMenuBar and no actual actions,
     * then we traverse to the first single menu. */
    bool found_actions = false;
    int submenu_count = 0;
    QWidget *m = nullptr;

    for(QAction *a : m_menuBar->actions()) {
        if (!a->menu()) {
            found_actions = true;
        }
        if (a->menu()) {
            submenu_count += 1;
            m = a->menu();
        }
    }

    if (!found_actions && submenu_count == 1) {
        m_menuBar = m;
    }
}

void QMaemo5ApplicationMenu::updateMenuActions() {
    int desktop_width = QApplication::desktop()->screenGeometry().width();
    bool portrait = desktop_width < QApplication::desktop()->screenGeometry().height();
    int maxcol = portrait ? 1 : 2;

    int horizontal_spacing = 16;
    int vertical_spacing = 16;
    int content_margin = 16;
    int xoffset = 50;
    int group_width = desktop_width - xoffset;
    int group_spacing = 16;

    if (qobject_cast<QMaemo5Style *>(style())) {
        horizontal_spacing = style()->pixelMetric(static_cast<QStyle::PixelMetric>(QMaemo5Style::PM_Maemo5AppMenuHorizontalSpacing));
        vertical_spacing   = style()->pixelMetric(static_cast<QStyle::PixelMetric>(QMaemo5Style::PM_Maemo5AppMenuVerticalSpacing));
        content_margin     = style()->pixelMetric(static_cast<QStyle::PixelMetric>(QMaemo5Style::PM_Maemo5AppMenuContentMargin));
        xoffset            = style()->pixelMetric(static_cast<QStyle::PixelMetric>(QMaemo5Style::PM_Maemo5AppMenuLandscapeXOffset));
        group_width        = style()->pixelMetric(static_cast<QStyle::PixelMetric>(QMaemo5Style::PM_Maemo5AppMenuFilterGroupWidth));
        group_spacing      = style()->pixelMetric(static_cast<QStyle::PixelMetric>(QMaemo5Style::PM_Maemo5AppMenuFilterGroupVerticalSpacing));
    }

    int menu_width = desktop_width - (portrait ? 0 : (2 * xoffset));

    m_actions.clear();
    m_groups.clear();

    clearWidgets(layout());
    delete layout();

    QVBoxLayout *topLayout = new QVBoxLayout(this);
    topLayout->setContentsMargins(content_margin, content_margin, content_margin, content_margin);
    topLayout->setSpacing(0);

    QGridLayout *grid = new QGridLayout();
    grid->setHorizontalSpacing(horizontal_spacing);
    grid->setVerticalSpacing(vertical_spacing);
    grid->setContentsMargins(0, 0, 0, 0);
    setFixedWidth(menu_width);

    // --- add all actions
    int row = 0, col = 0;

    buildActions(m_currentMenu->actions(), grid, row, col, maxcol);

    // -- ensure that both columns have the same width
    if (!portrait) {
        int column_width = (menu_width - horizontal_spacing - 2*content_margin) / grid->columnCount();

        for (int i = 0; i < grid->columnCount(); i++)
            grid->setColumnMinimumWidth( i, column_width);
    }

    // --- combine all the layouts
    for (GroupMap::const_iterator it = m_groups.constBegin(); it != m_groups.constEnd(); ++it) {
        QHBoxLayout *groupLayout = it.value().first->hl;
        groupLayout->setSpacing(0); // the buttons should be next to each other
        groupLayout->setContentsMargins(0, 0, 0, group_spacing);
        groupLayout->insertSpacerItem( 0, new QSpacerItem( (menu_width-2*content_margin-group_width)/2, 0,
                    QSizePolicy::Maximum, QSizePolicy::Minimum  ));
        groupLayout->insertSpacerItem( -1, new QSpacerItem( (menu_width-2*content_margin-group_width)/2, 0,
                    QSizePolicy::Maximum, QSizePolicy::Minimum  ));
        topLayout->addLayout(groupLayout);
    }
    topLayout->addLayout(grid);
}

void QMaemo5ApplicationMenu::buildActions(const QList<QAction *> &actions, QGridLayout *grid, int &row, int &col, int maxcol)
{
    QList<QAction *> acts = QList<QAction*>(actions);

    for(QAction *a : acts) {
        if (!a->isVisible() || !a->isEnabled() || a->isSeparator()) {
            // we won't show actions that are hidden or disabled, separators and widget actions
            continue;
        }
        else {
            QWidget *w = 0;
            bool isActionButton = 0;

            if (QWidgetAction *wa = ::qobject_cast<QWidgetAction *>(a)) {
                w = wa->requestWidget(this);

                if (!w)
                    continue;
            } else {
                QAbstractButton *button;
                if (a->isCheckable() && a->actionGroup() && a->actionGroup()->isExclusive()) {
                    button = new QRadioButton(this);
                } else {
                    button = new QPushButton(this);
                    button->setCheckable(a->isCheckable());
                }
                if (button->isCheckable())
                    button->setChecked(a->isChecked());

                if (a->menu()) {
                    QFont font(button->font());
                    font.setUnderline(true);
                    button->setFont(font);
                }

                button->setText(a->text());

                if (!a->icon().isNull())
                    button->setIcon(a->icon());

                connect(button, SIGNAL(clicked(bool)), this, SLOT(buttonClicked(bool)));

                isActionButton = addToActionsLayout(button, a);
                w = button;
            }

            connect(a, SIGNAL(changed()), this, SLOT(actionChanged()));
            m_actions.insert(w, a);

            if (!isActionButton) { // no special layout. just add it to the grid
                grid->addWidget(w, row, col++);

                if (col >= maxcol) {
                    col = 0;
                    ++row;
                }
            }
        }
    }

    acts.clear();
}

static bool textOverflows(QHBoxLayout *layout, int colwidth, int textWidth)
{
    if (textWidth > colwidth)
        return true;

    for (int i = 0; i < layout->count(); i++) {
        auto button = qobject_cast<QAbstractButton *>(layout->itemAt(i)->widget());

        if (button->fontMetrics().horizontalAdvance(button->text()) > colwidth)
            return true;
    }

    return false;
}

/*! \internal
 *  Creates a (or finds an existing) QButtonGroup and QHBoxLayout in the case where the button
 *  should be added to them.
 *  Returns if button was added to special layout.
 */
bool QMaemo5ApplicationMenu::addToActionsLayout(QAbstractButton *button, QAction *action)
{
    QActionGroup *actionGroup = action->actionGroup();
    if (actionGroup && actionGroup->isExclusive() && action->isCheckable()) {

        QButtonGroup *buttonGroup = 0;
        GroupLayout *layout = 0;

        GroupMap::const_iterator it = m_groups.constFind(actionGroup);
        if (it == m_groups.constEnd()) {  // we need to construct a new group
            buttonGroup = new QButtonGroup(this);

            layout = new GroupLayout();
            m_groups.insert(actionGroup, qMakePair(layout, buttonGroup));
            connect(actionGroup, SIGNAL(destroyed(QObject*)), this, SLOT(actionGroupDestroyed(QObject*)));

        } else {
            buttonGroup = it.value().second;
            layout = it.value().first;
        }

        if (button->group() != buttonGroup)
            buttonGroup->addButton(button);

        QHBoxLayout *row_layout = layout->rows.last();
        int textWidth = button->fontMetrics().horizontalAdvance(button->text());
        int cols = row_layout->count() + 1;

        int maxwidth = maximumWidth() - 32;
        int maxcolwidth = maxwidth / cols;

        if (cols > 5 || (cols > 1 && textOverflows(row_layout, maxcolwidth, textWidth)))
            row_layout = layout->addRow();

        row_layout->addWidget(button);

        return true;
    }

    return false;
}

bool QMaemo5ApplicationMenu::isEmpty() const
{
    return m_actions.isEmpty();
}

#if 0
void QMaemo5ApplicationMenu::paintEvent(QPaintEvent *)
{
    if (qobject_cast<QMaemo5Style *>(style())) {
        QPainter painter(this);
        QStyleOption option;
        option.initFrom(this);
        style()->drawPrimitive(static_cast<QStyle::PrimitiveElement>(QMaemo5Style::PE_Maemo5AppMenu),
                               &option, &painter, this);
    }
}
#endif

void QMaemo5ApplicationMenu::showEvent(QShowEvent *se)
{
    QDialog::showEvent(se);

    // XXX: Ugly/dedupe
    QPlatformNativeInterface *platformNativeInterface = QGuiApplication::platformNativeInterface();
    QXcbIntegration* xcbplatform = (QXcbIntegration*)platformNativeInterface;
    xcbplatform = xcbplatform->instance();
    QXcbConnection* conn = xcbplatform->defaultConnection();
    xcb_connection_t* realconn = conn->xcb_connection();

    long appmenutype = conn->atom(QXcbAtom::_HILDON_WM_WINDOW_TYPE_APP_MENU);
    long net_wm_type = conn->atom(QXcbAtom::_NET_WM_WINDOW_TYPE);

    xcb_change_property(realconn, XCB_PROP_MODE_REPLACE, winId(), net_wm_type,
            XCB_ATOM_ATOM, 32, 1, (void*)&appmenutype);
}

void QMaemo5ApplicationMenu::updateMenu()
{

    setUpdatesEnabled(false);

    updateMenuActions();

    /* In case our previous menu was larger than the current one, adjust our
     * size to make sure fit to our current menu. We have to process all events
     * before doing so otherwise our window is recreated sometimes, instead of
     * just being resized */
    QCoreApplication::processEvents();
    adjustSize();

    setUpdatesEnabled(true);

    m_selected = nullptr;
}

void QMaemo5ApplicationMenu::reject()
{
    if (m_menu_nest.count()) {
        m_menu_nest.removeLast();

        if (m_menu_nest.count())
            m_currentMenu = m_menu_nest.last();
        else
            m_currentMenu = m_menuBar;

        updateMenu();
    } else {
        QDialog::reject();
    }
}

void QMaemo5ApplicationMenu::buttonClicked(bool)
{
    QAbstractButton *button = qobject_cast<QAbstractButton *>(sender());

    if (button) {
        m_selected = m_actions.value(button).data();
        if (m_selected) {
            /* In case we're going back */
            if (m_selected->menu()) {
                m_currentMenu = m_selected->menu();
                QMenu *menu = qobject_cast<QMenu*>(m_currentMenu);
                m_menu_nest.append(menu);
                updateMenu();
            } else {
                accept();
            }
        }
    }
}

void QMaemo5ApplicationMenu::actionChanged()
{
    if (QAction *a = qobject_cast<QAction *>(sender())) {
        for (QWidget *w : m_actions.keys(a)) {
            if (QAbstractButton *button = qobject_cast<QAbstractButton *>(w))
                button->setChecked(a->isChecked());
        }
    }
}

void QMaemo5ApplicationMenu::actionGroupDestroyed(QObject *obj)
{
    QPair<GroupLayout *, QButtonGroup *> entry = m_groups.take(static_cast<QActionGroup *>(obj));
    delete entry.second;
    delete entry.first;
}

QAction *QMaemo5ApplicationMenu::selectedAction() const
{
    return m_selected;
}


QT_END_NAMESPACE

