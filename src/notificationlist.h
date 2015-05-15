#ifndef NOTIFICATIONLIST_H
#define NOTIFICATIONLIST_H

#include <QtDBus/QDBusArgument>
#include <QString>
#include <QStringList>
#include <QVariantMap>

struct NotificationInfo
{
    QString app_name;
    uint uid;
    QString icon;
    QString summary;
    QString body;
    QStringList actions;
    QVariantMap hints;
    int expires;
};
Q_DECLARE_METATYPE(NotificationInfo)

QDBusArgument &operator<<(QDBusArgument &argument, const NotificationInfo &notificationInfo);
const QDBusArgument &operator>>(const QDBusArgument &argument, NotificationInfo &notificationInfo);

typedef QList<NotificationInfo> NotificationList;
Q_DECLARE_METATYPE(NotificationList)

QDBusArgument &operator<<(QDBusArgument &argument, const NotificationList &notificationList);
const QDBusArgument &operator>>(const QDBusArgument &argument, NotificationList &notificationList);

#endif // NOTIFICATIONLIST_H
