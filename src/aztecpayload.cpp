#include "aztecpayload.h"

#include "zint/zint.h"

namespace {

bool isLatin1Encoding(const QString &encoding)
{
    const QString e = encoding.trimmed().toLower();
    return e.isEmpty() || e == "iso-8859-1" || e == "iso_8859-1" || e == "latin1";
}

bool looksLikeGs1(const QByteArray &payload)
{
    if (payload.isEmpty())
        return false;

    const uchar *const bytes = reinterpret_cast<const uchar *>(payload.constData());
    const int length = payload.size();

    if (bytes[0] == '(')
        return true;

    int highBytes = 0;
    int controlBytes = 0;
    int gs1Chars = 0;

    for (int i = 0; i < length; i++) {
        const uchar b = bytes[i];
        if (b == 0)
            return false;
        if (b >= 0x80)
            highBytes++;
        if (b < 0x20 && b != 0x09 && b != 0x0A && b != 0x0D)
            controlBytes++;
        if (b >= '0' && b <= '9')
            gs1Chars++;
        else if (b == '(' || b == ')')
            gs1Chars++;
    }

    if (controlBytes > 0 || highBytes > length / 4)
        return false;

    return gs1Chars * 2 >= length;
}

bool needsCompactBinary(const QByteArray &payload)
{
    if (payload.isEmpty())
        return false;

    const uchar *const bytes = reinterpret_cast<const uchar *>(payload.constData());
    const int length = payload.size();

    for (int i = 0; i < length; i++) {
        const uchar b = bytes[i];
        if (b == 0)
            return true;
        if (b < 0x20 && b != 0x09 && b != 0x0A && b != 0x0D)
            return true;
        if (b >= 0x80)
            return true;
    }
    return false;
}

} // namespace

AztecPayloadStyle AztecPayload::classify(const QByteArray &payload, const QString &encoding)
{
    if (!isLatin1Encoding(encoding))
        return AztecPayloadStyle::OptimizedMixed;
    if (looksLikeGs1(payload))
        return AztecPayloadStyle::Gs1;
    if (needsCompactBinary(payload))
        return AztecPayloadStyle::CompactBinary;
    return AztecPayloadStyle::OptimizedMixed;
}

const char *AztecPayload::styleName(AztecPayloadStyle style)
{
    switch (style) {
    case AztecPayloadStyle::CompactBinary:
        return "compact_binary";
    case AztecPayloadStyle::Gs1:
        return "gs1";
    case AztecPayloadStyle::OptimizedMixed:
    default:
        return "optimized_mixed";
    }
}

void AztecPayload::applyEncodeOptions(struct zint_symbol *symbol, AztecPayloadStyle style)
{
    if (!symbol)
        return;

    symbol->input_mode = DATA_MODE;
    symbol->output_options &= ~BARCODE_AZTEC_COMPACT_BINARY;

    switch (style) {
    case AztecPayloadStyle::Gs1:
        symbol->input_mode = GS1_MODE;
        break;
    case AztecPayloadStyle::CompactBinary:
        symbol->output_options |= BARCODE_AZTEC_COMPACT_BINARY;
        break;
    case AztecPayloadStyle::OptimizedMixed:
    default:
        break;
    }
}
