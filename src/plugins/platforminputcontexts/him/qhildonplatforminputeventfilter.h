#ifndef QHILDONPLATFORMINPUTEVENTFILTER_H
#define QHILDONPLATFORMINPUTEVENTFILTER_H

#include <QAbstractNativeEventFilter>

#include "qxcbintegration.h"
#include "qxcbconnection.h"

class MyXcbEventFilter : public QAbstractNativeEventFilter
{
public:
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;
};


#endif // QHILDONPLATFORMINPUTEVENTFILTER_H
