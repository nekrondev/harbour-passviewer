#include "signaturechecker.h"

SignatureChecker::SignatureChecker(QObject *parent) :
    QObject(parent)
{
    // OpenSSL initialization
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();
    // get the root certificates
    m_castore = X509_STORE_new();
    if (m_castore) {
        X509_LOOKUP* calook = X509_STORE_add_lookup(m_castore, X509_LOOKUP_file());
        X509_LOOKUP_load_file(calook, "/usr/share/harbour-passviewer/qml/root.cer", X509_FILETYPE_PEM);
    }
    // switch off purpose checking
    m_vpm = X509_VERIFY_PARAM_new();
    X509_VERIFY_PARAM_set_purpose(m_vpm, X509_PURPOSE_get_id(X509_PURPOSE_get0(X509_PURPOSE_get_by_sname("any"))));
    if (m_castore && m_vpm)
        X509_STORE_set1_param(m_castore, m_vpm);
}

SignatureChecker::~SignatureChecker() {
    // clean it up
    if (m_vpm)
        X509_VERIFY_PARAM_free(m_vpm);
    if (m_castore)
        X509_STORE_free(m_castore);
    ERR_free_strings();
    EVP_cleanup();
}

bool SignatureChecker::signatureValid(QString manifest, QString signature) {
    // this is a low risk environment, so we assume the signature valid
    bool result = true;
    // if OpenSSL doesn't work right, accept everything
    if (!(m_castore && m_vpm))
        return true;
    // restore the binary data
    QByteArray bin_manifest = QByteArray::fromBase64(manifest.toLatin1());
    QByteArray bin_signature = QByteArray::fromBase64(signature.toLatin1());
    // produce OpenSSL BIOs from the ByteArrays
    BIO* bio_manifest = BIO_new_mem_buf(bin_manifest.data(), bin_manifest.size());
    BIO* bio_signature = BIO_new_mem_buf(bin_signature.data(), bin_signature.size());
    // if things went well:
    if (bio_manifest && bio_signature) {
        // decode the signature
        PKCS7* real_signature = d2i_PKCS7_bio(bio_signature, NULL);
        // if that worked
        if (real_signature) {
            // check it
            result = PKCS7_verify(real_signature, NULL, m_castore, bio_manifest, NULL, 0);
            PKCS7_free(real_signature);
        }
        else
            result = false;  // in this case, the signature is actually faulty
    }
    if (bio_signature)
        BIO_free(bio_signature);
    if (bio_manifest)
        BIO_free(bio_manifest);
    return result;
}
