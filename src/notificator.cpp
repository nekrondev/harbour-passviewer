#include "notificator.h"

Notificator::Notificator(QObject *parent) :
    QObject(parent),
    m_entries(),
    m_reverseEntries(),
    m_notifications("org.freedesktop.Notifications", "/org/freedesktop/Notifications", QDBusConnection::sessionBus())
{
    qDBusRegisterMetaType<NotificationInfo>();
    qDBusRegisterMetaType<NotificationList>();
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
    for (auto entry = m_entries.cbegin(); entry != m_entries.cend(); ++entry)
        m_notifications.CloseNotification(entry.value());
    m_entries.clear();
    m_reverseEntries.clear();
}

void Notificator::errorNotification(QString subject, QString detail) {
    QVariantHash hints;
    hints.insert("x-nemo-preview-summary", subject);
    hints.insert("x-nemo-preview-body", detail);
    m_notifications.Notify("", 0, "", "", "", QStringList(), hints, -1);
}
