#ifndef QHILDONPLATFORMINPUTEVENTFILTER_H
#define QHILDONPLATFORMINPUTEVENTFILTER_H

#include <QAbstractNativeEventFilter>

#include "qhildonplatforminputcontext.h"

class MyXcbEventFilter : public QAbstractNativeEventFilter
{
public:
    MyXcbEventFilter(QHildonInputContext* himctx);
    ~MyXcbEventFilter();

    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;
private:
    QHildonInputContext* himctx;
};

#endif // QHILDONPLATFORMINPUTEVENTFILTER_H
