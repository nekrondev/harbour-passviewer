#ifndef AZTECPAYLOAD_H
#define AZTECPAYLOAD_H

#include <QByteArray>
#include <QString>

struct zint_symbol;

enum class AztecPayloadStyle {
    OptimizedMixed,
    CompactBinary,
    Gs1
};

class AztecPayload
{
public:
    static AztecPayloadStyle classify(const QByteArray &payload, const QString &encoding);
    static const char *styleName(AztecPayloadStyle style);
    static void applyEncodeOptions(struct zint_symbol *symbol, AztecPayloadStyle style);
};

#endif // AZTECPAYLOAD_H
