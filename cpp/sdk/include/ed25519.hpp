#pragma once

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <vector>

class ED25519
{
private:
    /* data */
    EVP_PKEY *m_pkey = NULL;

public:
    ED25519(/* args */);
    ~ED25519();

    void init();

    void sign();

    std::vector<uint8_t> sign(unsigned char *msg, size_t msg_len);

    bool verify(unsigned char *sigret, size_t siglen,
                const unsigned char *tbs, size_t tbslen);
};

ED25519::ED25519(/* args */)
{
}

ED25519::~ED25519()
{
}

void ED25519::init()
{
    EVP_PKEY_CTX *pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_ED25519, NULL);
    int ret = 0;

    ret = EVP_PKEY_keygen_init(pctx);
    ret = EVP_PKEY_keygen(pctx, &m_pkey);
    EVP_PKEY_CTX_free(pctx);

    PEM_write_PrivateKey(stdout, m_pkey, NULL, NULL, 0, NULL, NULL);
}

void ED25519::sign()
{
    EVP_MD_CTX *pctx = EVP_MD_CTX_new();

    int EVP_DigestSignInit(EVP_MD_CTX * ctx, EVP_PKEY_CTX * *pctx,
                           const EVP_MD *type, ENGINE *e, EVP_PKEY *pkey);
    int EVP_DigestSignUpdate(EVP_MD_CTX * ctx, const void *d, size_t cnt);
    int EVP_DigestSignFinal(EVP_MD_CTX * ctx, unsigned char *sig, size_t *siglen);

    int EVP_DigestSign(EVP_MD_CTX * ctx, unsigned char *sigret,
                       size_t *siglen, const unsigned char *tbs,
                       size_t tbslen);

    EVP_MD_CTX_destroy(pctx);
}

std::vector<uint8_t> ED25519::sign(unsigned char *msg, size_t msg_len)
{
    size_t sig_len;
    unsigned char *sig = NULL;
    EVP_MD_CTX *md_ctx = EVP_MD_CTX_new();
    int ret = 0;

    ret = EVP_DigestSignInit(md_ctx, NULL, EVP_sha3_256(), NULL, m_pkey);
    /* Calculate the requires size for the signature by passing a NULL buffer */
    ret = EVP_DigestSign(md_ctx, NULL, &sig_len, msg, msg_len);
    sig = (unsigned char *)OPENSSL_zalloc(sig_len);

    ret = EVP_DigestSign(md_ctx, sig, &sig_len, msg, msg_len);

    std::vector<uint8_t> signature(sig, sig + sig_len);

    OPENSSL_free(sig);

    EVP_MD_CTX_free(md_ctx);

    return signature;
}

bool ED25519::verify(unsigned char *sigret, size_t siglen,
                     const unsigned char *tbs, size_t tbslen)
{
    EVP_MD_CTX *md_ctx = EVP_MD_CTX_new();

    EVP_DigestSignInit(md_ctx, NULL, EVP_sha3_256(), NULL, m_pkey);

    EVP_MD_CTX_free(md_ctx);
    
    return true;
}