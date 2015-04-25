#ifndef HOMEWATCHER_H
#define HOMEWATCHER_H

#include <QFileSystemWatcher>
#include <QStringList>

class HomeWatcher : public QFileSystemWatcher
{
    Q_OBJECT
public:
    explicit HomeWatcher(QObject *parent = 0);

    Q_INVOKABLE void updatePaths(const QStringList &paths);

signals:

public slots:

};

#endif // HOMEWATCHER_H
