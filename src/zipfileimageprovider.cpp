#include "zipfileimageprovider.h"

ZipFileImageProvider::ZipFileImageProvider() :
    QQuickImageProvider(QQuickImageProvider::Pixmap)
{

}

QPixmap ZipFileImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) {
    QPixmap empty;
    QString zipPath(id.left(id.lastIndexOf('/')));
    QString imgFile(id.right(id.size() - id.lastIndexOf('/') - 1));
    size->setWidth(0);
    size->setHeight(0);
    zipPath.prepend('/');
    ZipFile zip(zipPath);
    if (!zip.isValid())
        return empty;
    QByteArray data;
    if (imgFile == "icon.png")
        data = zip.getFile("icon@2x.png");
    if (data.size() == 0)
        data = zip.getFile(imgFile);
    if (data.size() == 0)
        return empty;
    QImage img(QImage::fromData(data));
    if (img.isNull())
        return empty;
    size->setWidth(img.width());
    size->setHeight(img.height());
    return QPixmap::fromImage(img);
}
