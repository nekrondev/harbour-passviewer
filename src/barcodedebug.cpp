#include "barcodedebug.h"

#include <QDebug>
#include <QTextCodec>

QString barcodeHexDump(const QByteArray &data, int maxBytes)
{
    const int n = qMin(data.size(), maxBytes);
    QString out;
    for (int i = 0; i < n; i++)
        out += QString("%1 ").arg((uchar)data.at(i), 2, 16, QChar('0'));
    if (data.size() > maxBytes)
        out += QString("... (%1 bytes total)").arg(data.size());
    else
        out += QString("(%1 bytes)").arg(data.size());
    return out.trimmed();
}

void barcodeLogStage(const char *stage, const QByteArray &payload)
{
    qWarning() << "BARCODE_DEBUG" << stage << barcodeHexDump(payload);
}

void barcodeLogMessageStage(const char *stage, const QString &message, const QString &encoding)
{
    const QString enc = encoding.trimmed().toLower();
    QTextCodec *codec = QTextCodec::codecForName(
        (enc == "utf-8" || enc == "utf8") ? "UTF-8" : "ISO-8859-1");
    const QByteArray bytes = codec ? codec->fromUnicode(message) : QByteArray();
    qWarning() << "BARCODE_DEBUG" << stage << "encoding=" << encoding
               << "strlen=" << message.size() << barcodeHexDump(bytes);
}
