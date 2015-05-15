#ifndef NOTIFICATOR_H
#define NOTIFICATOR_H

#include <QObject>
#include <QtDBus/QDBusConnection>
#include <QStringList>
#include "notificationlist.h"
#include "notificationsproxy.h"

class Notificator : public QObject
{
    Q_OBJECT
public:
    explicit Notificator(QObject *parent = 0);
    ~Notificator();

    Q_INVOKABLE void addNotification(QString path, QString text);

signals:
    void notificationClicked(QString path);

public slots:
    void handleAction(uint id, const QString &action_key);

private:
    org::freedesktop::Notifications m_notifications;
    QMap<QString, uint> m_entries;

    void clearNotifications();

};

#endif // NOTIFICATOR_H
