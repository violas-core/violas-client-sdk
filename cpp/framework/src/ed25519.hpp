#pragma once

#include <openssl/evp.h>
#include <openssl/pem.h>

class ed25519
{
private:
    /* data */
public:
    ed25519(/* args */);
    ~ed25519();

    void init();

    void sign();
};

ed25519::ed25519(/* args */)
{
}

ed25519::~ed25519()
{
}

void ed25519::init()
{
    EVP_PKEY *pkey = NULL;
    EVP_PKEY_CTX *pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_ED25519, NULL);

    EVP_PKEY_keygen_init(pctx);
    EVP_PKEY_keygen(pctx, &pkey);
    EVP_PKEY_CTX_free(pctx);

    PEM_write_PrivateKey(stdout, pkey, NULL, NULL, 0, NULL, NULL);
}

void ed25519::sign()
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
