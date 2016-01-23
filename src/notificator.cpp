#include "notificator.h"

Notificator::Notificator(QObject *parent) :
    QObject(parent),
    m_notifications()
{

}

Notificator::~Notificator() {
    for (auto notification = m_notifications.cbegin(); notification != m_notifications.cend(); ++notification)
        (*notification)->close();
    qDeleteAll(m_notifications);
}

void Notificator::addNotification(QString origin, QString summary, QString body) {
    Notification* notification = new Notification();
    notification->setAppName("Pass Viewer");
    notification->setAppIcon("harbour-passviewer");
    notification->setOrigin(origin);
    notification->setSummary(summary);
    notification->setBody(body);
    // in this app, we want the notifications permanently until they become irrelevant or the app closes
    notification->setExpireTimeout(0);
    notification->setHintValue("resident", true);
    // this is necessary for click signals to be created
    notification->setRemoteActions(QVariantList({Notification::remoteAction("default", "", "dummy", "/dummy", "dummy", "dummy")}));
    // check if we're replaceing an already existing notification
    for (auto oldNotification = m_notifications.begin(); oldNotification != m_notifications.end(); ++oldNotification) {
        if ((*oldNotification)->origin() == origin) {
            notification->setReplacesId((*oldNotification)->replacesId());
            delete *oldNotification;
            m_notifications.erase(oldNotification);
            break;
        }
    }
    notification->publish();
    m_notifications.append(notification);
    connect(notification, &Notification::clicked, this, &Notificator::clicked);
}

void Notificator::removeNotification(QString origin) {
    for (auto notification = m_notifications.begin(); notification != m_notifications.end();) {
        if ((*notification)->origin() == origin) {
            (*notification)->close();
            delete *notification;
            notification = m_notifications.erase(notification);
        }
        else {
            ++notification;
        }
    }
}

void Notificator::bannerNotification(QString summary, QString body) {
    Notification notification;
    notification.setAppName("Pass Viewer");
    notification.setPreviewSummary(summary);
    notification.setPreviewBody(body);
    notification.setHintValue("transient", true);
    notification.publish();
}

void Notificator::clicked() {
    Notification* notification = dynamic_cast<Notification*>(sender());
    if (notification != NULL)
        emit notificationClicked(notification->origin());
}
