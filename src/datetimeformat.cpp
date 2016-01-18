#include "datetimeformat.h"

DateTimeFormat::DateTimeFormat(QObject *parent) :
    QObject(parent),
    m_dateFormats(),
    m_timeFormats()
{
    setlocale(LC_ALL, "");
    QString dtFormat(m_expandCodes(nl_langinfo(D_T_FMT)));
    QString dFormat(m_expandCodes(nl_langinfo(D_FMT)));
    QString tFormat(m_expandCodes(nl_langinfo(T_FMT)));

    m_dateFormats.insert("short", m_clearCodes(dFormat.replace("%b", "%m").replace("%B", "%m").replace("%Y", "%y"), "demy"));
    m_dateFormats.insert("medium", m_clearCodes(dtFormat.replace("%B", "%b").replace("%y", "%Y"), "debmY"));
    m_dateFormats.insert("long", m_clearCodes(dtFormat.replace("%b", "%B").replace("%y", "%Y"), "deBmY"));
    m_dateFormats.insert("full", m_clearCodes(dtFormat.replace("%a", "%A").replace("%b", "%B").replace("%y", "%Y"), "AdeBmY"));
    if (tFormat.contains("%p", Qt::CaseInsensitive)) {
        m_timeFormats.insert("short", "%I:%M %p");
        m_timeFormats.insert("medium", "%I:%M:%S %p");
        m_timeFormats.insert("long", "%I:%M:%S %p %Z");
    }
    else {
        m_timeFormats.insert("short", "%H:%M");
        m_timeFormats.insert("medium", "%H:%M:%S");
        m_timeFormats.insert("long", "%H:%M:%S %Z");
    }
    m_timeFormats.insert("full", m_timeFormats.value("long"));
}

QString DateTimeFormat::m_expandCodes(QString orig) {
    return orig.replace("%D", "%m/%d/%y").replace("%F", "%Y-%m-%d").replace("%h", "%b").replace("%r", "%I:%M:%S %p").replace("%R", "%H:%M").replace("%T", "%H:%M:%S");
}

QString DateTimeFormat::m_clearCodes(QString orig, QString allowed) {
    QString cleared;
    bool first = true;
    while (orig.indexOf('%') != -1) {
        int pos = orig.indexOf('%');
        if (!first)
            cleared += orig.left(pos);
        int size = 3;
        QChar code(orig.at(pos + 1));
        if (code == 'E' || code == 'O') {
            size = 4;
            code = orig.at(pos + 2);
        }
        if (allowed.contains(code)) {
            cleared += orig.mid(pos, size);
            first = false;
        }
        orig = orig.mid(pos + size);
    }
    int pos = cleared.lastIndexOf('%') + 1;
    if (pos == 0)
        return QString("%Y-%m-%d");
    if (cleared.at(pos) == 'E' || cleared.at(pos) == 'O')
        pos++;
    return cleared.left(pos + 1);
}

QString DateTimeFormat::format(QString dateTime, QString dateFormat, QString timeFormat, bool ignoreTimeZone) {
    if (ignoreTimeZone) {
        if (dateTime.right(1) == "Z") {
            dateTime = dateTime.left(dateTime.size() - 1);
        }
        else {
            int pos = dateTime.indexOf('+');
            if (pos == -1)
                pos = dateTime.indexOf('-');
            if (pos != -1)
                dateTime = dateTime.left(pos);
        }
    }
    QDateTime dtObj(QDateTime::fromString(dateTime, Qt::ISODate));
    if (!dtObj.isValid())
        return dateTime;
    QString formatStr;
    if (m_dateFormats.contains(dateFormat))
        formatStr = m_dateFormats.value(dateFormat);
    if (m_timeFormats.contains(timeFormat)) {
        if (formatStr.size() > 0)
            formatStr += ' ';
        formatStr += m_timeFormats.value(timeFormat);
    }
    time_t epoch = dtObj.toTime_t();
    tm* tuple = localtime(&epoch);
    if (tuple == NULL)
        return dateTime;
    QByteArray formatted(256, '\0');
    strftime(formatted.data(), formatted.size(), formatStr.toUtf8().data(), tuple);
    return QString(formatted);
}
