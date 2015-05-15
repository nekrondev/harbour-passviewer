#include "notificator.h"

Notificator::Notificator(QObject *parent) :
    QObject(parent),
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
    QDBusPendingReply<uint> uidReply = m_notifications.Notify("harbour-passviewer", 0, "", text, "", actions, hints, 0);
    uidReply.waitForFinished();
    if (!uidReply.isError())
        m_entries.insert(path, uidReply.value());
}

void Notificator::handleAction(uint id, const QString &action_key) {
    for (int key = 0; key < m_entries.keys().size(); key++) {
        QString path = m_entries.keys().at(key);
        if (m_entries.value(path) == id)
            notificationClicked(path);
    }
}

void Notificator::clearNotifications() {
    QDBusPendingReply<NotificationList> listReply = m_notifications.GetNotifications("harbour-passviewer");
    listReply.waitForFinished();
    if (listReply.isValid()) {
        for (int entry = 0; entry < listReply.value().size(); entry++)
            m_notifications.CloseNotification(listReply.value().at(entry).uid);
    }
}
