#include <iostream>

#include <xcb/xcb.h>

#include <QtCore/QCoreApplication>
#include <QGuiApplication>
#include <QAbstractNativeEventFilter>

#include <qpa/qplatformnativeinterface.h>

#include "qhildonplatforminputeventfilter.h"

#if 0
    // Hildon Input Method
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
#endif


// See /usr/include/xcb/xproto.h for xcb events

bool MyXcbEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    if (eventType == "xcb_generic_event_t") {
        xcb_generic_event_t* ev = static_cast<xcb_generic_event_t *>(message);
        if ((ev->response_type & ~0x80) == XCB_CLIENT_MESSAGE) {
            std::cerr << "Got XCB Client Message:" << std::endl;

            // Snatch QXcb's xcb connection
            QPlatformNativeInterface *platformNativeInterface = QGuiApplication::platformNativeInterface();
            QXcbIntegration* xcbplatform = (QXcbIntegration*)platformNativeInterface;
            xcbplatform = xcbplatform->instance();
            QXcbConnection* conn = xcbplatform->defaultConnection();
            xcb_connection_t* realconn = conn->xcb_connection();

            // Grab atom name
            xcb_client_message_event_t* message_ev = (xcb_client_message_event_t *)ev;

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
    return false;
}
