#include "notificationlist.h"

QDBusArgument &operator<<(QDBusArgument &argument, const NotificationInfo &notificationInfo) {
    argument.beginStructure();
    argument << notificationInfo.app_name << notificationInfo.uid << notificationInfo.icon << notificationInfo.summary << notificationInfo.body << notificationInfo.actions << notificationInfo.hints << notificationInfo.expires;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NotificationInfo &notificationInfo) {
    argument.beginStructure();
    argument >> notificationInfo.app_name >> notificationInfo.uid >> notificationInfo.icon >> notificationInfo.summary >> notificationInfo.body >> notificationInfo.actions >> notificationInfo.hints >> notificationInfo.expires;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NotificationList &notificationList) {
    argument.beginArray(qMetaTypeId<NotificationInfo>());
    for (int entry = 0; entry < notificationList.length(); entry++)
        argument << notificationList.at(entry);
    argument.endArray();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NotificationList &notificationList) {
    argument.beginArray();
    while (!argument.atEnd()) {
        NotificationInfo info;
        argument >> info;
        notificationList.append(info);
    }
    argument.endArray();
    return argument;
}
