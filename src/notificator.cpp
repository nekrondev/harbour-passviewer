#include "notificator.h"

Notificator::Notificator(QObject *parent) :
    QObject(parent),
    m_entries(),
    m_reverseEntries(),
    m_notifications("org.freedesktop.Notifications", "/org/freedesktop/Notifications", QDBusConnection::sessionBus())
{
    qDBusRegisterMetaType<NotificationInfo>();
    qDBusRegisterMetaType<NotificationList>();
    clearNotifications();
    connect(&m_notifications, &org::freedesktop::Notifications::ActionInvoked, this, &Notificator::handleAction);
}

Notificator::~Notificator() {
    clearNotifications();
}

void Notificator::addNotification(QString path, QString text) {
    QStringList actions;
    actions.append("default");
    actions.append("");
    QVariantHash hints;
    hints.insert("x-nemo-user-removable", false);
    hints.insert("x-nemo-icon", "harbour-passviewer");
    uint replaceUid = 0;
    if (m_entries.contains(path))
        replaceUid = m_entries.value(path);
    QDBusPendingReply<uint> uidReply = m_notifications.Notify("harbour-passviewer", replaceUid, "", text, "", actions, hints, 0);
    uidReply.waitForFinished();
    if (uidReply.isValid()) {
        m_entries[path] = uidReply.value();
        m_reverseEntries[uidReply.value()] = path;
    }
}

void Notificator::removeNotification(QString path) {
    m_notifications.CloseNotification(m_entries.value(path));
    m_reverseEntries.remove(m_entries.value(path));
    m_entries.remove(path);
}

void Notificator::handleAction(uint id, const QString &action_key) {
    notificationClicked(m_reverseEntries.value(id));
}

void Notificator::clearNotifications() {
    QDBusPendingReply<NotificationList> listReply = m_notifications.GetNotifications("harbour-passviewer");
    listReply.waitForFinished();
    if (listReply.isValid()) {
        for (int entry = 0; entry < listReply.value().size(); entry++)
            m_notifications.CloseNotification(listReply.value().at(entry).uid);
    }
    listReply = m_notifications.GetNotifications("");
    listReply.waitForFinished();
    if (listReply.isValid()) {
        for (int entry = 0; entry < listReply.value().size(); entry++)
            if (listReply.value().at(entry).app_name == "harbour-passviewer")
                m_notifications.CloseNotification(listReply.value().at(entry).uid);
    }
}

void Notificator::errorNotification(QString subject, QString detail) {
    QVariantHash hints;
    hints.insert("x-nemo-preview-summary", subject);
    hints.insert("x-nemo-preview-body", detail);
    m_notifications.Notify("", 0, "", "", "", QStringList(), hints, -1);
}
