#include "homescanner.h"

HomeScanner::HomeScanner(QObject *parent) : QObject(parent)
{

}

void HomeScanner::scanHome(bool update) {
    QStringList inspectPaths;
    QStringList visiblePaths;
    QStringList passPaths;
    QVariantList passes;
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
                QVariantMap pass = m_buildPass(fpath);
                if (pass.isEmpty())
                    continue;  // invalid pass
                passPaths.append(fpath);
                passes.append(pass);
            }
        }
    }
    emit passesFound(passes, visiblePaths, update);
}

void HomeScanner::scanHome(QString path) {
    scanHome(false);
}

QVariantMap HomeScanner::m_buildPass(QString zipname) {
    // get json data from the file
    ZipFile zip(zipname);
    if (!zip.isValid())
        return QVariantMap();
    QString jsondata = zip.getTextFile("pass.json");
    QJsonDocument json(QJsonDocument::fromJson(jsondata.toUtf8()));
    if (json.isNull())
        return QVariantMap();
    // get the type id
    QString typeId = json.object().value("passTypeIdentifier").toString();
    if (typeId == "")
        return QVariantMap();
    // get the pass style
    QStringList styles({"boardingPass", "coupon", "eventTicket", "storeCard", "generic"});
    QString passStyle;
    for (auto style = styles.cbegin(); style != styles.cend(); ++style) {
        if (json.object().contains(*style)) {
            passStyle = *style;
            break;
        }
    }
    if (passStyle == "")
        return QVariantMap();
    // create the pass name
    QString name;
    QJsonArray primaries(json.object().value(passStyle).toObject().value("primaryFields").toArray());
    if (passStyle == "boardingPass")  // use parting point and destination
        name = primaries.at(0).toObject().value("value").toString() + " → " + primaries.at(1).toObject().value("value").toString();
    else  // use the subject
        name = primaries.at(0).toObject().value("label").toString() + " " + primaries.at(0).toObject().value("value").toString();
    // use the file basename as backup
    if (name == "")
        name = QFileInfo(zipname).baseName();
    // check if the pass is updateable
    bool updateable = json.object().contains("webServiceURL") && json.object().contains("serialNumber") && json.object().contains("authenticationToken");
    // construct and return the pass
    return QVariantMap({{"name", name}, {"path", zipname}, {"jsondata", jsondata}, {"typeId", typeId}, {"updateable", updateable}});
}
