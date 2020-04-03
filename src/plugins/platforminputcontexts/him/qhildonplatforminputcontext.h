#ifndef QHILDONPLATFORMINPUTCONTEXT_H
#define QHILDONPLATFORMINPUTCONTEXT_H

#include <qpa/qplatforminputcontext.h>

#include "qhildonplatforminputeventfilter.h"

#include <QtCore/QList>
#include <QWidget>

QT_BEGIN_NAMESPACE


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

protected:

private:
    MyXcbEventFilter *eventFilter;
};


QT_END_NAMESPACE

#endif // QHILDONPLATFORMINPUTCONTEXT_H
