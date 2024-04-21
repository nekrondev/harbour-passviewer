#include "barcodeimageprovider.h"

BarcodeImageProvider::BarcodeImageProvider() :
    QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}

QPixmap BarcodeImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) {
    QString type;
    QTextCodec* codec;
    QByteArray content;
    QPixmap empty;
    size->setWidth(0);
    size->setHeight(0);
    // get the barcode type, encoding and content from the ID
    int firstslash = id.indexOf('/');
    int secondslash = id.indexOf('/', firstslash + 1);
    if (firstslash == -1 || secondslash == -1)
        return empty;
    type = id.left(firstslash);
    codec = QTextCodec::codecForName(id.mid(firstslash + 1, secondslash - firstslash - 1).toUtf8());
    if (!codec)
        return empty;
    // decode the (Base64 encoded UTF-8) barcode content
    content = QByteArray::fromBase64(id.mid(secondslash + 1).toLatin1());
    // Code128 requires UTF-8. For everything else: convert content to the desired encoding.
    if (type != "code128")
        content = codec->fromUnicode(QString::fromUtf8(content.constData(), content.size()));
    // build the request
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
        symbol->symbology = BARCODE_AZTEC;
    }
    else if (type == "pdf417") {
        symbol->symbology = BARCODE_PDF417;
    }
    else {
        ZBarcode_Delete(symbol);
        return empty;
    }
    // create the barcode
    if (ZBarcode_Encode_and_Buffer(symbol, (unsigned char*)content.data(), content.size(), 0) != 0) {
        ZBarcode_Delete(symbol);
        return empty;
    }
    // convert barcode to QImage for QPixmap
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
