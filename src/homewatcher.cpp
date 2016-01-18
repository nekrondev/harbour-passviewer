#include "homewatcher.h"

HomeWatcher::HomeWatcher(QObject *parent) :
    QFileSystemWatcher(parent),
    m_worker()
{
    HomeScanner* scanner = new HomeScanner;
    scanner->moveToThread(&m_worker);
    connect(&m_worker, &QThread::finished, scanner, &HomeScanner::deleteLater);
    connect(this, &HomeWatcher::workerScanHome, scanner, &HomeScanner::scanHome);
    connect(scanner, &HomeScanner::passesFound, this, &HomeWatcher::passesFoundByWorker);
    m_worker.start();
}

HomeWatcher::~HomeWatcher() {
    m_worker.quit();
    m_worker.wait();
}

void HomeWatcher::updatePaths(const QStringList &paths) {
    // clear the old list
    if (!directories().isEmpty())
        removePaths(directories());
    // add the new ones
    addPaths(paths);
}

void HomeWatcher::scanHome(bool update) {
    emit workerScanHome(update);
}

void HomeWatcher::passesFoundByWorker(QList<QObject *> list, QStringList paths, bool update) {
    PassDB db;
    QStringList oldids = db.getPassIDs();
    for (auto pass = list.cbegin(); pass != list.cend(); ++pass) {
        QJsonDocument json(QJsonDocument::fromJson(((Pass*)*pass)->jsondata().toUtf8()));
        oldids.removeAll(json.object().value("passTypeIdentifier").toString() + "/" + json.object().value("serialNumber").toString());
    }
    for (auto oldid = oldids.cbegin(); oldid != oldids.cend(); ++oldid) {
        db.deletePassInfo(*oldid);
    }
    updatePaths(paths);
    emit passesFound(list, update);
}
