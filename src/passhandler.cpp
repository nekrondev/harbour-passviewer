#include "passhandler.h"

PassHandler::PassHandler(QObject *parent) :
    QObject(parent),
    m_network(),
    m_replies()
{
    QObject::connect(&m_network, &QNetworkAccessManager::finished, this, &PassHandler::replyFinished);
}

void PassHandler::removePass(QString path) {
    QFile(path).remove();
}

void PassHandler::updatePass(QString path) {
    ZipFile passFile(path);
    if (!passFile.isValid()) {
        emit updateFinished("not updateable");
        return;
    }
    QJsonDocument pass(QJsonDocument::fromJson(passFile.getTextFile("pass.json").toUtf8()));
    QString baseURL(pass.object().value("webServiceURL").toString());
    QString passID(pass.object().value("passTypeIdentifier").toString());
    QString serial(pass.object().value("serialNumber").toString());
    QString auth(pass.object().value("authenticationToken").toString());
    if (baseURL == "" || passID == "" || serial == "" || auth == "") {
        emit updateFinished("not updateable");
        return;
    }
    QNetworkRequest request(QUrl(baseURL + "/v1/passes/" + passID + "/" + serial));
    request.setRawHeader("Authorization", QByteArray("ApplePass ") + auth.toUtf8());
    PassDB db;
    PassInfo* info = db.getPassInfo(passID + "/" + serial);
    if (!info->updated().isNull())
        request.setRawHeader("If-Modified-Since", m_rfc2616(info->updated()));
    delete info;
    QNetworkReply* reply = m_network.get(request);
    m_replies.insert(reply, path);
}

void PassHandler::replyFinished(QNetworkReply *reply) {
    QString path(m_replies.value(reply));
    if (path == "")
        return;
    m_replies.remove(reply);
    reply->deleteLater();
    if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 304) {
        emit updateFinished("no new version");
        return;
    }
    if (reply->error() != QNetworkReply::NoError) {
        emit updateFinished("update failed");
        return;
    }
    QTemporaryFile tmp;
    QString id;
    if (tmp.open()) {
        m_copyFile(tmp, *reply);
        tmp.seek(0);
        ZipFile zip(tmp.fileName());
        if (!zip.isValid())
            tmp.close();
        QJsonDocument json(QJsonDocument::fromJson(zip.getTextFile("pass.json").toUtf8()));
        id = json.object().value("passTypeIdentifier").toString() + "/" + json.object().value("serialNumber").toString();
    }
    if (tmp.isOpen()) {
        QFile passFile(path);
        QStringList changes;
        if (passFile.exists())
            changes = getChanges(path, tmp.fileName());
        tmp.seek(0);
        if (passFile.open(QFile::WriteOnly)) {
            m_copyFile(passFile, tmp);
            passFile.close();
            PassDB db;
            PassInfo* info = new PassInfo(id, QDateTime::currentDateTime(), changes);
            db.setPassInfo(info);
            delete info;
            emit updateFinished("ok");
        }
        else {
            emit updateFinished("update failed");
        }
        tmp.close();
    }
    else {
        emit updateFinished("update failed");
    }
}

QMap<QString, QVariant> PassHandler::getFields(QString filename) {
    QMap<QString, QVariant> fields;
    ZipFile zip(filename);
    if (!zip.isValid())
        return fields;
    QJsonDocument pass(QJsonDocument::fromJson(zip.getTextFile("pass.json").toUtf8()));
    QStringList styles({"boardingPass", "coupon", "eventTicket", "storeCard", "generic"});
    QStringList types({"headerFields", "primaryFields", "secondaryFields", "auxiliaryFields"});
    for (auto style = styles.cbegin(); style != styles.cend(); ++style) {
        for (auto type = types.cbegin(); type != types.cend(); ++type) {
            QJsonArray thisFields(pass.object().value(*style).toObject().value(*type).toArray());
            for (int entry = 0; entry < thisFields.size(); entry++) {
                QJsonObject field(thisFields.at(entry).toObject());
                if (field.contains("key") && field.contains("value"))
                    fields.insert(field.value("key").toString(), field.value("value").toVariant());
            }
        }
    }
    return fields;
}

QStringList PassHandler::getChanges(QString oldfile, QString newfile) {
    QStringList changed;
    QMap<QString, QVariant> oldfields(getFields(oldfile));
    QMap<QString, QVariant> newfields(getFields(newfile));
    for (auto newfield = newfields.cbegin(); newfield != newfields.cend(); ++newfield) {
        if (!oldfields.contains(newfield.key()) || oldfields.value(newfield.key()) != newfield.value())
            changed.append(newfield.key());
    }
    return changed;
}

void PassHandler::createCalendarEntry(QString subject, QString isoDateTime) {
    QDateTime dateTime(QDateTime::fromString(isoDateTime, Qt::ISODate));
    if (!dateTime.isValid()) {
        emit calendarEntryFinished("format");
        return;
    }
    QString icaldir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    if (!QDir(icaldir).exists())
        QDir().mkpath(icaldir);
    QFile ical(icaldir + "/passbook.ics");
    if (ical.open(QFile::WriteOnly)) {
        QString w_subject(subject);
        ical.write("BEGIN:VCALENDAR\r\n");
        ical.write("VERSION:2.0\r\n");
        ical.write("PRODID:-//p2501.ch//Pass Viewer 0.10//EN\r\n");
        ical.write("BEGIN:VEVENT\r\n");
        ical.write("UID:" + w_subject.replace(' ', '_').toUtf8() + "/" + isoDateTime.toUtf8() + "/harbour-passviewer\r\n");
        ical.write("DTSTAMP:" + QDateTime::currentDateTimeUtc().toString("yyyyMMddTHHmmssZ").toUtf8() + "\r\n");
        ical.write("DTSTART:" + dateTime.toUTC().toString("yyyyMMddTHHmmssZ").toUtf8() + "\r\n");
        ical.write("SUMMARY:" + subject.toUtf8() + "\r\n");
        ical.write("TRANSP:TRANSPARENT\r\n");
        ical.write("END:VEVENT\r\n");
        ical.write("END:VCALENDAR\r\n");
        ical.close();
        QProcess* xdg = new QProcess();
        connect(xdg, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processFinished(int,QProcess::ExitStatus)));
        connect(xdg, SIGNAL(finished(int,QProcess::ExitStatus)), xdg, SLOT(deleteLater()));
        xdg->start("xdg-open", QStringList({ical.fileName()}));
    }
}

void PassHandler::processFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitCode != 0 || exitStatus == QProcess::CrashExit)
        emit calendarEntryFinished("xdg-open");
    else
        emit calendarEntryFinished("ok");
}

void PassHandler::m_copyFile(QIODevice &to, QIODevice &from) {
    for (QByteArray data(from.read(4096)); data.size() > 0; data = from.read(4096))
        to.write(data);
}

QByteArray PassHandler::m_rfc2616(QDateTime datetime) {
    QString dateString(datetime.toUTC().toString(Qt::RFC2822Date));
    QStringList weekdays({"NULL", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"});
    dateString.prepend(weekdays.at(datetime.date().dayOfWeek()) + ", ");
    dateString.replace("+0000", "GMT");
    return dateString.toUtf8();
}
