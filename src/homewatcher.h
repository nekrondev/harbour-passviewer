#ifndef HOMEWATCHER_H
#define HOMEWATCHER_H

#include <QObject>
#include <QFileSystemWatcher>
#include <QList>
#include <QStringList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QThread>

#include "homescanner.h"
#include "pass.h"
#include "passdb.h"

class HomeWatcher : public QFileSystemWatcher
{
    Q_OBJECT
public:
    explicit HomeWatcher(QObject *parent = 0);
    ~HomeWatcher();

    Q_INVOKABLE void updatePaths(const QStringList &paths);

signals:
    void workerScanHome(bool update);
    void passesFound(QList<QObject*> list, bool update);

public slots:
    void scanHome(bool update = false);
    void passesFoundByWorker(QList<QObject*> list, QStringList paths, bool update);

private:
    QThread m_worker;
};

#endif // HOMEWATCHER_H
