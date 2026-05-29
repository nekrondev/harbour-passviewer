#include "barcodeimageprovider.h"

#include "aztecpayload.h"
#include "barcodecodec.h"
#include "barcodedebug.h"

BarcodeImageProvider::BarcodeImageProvider() :
    QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}

QPixmap BarcodeImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) {
    QString type;
    QString encoding;
    QByteArray content;
    QPixmap empty;
    size->setWidth(0);
    size->setHeight(0);
    int firstslash = id.indexOf('/');
    int secondslash = id.indexOf('/', firstslash + 1);
    if (firstslash == -1 || secondslash == -1)
        return empty;
    type = id.left(firstslash);
    encoding = id.mid(firstslash + 1, secondslash - firstslash - 1);
    if (!BarcodeCodec::isSupportedEncoding(encoding))
        return empty;
    content = BarcodeCodec::payloadFromBase64(id.mid(secondslash + 1).toLatin1(), encoding);
    barcodeLogStage("imageprovider_zint_input", content);
    if (content.isEmpty() && !id.mid(secondslash + 1).isEmpty())
        return empty;
    zint_symbol* symbol = ZBarcode_Create();
    if (!symbol)
        return empty;
    if (type == "code128") {
        symbol->symbology = BARCODE_CODE128;
        symbol->input_mode = UNICODE_MODE;
    }
    else if (type == "qr") {
        symbol->symbology = BARCODE_QRCODE;
    }
    else if (type == "aztec") {
        const AztecPayloadStyle aztecStyle = AztecPayload::classify(content, encoding);
        barcodeLogStage("aztec_encode_style", QByteArray(AztecPayload::styleName(aztecStyle)));
        symbol->symbology = BARCODE_AZTEC;
        AztecPayload::applyEncodeOptions(symbol, aztecStyle);
    }
    else if (type == "pdf417") {
        symbol->symbology = BARCODE_PDF417;
    }
    else {
        ZBarcode_Delete(symbol);
        return empty;
    }
    if (ZBarcode_Encode_and_Buffer(symbol, (unsigned char*)content.data(), content.size(), 0) != 0) {
        ZBarcode_Delete(symbol);
        return empty;
    }
    QImage barcode(symbol->bitmap_width, symbol->bitmap_height, QImage::Format_RGB32);
    size->setWidth(symbol->bitmap_width);
    size->setHeight(symbol->bitmap_height);
    int bitmapPos = 0;
    for (int row = 0; row < symbol->bitmap_height; row++) {
        QRgb* line = (QRgb*)barcode.scanLine(row);
        for (int col = 0; col < symbol->bitmap_width; col++) {
            line[col] = qRgb((uchar)symbol->bitmap[bitmapPos], (uchar)symbol->bitmap[bitmapPos + 1], (uchar)symbol->bitmap[bitmapPos + 2]);
            bitmapPos += 3;
        }
    }
    ZBarcode_Delete(symbol);
    return QPixmap::fromImage(barcode);
}
