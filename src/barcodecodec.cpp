#include "barcodecodec.h"

#include "barcodedebug.h"

#include <QTextCodec>

QByteArray BarcodeCodec::normalizedCodecName(const QString &encoding)
{
    const QString e = encoding.trimmed().toLower();
    if (e.isEmpty() || e == "iso-8859-1" || e == "iso_8859-1" || e == "latin1")
        return "ISO-8859-1";
    if (e == "utf-8" || e == "utf8")
        return "UTF-8";
    return encoding.toUtf8();
}

bool BarcodeCodec::isSupportedEncoding(const QString &encoding)
{
    return QTextCodec::codecForName(normalizedCodecName(encoding)) != 0;
}

QByteArray BarcodeCodec::messageToPayload(const QString &message, const QString &encoding)
{
    const QByteArray codecName = normalizedCodecName(encoding);
    if (codecName == "ISO-8859-1") {
        /* Wallet barcodes: one Latin-1 code unit per QChar, no QTextCodec round-trip */
        QByteArray payload;
        payload.reserve(message.size());
        for (int i = 0; i < message.size(); i++) {
            const ushort unit = message.at(i).unicode();
            if (unit > 0xFF)
                return QByteArray();
            payload.append((char) unit);
        }
        return payload;
    }
    QTextCodec *codec = QTextCodec::codecForName(codecName);
    if (!codec)
        return QByteArray();
    return codec->fromUnicode(message);
}

QByteArray BarcodeCodec::payloadFromBase64(const QByteArray &base64, const QString &encoding)
{
    if (!isSupportedEncoding(encoding))
        return QByteArray();
    return QByteArray::fromBase64(base64);
}

QString BarcodeCodec::messageToBase64(const QString &message, const QString &encoding) const
{
    barcodeLogMessageStage("barcodeCodec_input_string", message, encoding);
    const QByteArray payload = messageToPayload(message, encoding);
    barcodeLogStage("barcodeCodec_payload", payload);
    if (payload.isEmpty() && !message.isEmpty())
        return QString();
    return QString::fromLatin1(payload.toBase64());
}

BarcodeCodec::BarcodeCodec(QObject *parent) :
    QObject(parent)
{
}
