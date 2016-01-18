#include "pass.h"

Pass::Pass(QObject *parent) :
    QObject(parent),
    m_isValid(false),
    m_name(),
    m_path(),
    m_points(-1),
    m_jsondata(),
    m_typeId(),
    m_updateable(false)
{

}

Pass::Pass(QString path, QString jsondata) :
    QObject(),
    m_isValid(false),
    m_name(),
    m_path(path),
    m_points(-1),
    m_jsondata(jsondata),
    m_typeId(),
    m_updateable(false)
{
    QJsonDocument json(QJsonDocument::fromJson(jsondata.toUtf8()));
    if (json.isNull())
        return;
    m_typeId = json.object().value("passTypeIdentifier").toString();
    if (m_typeId == "")
        return;
    QStringList styles({"boardingPass", "coupon", "eventTicket", "storeCard", "generic"});
    QString passStyle;
    for (auto style = styles.cbegin(); style != styles.cend(); ++style) {
        if (json.object().contains(*style)) {
            passStyle = *style;
            break;
        }
    }
    if (passStyle == "")
        return;
    QJsonArray primary(json.object().value(passStyle).toObject().value("primaryFields").toArray());
    if (passStyle == "boardingPass")
        m_name = primary.at(0).toObject().value("value").toString() + " → " + primary.at(1).toObject().value("value").toString();
    else
        m_name = primary.at(0).toObject().value("label").toString() + " " + primary.at(0).toObject().value("value").toString();
    if (m_name.size() < 4) {
        QFileInfo file(path);
        m_name = file.baseName();
    }
    m_isValid = true;
    if (json.object().contains("webServiceURL") && json.object().contains("serialNumber") && json.object().contains("authenticationToken"))
        m_updateable = true;
}
