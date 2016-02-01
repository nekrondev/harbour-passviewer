#include "zipfile.h"

#pragma pack(1)

struct Eod {
    char signature[4];
    quint16 disks;
    quint16 startDisk;
    quint16 directoriesDisk;
    quint16 directories;
    quint32 directorySize;
    quint32 directoryPos;
};

struct DirEntry {
    char signature[4];
    quint16 version;
    quint16 versionNeeded;
    quint16 gpbf;
    quint16 compression;
    quint16 mtime;
    quint16 mdate;
    quint32 crc32;
    quint32 compressedSize;
    quint32 size;
    quint16 nameLength;
    quint16 extraLength;
    quint16 commentLength;
    quint16 firstDisk;
    quint16 internalAttributes;
    quint32 externalAttributes;
    quint32 headerPos;
};

struct FileHeader {
    char signature[4];
    quint16 versionNeeded;
    quint16 gpgf;
    quint16 compression;
    quint16 mtime;
    quint16 mdate;
    quint32 crc32;
    quint32 compressedSize;
    quint32 size;
    quint16 nameLength;
    quint16 extraLength;
};

#pragma pack()

ZipFile::ZipFile(QObject *parent) :
    QObject(parent),
    m_valid(false),
    m_file(NULL),
    m_entries()
{

}

ZipFile::ZipFile(QString filename) :
    QObject(NULL),
    m_valid(false),
    m_file(filename),
    m_entries()
{
    if (!m_file.open(QFile::ReadOnly))
        return;
    // search the "end of directory" entry
    m_file.seek(m_file.size() - 1024);
    QByteArray trailer(m_file.read(1024));
    qint64 pos_eod = trailer.lastIndexOf("PK\x05\x06");
    if (pos_eod == -1)
        return;
    pos_eod += m_file.size() - 1024;
    // read the "end of directory" entry (with the directory position)
    m_file.seek(pos_eod);
    QByteArray eodString(m_file.read(sizeof(Eod)));
    if (eodString.size() != sizeof(Eod))
        return;
    Eod* eod = (Eod*)eodString.data();
    // read the directory
    quint32 dirPos = qFromLittleEndian<quint32>(eod->directoryPos);
    DirEntry* entry = NULL;
    QTextCodec* cp437 = QTextCodec::codecForName("cp437");
    while(dirPos + sizeof(DirEntry) <= m_file.size()) {
        // read the directory entry
        m_file.seek(dirPos);
        QByteArray entryString(m_file.read(sizeof(DirEntry)));
        if (entryString.size() != sizeof(DirEntry))
            break;
        entry = (DirEntry*)entryString.data();
        if (QByteArray(entry->signature, 4) != "PK\x01\x02")
            break;
        dirPos += sizeof(DirEntry) + qFromLittleEndian<quint16>(entry->nameLength) + qFromLittleEndian<quint16>(entry->extraLength) + qFromLittleEndian<quint16>(entry->commentLength);
        // ignore entries with unsupported compression methods
        quint16 compression = qFromLittleEndian<quint16>(entry->compression);
        if (compression == 0 || compression == 8 || compression == 12 || compression == 14) {
            // get the info
            QByteArray rawname(m_file.read(qFromLittleEndian<quint16>(entry->nameLength)));
            QString name;
            if (cp437 != NULL && (qFromLittleEndian<quint16>(entry->gpbf) & 0x0800) == 0)
                name = cp437->toUnicode(rawname);
            else
                name = QString(rawname); // UTF-8
            QList<int> entryData;
            entryData.append(compression);
            entryData.append(qFromLittleEndian<quint32>(entry->headerPos));
            entryData.append(qFromLittleEndian<quint32>(entry->compressedSize));
            entryData.append(qFromLittleEndian<quint32>(entry->size));
            m_entries.insert(name, entryData);
        }
    }
    if (m_entries.size() > 0)
        m_valid = true;
}

QByteArray ZipFile::getFile(QString filename) {
    if (!m_entries.contains(filename))
        return QByteArray();
    if (m_entries.value(filename).at(2) == 0)  // empty file
        return QByteArray();
    // get the ZIP file header
    m_file.seek(m_entries.value(filename).at(1));
    QByteArray headerString(m_file.read(sizeof(FileHeader)));
    if (headerString.size() != sizeof(FileHeader))
        return QByteArray();
    FileHeader* header = (FileHeader*)headerString.data();
    // read the actual, compressed file
    m_file.seek(m_entries.value(filename).at(1) + sizeof(FileHeader) + qFromLittleEndian<quint16>(header->nameLength) + qFromLittleEndian<quint16>(header->extraLength));
    QByteArray compressed(m_file.read(m_entries.value(filename).at(2)));
    if (compressed.size() != m_entries.value(filename).at(2))
        return QByteArray();
    // decompress (if it's compressed)
    switch (m_entries.value(filename).at(0)) {
    case 0:  // uncompressed
        return compressed;
        break;
    case 8:  // deflate
        {
            compressed.prepend("\x78\x9c");  // ZLib Header
            QByteArray uncompressed(m_entries.value(filename).at(3), '\0');
            uLongf insize = compressed.size();
            uLongf outsize = uncompressed.size();
            uncompress((uchar*)uncompressed.data(), &outsize, (uchar*)compressed.data(), insize);
            return uncompressed;
        }
        break;
    case 12:  // bzip2
        {
            QByteArray uncompressed(m_entries.value(filename).at(3), '\0');
            uint insize = compressed.size();
            uint outsize = uncompressed.size();
            BZ2_bzBuffToBuffDecompress(uncompressed.data(), &outsize, compressed.data(), insize, 0, 0);
            return uncompressed;
        }
        break;
    case 14:  // lzma
        {
            lzma_stream lzma = LZMA_STREAM_INIT;
            if (lzma_alone_decoder(&lzma, UINT64_MAX) != LZMA_OK)
                return QByteArray();
            QByteArray uncompressed(m_entries.value(filename).at(3), '\0');
            // to convert from ZIP to LZMA header, remove the first 4 bytes, then insert a little endian quint64 with the uncompressed length at position 5
            union {
                quint64 ll;
                char c[8];
            } size;
            size.ll = qToLittleEndian<quint64>(uncompressed.size());
            compressed.remove(0, 4);
            compressed.insert(5, size.c, 8);
            lzma.next_in = (uint8_t*)compressed.data();
            lzma.avail_in = compressed.size();
            lzma.next_out = (uint8_t*)uncompressed.data();
            lzma.avail_out = uncompressed.size();
            lzma_code(&lzma, LZMA_RUN);
            lzma_end(&lzma);
            return uncompressed;
        }
    }
    return QByteArray();
}

QString ZipFile::getTextFile(QString filename) {
    // convenience method for unicode-encoded text files
    QByteArray bytes(getFile(filename));
    QTextCodec* codec = QTextCodec::codecForUtfText(bytes, QTextCodec::codecForName("UTF-8"));
    if (codec != NULL)
        return codec->toUnicode(bytes);
    else
        return QString(bytes);
}
