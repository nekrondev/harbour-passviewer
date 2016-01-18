#ifndef ZIPFILE_H
#define ZIPFILE_H

/* This class reads ZIP files using Deflate, BZip2 and/or LZMA compression,
 * which should cover almost anything in the wild. Please note, however, that
 * decompression is done in memory, and the ZIP64 extension is not supported.
 * Thus, it is not suitable for large files. */

#include <QObject>
#include <QByteArray>
#include <QMap>
#include <QList>
#include <QFile>
#include <QtEndian>
#include <QTextCodec>

#include <zlib.h>
#include <bzlib.h>
#include <lzma.h>

class ZipFile : public QObject
{
    Q_OBJECT
public:
    explicit ZipFile(QObject *parent = 0);
    explicit ZipFile(QString filename);

    Q_INVOKABLE bool isValid() { return m_valid; }
    Q_INVOKABLE QByteArray getFile(QString filename);
    Q_INVOKABLE QString getTextFile(QString filename);

signals:

public slots:

private:
    bool m_valid;
    QFile m_file;
    QMap<QString, QList<int> > m_entries;
};

#endif // ZIPFILE_H
