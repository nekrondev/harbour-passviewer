#ifndef BARCODECODEC_H
#define BARCODECODEC_H

#include <QByteArray>
#include <QObject>
#include <QString>

class BarcodeCodec : public QObject
{
    Q_OBJECT

public:
    explicit BarcodeCodec(QObject *parent = 0);

    Q_INVOKABLE QString messageToBase64(const QString &message, const QString &encoding) const;

    static QByteArray messageToPayload(const QString &message, const QString &encoding);
    static QByteArray payloadFromBase64(const QByteArray &base64, const QString &encoding);
    static bool isSupportedEncoding(const QString &encoding);

private:
    static QByteArray normalizedCodecName(const QString &encoding);
};

#endif // BARCODECODEC_H
