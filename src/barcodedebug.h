#ifndef BARCODEDEBUG_H
#define BARCODEDEBUG_H

#include <QByteArray>
#include <QString>

QString barcodeHexDump(const QByteArray &data, int maxBytes = 128);
void barcodeLogStage(const char *stage, const QByteArray &payload);
void barcodeLogMessageStage(const char *stage, const QString &message, const QString &encoding);

#endif // BARCODEDEBUG_H
