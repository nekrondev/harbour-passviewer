#include "homewatcher.h"

HomeWatcher::HomeWatcher(QObject *parent) :
    QFileSystemWatcher(parent)
{
}

void HomeWatcher::updatePaths(const QStringList &paths) {
    // clear the old list
    removePaths(directories());
    // add the new ones
    addPaths(paths);
}
