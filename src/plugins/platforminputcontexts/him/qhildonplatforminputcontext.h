#ifndef QHILDONPLATFORMINPUTCONTEXT_H
#define QHILDONPLATFORMINPUTCONTEXT_H

#include <qpa/qplatforminputcontext.h>

#include <xcb/xcb.h>

#include <QtCore/QCoreApplication>
#include <QGuiApplication>
#include <QAbstractNativeEventFilter>
#include <QtWidgets/QApplication>
#include <QtCore/QList>
#include <QWidget>

#include <qpa/qplatformnativeinterface.h>

#include <QX11Info> // XXX: maybe deprecate this

#include "qhildonplatforminputcontext.h"
#include "qhildonplatforminputproxywidget.h"

#include "qxcbintegration.h"
#include "qxcbconnection.h"


// Include this last, otherwise things blow up
#include <hildon-im-protocol.h>


// XXX: forward-define this for now
class MyXcbEventFilter;


QT_BEGIN_NAMESPACE


namespace QXcbHIMAtom {
    enum Atom {
        _HILDON_IM_WINDOW,
        _HILDON_IM_ACTIVATE,
        _HILDON_IM_SURROUNDING,
        _HILDON_IM_SURROUNDING_CONTENT,
        _HILDON_IM_KEY_EVENT,
        _HILDON_IM_INSERT_UTF8,
        _HILDON_IM_COM,
        _HILDON_IM_CLIPBOARD_COPIED,
        _HILDON_IM_CLIPBOARD_SELECTION_QUERY,
        _HILDON_IM_CLIPBOARD_SELECTION_REPLY,
        _HILDON_IM_INPUT_MODE,
        _HILDON_IM_PREEDIT_COMMITTED,
        _HILDON_IM_PREEDIT_COMMITTED_CONTENT,
        _HILDON_IM_LONG_PRESS_SETTINGS,

        NAtoms,
    };
}


class QHildonInputContext : public QPlatformInputContext
{
    Q_OBJECT

public:
    QHildonInputContext();
    ~QHildonInputContext();

    bool isValid() const override;
    void setFocusObject(QObject *object) override;
    void reset() override;
    void update(Qt::InputMethodQueries) override;
    bool filterEvent(const QEvent *event) override;


    bool x11FilterEvent(xcb_client_message_event_t *event);
    inline xcb_atom_t atom(QXcbHIMAtom::Atom atom) const { return m_allAtoms[atom]; }
protected:

private:
    MyXcbEventFilter *eventFilter;

    void initialiseAtoms(void);
    xcb_atom_t m_allAtoms[QXcbHIMAtom::NAtoms];

    void sendHildonCommand(HildonIMCommand cmd, QWidget *widget);
    void sendInputMode();
    xcb_window_t findHildonIm(void);

    xcb_connection_t* getXcbConnection(void);
    void printEventName(xcb_client_message_event_t* message_event);
};

QT_END_NAMESPACE

#endif // QHILDONPLATFORMINPUTCONTEXT_H
