#include <iostream>

#include "qhildonplatforminputeventfilter.h"


MyXcbEventFilter::MyXcbEventFilter(QHildonInputContext* himctx)
{
    this->himctx = himctx;
}

MyXcbEventFilter::~MyXcbEventFilter()
{
}


// See /usr/include/xcb/xproto.h for xcb events
bool MyXcbEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
	bool ours = false;
    bool filter = false;

    if (eventType == "xcb_generic_event_t") {
        xcb_generic_event_t* ev = static_cast<xcb_generic_event_t *>(message);
        if ((ev->response_type & ~0x80) == XCB_CLIENT_MESSAGE) {
            // Grab atom name
            xcb_client_message_event_t* message_ev = (xcb_client_message_event_t *)ev;

            for (int i = 0; i < QXcbHIMAtom::NAtoms; i++) {
                if (message_ev->type == himctx->atom((QXcbHIMAtom::Atom)i)) {
					ours = true;
                }
            }
            // TODO: Maybe only call x11FilterEvent if ours == true, but needs
            // to be checked
            filter = ours || himctx->x11FilterEvent(message_ev);
        }
    }
    return filter;
}


