#include <iostream>

#include <xcb/xcb.h>

#include <QtCore/QCoreApplication>
#include <QGuiApplication>
#include <QAbstractNativeEventFilter>
#include <QtWidgets/QApplication>

#include <qpa/qplatformnativeinterface.h>

#include <QX11Info> // XXX: maybe deprecate this

//#include <hildon-input-method/hildon-im-protocol.h>

#include "qhildonplatforminputeventfilter.h"


// TODO
static const char *debugNameForCommunicationId(HildonIMCommunication id);

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

    // XXX HACK
    QWidget *widget = qobject_cast<QWidget *>(object);
    if (widget) {
        std::cerr << "QApplication::focusWidget() objectName: " << widget->objectName().toStdString() << std::endl;
        std::cerr << "QApplication::focusWidget() isWidgetType: " << widget->isWidgetType() << std::endl;
        std::cerr << "QApplication::focusWidget() isWindowType: " << widget->isWindowType() << std::endl;
    } else {
        std::cerr << "QApplication::focusWidget() NULL" << std::endl;
    }

	if (widget) {
		std::cerr << "MyXcbEventFilter::setFocusObject - sendHildonCommand" << std::endl;
		sendHildonCommand(HILDON_IM_SETCLIENT, widget);
		//sendHildonCommand(HILDON_IM_SETNSHOW, widget);
        sendHildonCommand(HILDON_IM_SHIFT_UNSTICKY, widget);
        sendHildonCommand(HILDON_IM_MOD_UNSTICKY, widget);
        sendInputMode();
		sendHildonCommand(HILDON_IM_CLEAR, widget);
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

            //std::cerr << "Window ID:" << result << std::endl;
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
	//std::cerr << "sendHildonCommand" << std::endl;
    xcb_window_t hildon_im_window = findHildonIm();
    xcb_connection_t* conn = getXcbConnection();

	//std::cerr << "sendHildonCommand window:" << hildon_im_window << std::endl;

    xcb_client_message_event_t * event = (xcb_client_message_event_t*)calloc(32, 1);
    if (event == NULL) {
        qWarning("sendHildonCommand: could not allocate memory to send event");
        return;
    }

    event->response_type = XCB_CLIENT_MESSAGE;
    event->format = HILDON_IM_ACTIVATE_FORMAT;
    event->sequence = 0; // XXX: required? we already do calloc
    event->window = hildon_im_window;
    event->type = atom(QXcbHIMAtom::_HILDON_IM_ACTIVATE);

    HildonIMActivateMessage *msg = reinterpret_cast<HildonIMActivateMessage *>(&event->data.data8[0]);

	if (widget) {
		msg->input_window = widget->window()->winId();
		//msg->input_window = QHIMProxyWidget::proxyFor(widget)->winId();
		msg->app_window = widget->window()->winId();
        std::cerr << "**** msg->input_window: " << msg->input_window << std::endl;
        std::cerr << "**** msg->app_window: " << msg->app_window<< std::endl;
	} else if (cmd != HILDON_IM_HIDE) {
		qWarning() << "Invalid Hildon Command:" << cmd;
		return;
	}

	//// TODO
	if (cmd == HILDON_IM_SETCLIENT || cmd == HILDON_IM_SETNSHOW)
		sendInputMode();

	msg->cmd = cmd;
	msg->trigger = HILDON_IM_TRIGGER_KEYBOARD; // XXX
	//msg->trigger = triggerMode;

	//std::cerr << "Sending event!" << std::endl;
    xcb_send_event(conn, 0, hildon_im_window, 0, (const char*)event);

	xcb_flush(conn); // XXX: maybe remove

    free(event);
}

void MyXcbEventFilter::sendInputMode()
{
	//std::cerr << "sendInputMode" << std::endl;

    //qHimDebug() << "HIM: sendInputMode";
    xcb_window_t hildon_im_window = findHildonIm();
    xcb_connection_t* conn = getXcbConnection();

    xcb_client_message_event_t * event = (xcb_client_message_event_t*)calloc(32, 1);
    if (event == NULL) {
        qWarning("sendInputMode: could not allocate memory to send event");
        return;
    }

	event->response_type = XCB_CLIENT_MESSAGE;
	event->format = HILDON_IM_INPUT_MODE_FORMAT;
	event->window = hildon_im_window;
    event->type = atom(QXcbHIMAtom::_HILDON_IM_INPUT_MODE);

    HildonIMInputModeMessage *msg = reinterpret_cast<HildonIMInputModeMessage *>(&event->data.data8[0]);

    //msg->input_mode = static_cast<HildonGtkInputMode>(inputMode);
    msg->input_mode = static_cast<HildonGtkInputMode>(HILDON_GTK_INPUT_MODE_FULL); // XXX: keep track of our current mode rather than defaulting to this ^^

    msg->default_input_mode = static_cast<HildonGtkInputMode>(HILDON_GTK_INPUT_MODE_FULL);

	//std::cerr << "Sending event!" << std::endl;
    xcb_send_event(conn, 0, hildon_im_window, 0, (const char*)event);

	xcb_flush(conn); // XXX: maybe remove

    free(event);
}


// See /usr/include/xcb/xproto.h for xcb events

bool MyXcbEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
	bool ours = false;

    if (eventType == "xcb_generic_event_t") {
        xcb_generic_event_t* ev = static_cast<xcb_generic_event_t *>(message);
        if ((ev->response_type & ~0x80) == XCB_CLIENT_MESSAGE) {
            //std::cerr << "Got XCB Client Message:" << std::endl;

            // Grab atom name
            xcb_client_message_event_t* message_ev = (xcb_client_message_event_t *)ev;

            for (int i = 0; i < QXcbHIMAtom::NAtoms; i++) {
                if (message_ev->type == m_allAtoms[i]) {
                    //std::cerr << "Got known atom" << std::endl;
					ours = true;
                }
            }

            xcb_connection_t* realconn = getXcbConnection();

            xcb_get_atom_name_cookie_t name_cookie = xcb_get_atom_name(realconn, message_ev->type);
            xcb_get_atom_name_reply_t *name_reply = xcb_get_atom_name_reply(realconn, name_cookie, NULL);
            int length = xcb_get_atom_name_name_length(name_reply);
            //std::cerr << "Atom name length: " << length << std::endl;
            char *atom_name = (char*)malloc(length + 1);
            strncpy(atom_name, xcb_get_atom_name_name(name_reply), length);
            atom_name[length] = '\0';

            //std::cerr << "Atom name: " << atom_name << std::endl;

            free(atom_name);
            free(name_reply);

            ours |= x11FilterEvent(message_ev);
        }
        // ...
    }
    return ours;
}

bool MyXcbEventFilter::x11FilterEvent(xcb_client_message_event_t *event)
//bool MyXcbEventFilter::x11FilterEvent(QWidget *keywidget, xcb_client_message_event *event)
{
    //if (QHIMProxyWidget *proxy = qobject_cast<QHIMProxyWidget *>(keywidget))
    //    keywidget = proxy->widget();

    if (event->type == atom(QXcbHIMAtom::_HILDON_IM_INSERT_UTF8) &&
        event->format == HILDON_IM_INSERT_UTF8_FORMAT) {
        std::cerr << "HIM: x11FilterEvent( HILDON_IM_INSERT_UTF8_FORMAT )" << std::endl;
        //qHimDebug() << "HIM: x11FilterEvent( HILDON_IM_INSERT_UTF8_FORMAT )";

        HildonIMInsertUtf8Message *msg = reinterpret_cast<HildonIMInsertUtf8Message *>(&event->data);
        std::cerr << "Got text: " << msg->utf8_str << std::endl;
        //insertUtf8(msg->msg_flag, QString::fromUtf8(msg->utf8_str));
        return true;
    } else if (event->type == atom(QXcbHIMAtom::_HILDON_IM_COM)) {
        HildonIMComMessage *msg = (HildonIMComMessage *)&event->data;
        //options = msg->options;

        std::cerr << "HIM: x11FilterEvent( _HILDON_IM_COM /" << debugNameForCommunicationId(msg->type) << ")" << std::endl;

#if 0
        switch (msg->type) {
        //Handle Keys msgs
        case HILDON_IM_CONTEXT_HANDLE_ENTER:
            sendKey(keywidget, Qt::Key_Enter);
            return true;
        case HILDON_IM_CONTEXT_HANDLE_TAB:
            sendKey(keywidget, Qt::Key_Tab);
            return true;
        case HILDON_IM_CONTEXT_HANDLE_BACKSPACE:
            sendKey(keywidget, Qt::Key_Backspace);
            return true;
        case HILDON_IM_CONTEXT_HANDLE_SPACE:
            insertUtf8(HILDON_IM_MSG_CONTINUE, QChar(Qt::Key_Space));
            commitPreeditBuffer();
            return true;

        //Handle Clipboard msgs
        case HILDON_IM_CONTEXT_CLIPBOARD_SELECTION_QUERY:
            answerClipboardSelectionQuery(keywidget);
            return true;
        case HILDON_IM_CONTEXT_CLIPBOARD_PASTE:
            if (QClipboard *clipboard = QApplication::clipboard()) {
                QInputMethodEvent e;
                e.setCommitString(clipboard->text());
                QApplication::sendEvent(keywidget, &e);
            }
            return true;
        case HILDON_IM_CONTEXT_CLIPBOARD_COPY:
            if (QClipboard *clipboard = QApplication::clipboard())
                clipboard->setText(keywidget->inputMethodQuery(Qt::ImCurrentSelection).toString());
            return true;
        case HILDON_IM_CONTEXT_CLIPBOARD_CUT:
            if (QClipboard *clipboard = QApplication::clipboard()) {
                clipboard->setText(keywidget->inputMethodQuery(Qt::ImCurrentSelection).toString());
                QInputMethodEvent ev;
                QApplication::sendEvent(keywidget, &ev);
            }
            return true;

        //Handle commit mode msgs
        case HILDON_IM_CONTEXT_DIRECT_MODE:
            setCommitMode(HILDON_IM_COMMIT_DIRECT);
            return true;
        case HILDON_IM_CONTEXT_BUFFERED_MODE:
            setCommitMode(HILDON_IM_COMMIT_BUFFERED);
            return true;
        case HILDON_IM_CONTEXT_REDIRECT_MODE:
            setCommitMode(HILDON_IM_COMMIT_REDIRECT);
            clearSelection();
            return true;
        case HILDON_IM_CONTEXT_SURROUNDING_MODE:
            setCommitMode(HILDON_IM_COMMIT_SURROUNDING);
            return true;
        case HILDON_IM_CONTEXT_PREEDIT_MODE:
            setCommitMode(HILDON_IM_COMMIT_PREEDIT);
            return true;

        //Handle context
        case HILDON_IM_CONTEXT_CONFIRM_SENTENCE_START:
            checkSentenceStart();
            return true;
        case HILDON_IM_CONTEXT_FLUSH_PREEDIT:
            commitPreeditBuffer();
            return true;
        case HILDON_IM_CONTEXT_REQUEST_SURROUNDING:
            sendSurrounding(false);
            return true;
        case HILDON_IM_CONTEXT_LEVEL_UNSTICKY:
            mask &= ~(HILDON_IM_LEVEL_STICKY_MASK | HILDON_IM_LEVEL_LOCK_MASK);
            return true;
        case HILDON_IM_CONTEXT_SHIFT_UNSTICKY:
            mask &= ~(HILDON_IM_SHIFT_STICKY_MASK | HILDON_IM_SHIFT_LOCK_MASK);
            return true;
        case HILDON_IM_CONTEXT_CANCEL_PREEDIT:
            cancelPreedit();
            return true;
        case HILDON_IM_CONTEXT_REQUEST_SURROUNDING_FULL:
            sendSurrounding(true);
            return true;
        case HILDON_IM_CONTEXT_SPACE_AFTER_COMMIT:
        case HILDON_IM_CONTEXT_NO_SPACE_AFTER_COMMIT:
            spaceAfterCommit = (msg->type == HILDON_IM_CONTEXT_SPACE_AFTER_COMMIT);
            return true;

        case HILDON_IM_CONTEXT_WIDGET_CHANGED:
        case HILDON_IM_CONTEXT_ENTER_ON_FOCUS:
        case HILDON_IM_CONTEXT_SHIFT_LOCKED:
        case HILDON_IM_CONTEXT_SHIFT_UNLOCKED:
        case HILDON_IM_CONTEXT_LEVEL_LOCKED:
        case HILDON_IM_CONTEXT_LEVEL_UNLOCKED:
            // ignore
            return true;

        default:
            qWarning() << "HIM: x11FilterEvent( _HILDON_IM_COM /" << debugNameForCommunicationId(msg->type) << ") was not handled.";
            break;
        }
#endif
    } else if (event->type == atom(QXcbHIMAtom::_HILDON_IM_SURROUNDING_CONTENT) &&
               event->format == HILDON_IM_SURROUNDING_CONTENT_FORMAT) {
        std::cerr << "HIM: x11FilterEvent( _HILDON_IM_SURROUNDING_CONTENT ) is not supported" << std::endl;
    } else if (event->type == atom(QXcbHIMAtom::_HILDON_IM_SURROUNDING) &&
               event->format == HILDON_IM_SURROUNDING_FORMAT) {
        std::cerr << "HIM: x11FilterEvent( _HILDON_IM_SURROUNDING )" << std::endl;

        HildonIMSurroundingMessage *msg = reinterpret_cast<HildonIMSurroundingMessage*>(&event->data);
        //setClientCursorLocation(msg->offset_is_relative, msg->cursor_offset );
        return true;
    } else if (event->type == atom(QXcbHIMAtom::_HILDON_IM_LONG_PRESS_SETTINGS)) {
        std::cerr << "HIM: x11FilterEvent( _HILDON_IM_LONG_PRESS_SETTINGS )" << std::endl;

        HildonIMLongPressSettingsMessage *msg = reinterpret_cast<HildonIMLongPressSettingsMessage *>(&event->data);
        //if (msg->enable_long_press) {
        //    longPressTimer->setInterval((msg->long_press_timeout > 0) ? msg->long_press_timeout : DEFAULT_LONG_PRESS_TIMEOUT);
        //} else {
        //    longPressTimer->stop();
        //    longPressTimer->setInterval(0);
        //}
        return true;
    }
    return false;
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

static const char *debugNameForCommunicationId(HildonIMCommunication id)
{
//#ifdef HIM_DEBUG
    static const char *const mapping[] = {
        "HILDON_IM_CONTEXT_HANDLE_ENTER",
        "HILDON_IM_CONTEXT_HANDLE_TAB",
        "HILDON_IM_CONTEXT_HANDLE_BACKSPACE",
        "HILDON_IM_CONTEXT_HANDLE_SPACE",
        "HILDON_IM_CONTEXT_CONFIRM_SENTENCE_START",
        "HILDON_IM_CONTEXT_FLUSH_PREEDIT",
        "HILDON_IM_CONTEXT_CANCEL_PREEDIT",
        "HILDON_IM_CONTEXT_BUFFERED_MODE",
        "HILDON_IM_CONTEXT_DIRECT_MODE",
        "HILDON_IM_CONTEXT_REDIRECT_MODE",
        "HILDON_IM_CONTEXT_SURROUNDING_MODE",
        "HILDON_IM_CONTEXT_PREEDIT_MODE",
        "HILDON_IM_CONTEXT_CLIPBOARD_COPY",
        "HILDON_IM_CONTEXT_CLIPBOARD_CUT",
        "HILDON_IM_CONTEXT_CLIPBOARD_PASTE",
        "HILDON_IM_CONTEXT_CLIPBOARD_SELECTION_QUERY",
        "HILDON_IM_CONTEXT_REQUEST_SURROUNDING",
        "HILDON_IM_CONTEXT_REQUEST_SURROUNDING_FULL",
        "HILDON_IM_CONTEXT_WIDGET_CHANGED",
        "HILDON_IM_CONTEXT_OPTION_CHANGED",
        "HILDON_IM_CONTEXT_ENTER_ON_FOCUS",
        "HILDON_IM_CONTEXT_SPACE_AFTER_COMMIT",
        "HILDON_IM_CONTEXT_NO_SPACE_AFTER_COMMIT",
        "HILDON_IM_CONTEXT_SHIFT_LOCKED",
        "HILDON_IM_CONTEXT_SHIFT_UNLOCKED",
        "HILDON_IM_CONTEXT_LEVEL_LOCKED",
        "HILDON_IM_CONTEXT_LEVEL_UNLOCKED",
        "HILDON_IM_CONTEXT_SHIFT_UNSTICKY",
        "HILDON_IM_CONTEXT_LEVEL_UNSTICKY"
    };
    if (unsigned(id) < (sizeof(mapping) / sizeof(mapping[0]))) {
        return mapping[id];
    } else {
        static char name[] = "ID 00";
        name[3] = '0' + (id / 10);
        name[4] = '0' + (id % 10);
        return name;
    }
//#endif
//    return 0;
}
