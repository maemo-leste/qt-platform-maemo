#ifndef QHILDONPLATFORMINPUTCONTEXT_H
#define QHILDONPLATFORMINPUTCONTEXT_H

#include <qpa/qplatforminputcontext.h>

#include <QtCore/QList>

QT_BEGIN_NAMESPACE

class QEvent;

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
};

QT_END_NAMESPACE

#endif // QHILDONPLATFORMINPUTCONTEXT_H
