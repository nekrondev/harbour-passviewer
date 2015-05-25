#ifndef SIGNATURECHECKER_H
#define SIGNATURECHECKER_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/x509v3.h>
#include <openssl/x509_vfy.h>
#include <openssl/bio.h>
#include <openssl/pkcs7.h>

class SignatureChecker : public QObject
{
    Q_OBJECT
public:
    explicit SignatureChecker(QObject *parent = 0);
    ~SignatureChecker();

    Q_INVOKABLE bool signatureValid(QString manifest, QString signature);

signals:

public slots:

private:
    X509_STORE* m_castore;
    X509_VERIFY_PARAM* m_vpm;

};

#endif // SIGNATURECHECKER_H
