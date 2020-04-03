#ifndef QHILDONPLATFORMINPUTEVENTFILTER_H
#define QHILDONPLATFORMINPUTEVENTFILTER_H

#include <QAbstractNativeEventFilter>
#include <QWidget>

#include "qxcbintegration.h"
#include "qxcbconnection.h"

#include <hildon-im-protocol.h>

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


class MyXcbEventFilter : public QAbstractNativeEventFilter
{
public:
    MyXcbEventFilter();
    ~MyXcbEventFilter();

    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;
    void initialiseAtoms(void);

    inline xcb_atom_t atom(QXcbHIMAtom::Atom atom) const { return m_allAtoms[atom]; }

	void setFocusObject(QObject *object);


private:
    void sendHildonCommand(HildonIMCommand cmd, QWidget *widget);
    xcb_window_t findHildonIm(void);
    xcb_atom_t m_allAtoms[QXcbHIMAtom::NAtoms]; // XXX: m_allAtoms name is from Xcb platform
    xcb_connection_t* getXcbConnection(void);
};



class QHIMProxyWidget : public QWidget {
    Q_OBJECT
public:
    QHIMProxyWidget(QWidget *widget);
    virtual ~QHIMProxyWidget();
    QWidget *widget() const;
    static QHIMProxyWidget *proxyFor(QWidget *w);
private Q_SLOTS:
    void widgetWasDestroyed();
private:
    static QMap<QWidget *, QHIMProxyWidget *> proxies;
    QWidget *w;
};


#endif // QHILDONPLATFORMINPUTEVENTFILTER_H
