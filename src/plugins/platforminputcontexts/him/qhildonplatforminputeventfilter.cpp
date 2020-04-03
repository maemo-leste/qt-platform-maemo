#include <iostream>

#include <xcb/xcb.h>

#include <QtCore/QCoreApplication>
#include <QGuiApplication>
#include <QAbstractNativeEventFilter>

#include <qpa/qplatformnativeinterface.h>

#include <QX11Info> // XXX: maybe deprecate this

//#include <hildon-input-method/hildon-im-protocol.h>

#include "qhildonplatforminputeventfilter.h"


MyXcbEventFilter::MyXcbEventFilter()
{
    initialiseAtoms();
	findHildonIm();
}

MyXcbEventFilter::~MyXcbEventFilter()
{
    //sendHildonCommand(HILDON_IM_HIDE);
}

static const char * xcb_atomnames = {
    "_HILDON_IM_WINDOW\0"                    // find the global im window
    "_HILDON_IM_ACTIVATE\0"                  // activate the input method
    "_HILDON_IM_SURROUNDING\0"               // send surrounding
    "_HILDON_IM_SURROUNDING_CONTENT\0"       // send surrounding header
    "_HILDON_IM_KEY_EVENT\0"                 // send key event to im
    "_HILDON_IM_INSERT_UTF8\0"               // input method wants to insert data
    "_HILDON_IM_COM\0"                       // input method wants to communicate with us
    "_HILDON_IM_CLIPBOARD_COPIED\0"          //### NOT USED YET
    "_HILDON_IM_CLIPBOARD_SELECTION_QUERY\0" //### NOT USED YET
    "_HILDON_IM_CLIPBOARD_SELECTION_REPLY\0" // tell im whether we have a selection or not
    "_HILDON_IM_INPUT_MODE\0"
    "_HILDON_IM_PREEDIT_COMMITTED\0"
    "_HILDON_IM_PREEDIT_COMMITTED_CONTENT\0"
    "_HILDON_IM_LONG_PRESS_SETTINGS\0"
};

// Function taken frm XcbPlatform plugin
void MyXcbEventFilter::initialiseAtoms(void)
{
    const char *names[QXcbHIMAtom::NAtoms];
    const char *ptr = xcb_atomnames;

    xcb_connection_t* conn = getXcbConnection();

    int i = 0;
    while (*ptr) {
        names[i++] = ptr;
        while (*ptr)
            ++ptr;
        ++ptr;
    }

    Q_ASSERT(i == QXcbHIMAtom::NAtoms);

    xcb_intern_atom_cookie_t cookies[QXcbHIMAtom::NAtoms];

    Q_ASSERT(i == QXcbHIMAtom::NAtoms);
    for (i = 0; i < QXcbHIMAtom::NAtoms; ++i)
        cookies[i] = xcb_intern_atom(conn, false, strlen(names[i]), names[i]);

    for (i = 0; i < QXcbHIMAtom::NAtoms; ++i) {
        xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(conn, cookies[i], 0);
        m_allAtoms[i] = reply->atom;
        free(reply);
    }
}


#if 0
static xcb_screen_t *screen_of_display (xcb_connection_t *c, int screen)
{
	xcb_screen_iterator_t iter;

	iter = xcb_setup_roots_iterator (xcb_get_setup (c));
	for (; iter.rem; --screen, xcb_screen_next (&iter))
		if (screen == 0)
			return iter.data;

	return NULL;
}
#endif

void MyXcbEventFilter::setFocusObject(QObject *object) {
	std::cerr << "MyXcbEventFilter::setFocusObject" << std::endl;

	if (object) {
		std::cerr << "MyXcbEventFilter::setFocusObject - sendHildonCommand" << std::endl;
		sendHildonCommand(HILDON_IM_SETCLIENT, (QWidget*)object);
	}
}

xcb_window_t MyXcbEventFilter::findHildonIm()
{
    xcb_window_t result = 0;

    xcb_connection_t* conn = getXcbConnection();

#if 0
    // This is kind of quirky
    int screen_no = QX11Info::appScreen();
    xcb_screen_t  * screen = screen_of_display (conn, screen_no);
    xcb_window_t root_window = 0;
    if (screen)
        root_window = screen->root;
#endif
    xcb_window_t root_window = (xcb_window_t)QX11Info::appRootWindow(-1);

    xcb_get_property_cookie_t prop_cookie = xcb_get_property(conn, 0, root_window, atom(QXcbHIMAtom::_HILDON_IM_WINDOW), XCB_GET_PROPERTY_TYPE_ANY, 0, 1);
    xcb_get_property_reply_t* prop_reply = xcb_get_property_reply(conn, prop_cookie, NULL);

    if (!prop_reply)
        return result;

    xcb_window_t *prop_ret = NULL;

    if ((prop_reply->value_len > 0) && (prop_reply->type == XCB_ATOM_WINDOW) && (prop_reply->format == HILDON_IM_WINDOW_ID_FORMAT)) {
        int length = xcb_get_property_value_length (prop_reply);
        prop_ret = (xcb_window_t*)malloc(length);
        if (prop_ret) {
            memcpy(prop_ret, xcb_get_property_value(prop_reply), length);
            result = *prop_ret;

            std::cerr << "Window ID:" << result << std::endl;
        }
    }

    free(prop_reply);

    if (result == 0) {
        // XXX: QHildonInputContext is old name/str
        qWarning("QHildonInputContext: Unable to get the Hildon IM window id");
    }

	return result;
}

void MyXcbEventFilter::sendHildonCommand(HildonIMCommand cmd, QWidget *widget) {
    xcb_window_t hildon_im_window = findHildonIm();
    xcb_connection_t* conn = getXcbConnection();


    xcb_client_message_event_t * event = (xcb_client_message_event_t*)calloc(32, 1);
    if (event == NULL) {
        qWarning("sendHildonCommand: could not allocate memory to send event");
        return;
    }

    event->response_type = XCB_CLIENT_MESSAGE;
    event->format = HILDON_IM_ACTIVATE_FORMAT;
    event->sequence = 0;
    event->window = hildon_im_window;
    event->type = atom(QXcbHIMAtom::_HILDON_IM_ACTIVATE);

    HildonIMActivateMessage *msg = reinterpret_cast<HildonIMActivateMessage *>(&event->data);

	if (widget) {
		msg->input_window = QHIMProxyWidget::proxyFor(widget)->winId();
		msg->app_window = widget->window()->winId();
	} else if (cmd != HILDON_IM_HIDE) {
		qWarning() << "Invalid Hildon Command:" << cmd;
		return;
	}

	// TODO
	//if (cmd == HILDON_IM_SETCLIENT || cmd == HILDON_IM_SETNSHOW)
	//	sendInputMode();

	msg->cmd = cmd;
	msg->trigger = HILDON_IM_TRIGGER_KEYBOARD; // XXX
	//msg->trigger = triggerMode;

	std::cerr << "Sending event!" << std::endl;
    xcb_send_event(conn, 0, hildon_im_window, 0, (const char*)event);

    free(event);
}


// See /usr/include/xcb/xproto.h for xcb events

bool MyXcbEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
	bool ours = false;

    if (eventType == "xcb_generic_event_t") {
        xcb_generic_event_t* ev = static_cast<xcb_generic_event_t *>(message);
        if ((ev->response_type & ~0x80) == XCB_CLIENT_MESSAGE) {
            std::cerr << "Got XCB Client Message:" << std::endl;

            // Grab atom name
            xcb_client_message_event_t* message_ev = (xcb_client_message_event_t *)ev;

            for (int i = 0; i < QXcbHIMAtom::NAtoms; i++) {
                if (message_ev->type == m_allAtoms[i]) {
                    std::cerr << "Got known atom" << std::endl;
					ours = true;
                }
            }

            xcb_connection_t* realconn = getXcbConnection();

            xcb_get_atom_name_cookie_t name_cookie = xcb_get_atom_name(realconn, message_ev->type);
            xcb_get_atom_name_reply_t *name_reply = xcb_get_atom_name_reply(realconn, name_cookie, NULL);
            int length = xcb_get_atom_name_name_length(name_reply);
            char *atom_name = (char*)malloc(length + 1);
            strncpy(atom_name, xcb_get_atom_name_name(name_reply), length);
            atom_name[length] = '\0';

            std::cerr << atom_name << std::endl;

            free(atom_name);
            free(name_reply);
        }
        // ...
    }
    return ours;
}

xcb_connection_t* MyXcbEventFilter::getXcbConnection(void) {
    // XXX: Maybe just use https://doc.qt.io/qt-5/qx11info.html#connection

    // Snatch QXcb's xcb connection
    QPlatformNativeInterface *platformNativeInterface = QGuiApplication::platformNativeInterface();
    QXcbIntegration* xcbplatform = (QXcbIntegration*)platformNativeInterface;
    xcbplatform = xcbplatform->instance();
    QXcbConnection* conn = xcbplatform->defaultConnection();
    xcb_connection_t* realconn = conn->xcb_connection();
    return realconn;
}



QMap<QWidget *, QHIMProxyWidget *> QHIMProxyWidget::proxies;

QHIMProxyWidget::QHIMProxyWidget(QWidget *widget)
    : QWidget(0), w(widget)
{
    setAttribute(Qt::WA_InputMethodEnabled);
    setAttribute(Qt::WA_NativeWindow);
    createWinId();
    connect(w, SIGNAL(destroyed()), this, SLOT(widgetWasDestroyed()));
}

QHIMProxyWidget::~QHIMProxyWidget()
{
}

QWidget *QHIMProxyWidget::widget() const
{
    return w;
}

QHIMProxyWidget *QHIMProxyWidget::proxyFor(QWidget *w)
{
    QHIMProxyWidget *proxy = qobject_cast<QHIMProxyWidget *>(w);

    if (!proxy)
        proxy = proxies.value(w);

    if (!proxy) {
        proxy = new QHIMProxyWidget(w);
        proxies.insert(w, proxy);
    }
    //qWarning() << "Using HIM Proxy widget" << proxy << "for widget" << w << "isnative: " << proxy->testAttribute(Qt::WA_NativeWindow) << " / " << w->testAttribute(Qt::WA_NativeWindow);

    return proxy;
}

void QHIMProxyWidget::widgetWasDestroyed()
{
    proxies.remove(w);
    delete this;
}

