#include "homescanner.h"

HomeScanner::HomeScanner(QObject *parent) : QObject(parent)
{

}

void HomeScanner::scanHome(bool update) {
    QStringList inspectPaths;
    QStringList visiblePaths;
    QStringList passPaths;
    QList<QObject*> passes;
    inspectPaths.append(QDir::homePath());  // start with the home directory
    if (QDir("/media/sdcard").exists())
        inspectPaths.append("/media/sdcard");  // also check the SD-Card
    while (!inspectPaths.isEmpty()) {
        QDir current(inspectPaths.at(0));
        inspectPaths.removeFirst();
        visiblePaths.append(current.path());
        QFileInfoList entries(current.entryInfoList());
        for (auto entry = entries.cbegin(); entry != entries.cend(); ++entry) {
            if (entry->isHidden())  // we don't look at hidden directories or files
                continue;
            if (entry->isDir()) {
                QString dpath(entry->fileName().toLower());
                if (dpath == "tmp" || dpath == "temp")  // ignore temporary directories
                    continue;
                dpath = entry->canonicalFilePath();
                if (inspectPaths.contains(dpath) || visiblePaths.contains(dpath))  // avoid double checks
                    continue;
                inspectPaths.append(dpath);
            }
            if (entry->isFile()) {
                if (entry->suffix() != "pkpass")  // look for pass files only
                    continue;
                QString fpath(entry->canonicalFilePath());
                if (passPaths.contains(fpath))  // avoid double listing
                    continue;
                ZipFile zip(fpath);
                if (!zip.isValid())
                    continue;
                passPaths.append(fpath);
                Pass* pass = new Pass(fpath, zip.getTextFile("pass.json"));
                if (pass->isValid()) {
                    passes.append(pass);
                    // make sure these are handled by the QML GC in the end
                    QQmlEngine::setObjectOwnership(pass, QQmlEngine::JavaScriptOwnership);
                }
                else {
                    delete pass;
                }
            }
        }
    }
    emit passesFound(passes, visiblePaths, update);
}
