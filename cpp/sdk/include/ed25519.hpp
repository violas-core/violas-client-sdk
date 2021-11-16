#pragma once

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <vector>
#include <string>
#include <sstream>
#include <array>
#include <cassert>
#include <exception>

class ED25519
{
private:
    /* data */
    EVP_PKEY *m_pkey = NULL;

    ED25519(EVP_PKEY *pkey);

public:
    ED25519();
    ~ED25519();

    static ED25519 init(std::array<uint8_t, 32> private_key);

    std::array<uint8_t, 32> get_private_key();

    std::array<uint8_t, 32> get_public_key();

    void sign();

    std::vector<uint8_t> sign(unsigned char *msg, size_t msg_len);

    bool verify(unsigned char *sigret, size_t siglen,
                const unsigned char *tbs, size_t tbslen);

    static void test();
};

ED25519::ED25519()
{
    EVP_PKEY_CTX *pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_ED25519, NULL);
    int ret = 0;

    ret = EVP_PKEY_keygen_init(pctx);
    ret = EVP_PKEY_keygen(pctx, &m_pkey);
    EVP_PKEY_CTX_free(pctx);

    // PEM_write_PrivateKey(stdout, m_pkey, NULL, NULL, 0, NULL, NULL);
}

ED25519::ED25519(EVP_PKEY *pkey)
{
    m_pkey = pkey;
}

ED25519::~ED25519()
{
    EVP_PKEY_free(m_pkey);
    m_pkey = nullptr;
}

inline void check_ret(int ret, char *error)
{
    if (ret != 1)
        std::__throw_runtime_error(error);
}

ED25519 ED25519::init(std::array<uint8_t, 32> private_key)
{
    EVP_PKEY *pkey = EVP_PKEY_new_raw_private_key(EVP_PKEY_ED25519, NULL, private_key.data(), private_key.size());

    return ED25519(pkey);
}

std::array<uint8_t, 32> ED25519::get_public_key()
{
    std::array<uint8_t, 32> pub_key;
    size_t len = pub_key.size();

    EVP_PKEY_get_raw_public_key(m_pkey, pub_key.data(), &len);

    return pub_key;
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

    ret = EVP_DigestSignInit(md_ctx, NULL, NULL, NULL, m_pkey); // EVP_sha256()
    check_ret(ret, "EVP_DigestSignInit");

    /* Calculate the requires size for the signature by passing a NULL buffer */
    ret = EVP_DigestSign(md_ctx, NULL, &sig_len, msg, msg_len);
    check_ret(ret, "EVP_DigestSign");

    sig = (unsigned char *)OPENSSL_zalloc(sig_len);

    ret = EVP_DigestSign(md_ctx, sig, &sig_len, msg, msg_len);
    check_ret(ret, "EVP_DigestSign");

    std::vector<uint8_t> signature(sig, sig + sig_len);

    OPENSSL_free(sig);

    EVP_MD_CTX_free(md_ctx);

    return signature;
}

bool ED25519::verify(unsigned char *msg, size_t msg_len,
                     const unsigned char *sig, size_t sig_len)
{
    int ret = 0;

    EVP_MD_CTX *md_ctx = EVP_MD_CTX_new();

    ret = EVP_DigestVerifyInit(md_ctx, NULL, NULL, NULL, m_pkey); // EVP_sha3_256()
    check_ret(ret, "EVP_DigestVerifyInit");

    ret = EVP_DigestVerifyUpdate(md_ctx, msg, msg_len);
    check_ret(ret, "EVP_DigestVerifyUpdate");

    ret = EVP_DigestVerifyFinal(md_ctx, sig, sig_len);
    check_ret(ret, "EVP_DigestVerifyFinal");

    EVP_MD_CTX_free(md_ctx);

    return ret == 1;
}

void ED25519::test()
{
    uint8_t buff[32];
    size_t len = sizeof(buff);
    using namespace std;

    string private_key = "4cc9cd70d755484327b5164fa8f3f080b12aea9cbcc7bf0d4e7d92f58d4ae990";

    for (size_t i = 0; i < private_key.length(); i += 2)
    {
        stringstream ss;
        uint16_t byte;
        ss << private_key[i] << private_key[i + 1];
        ss >> hex >> byte;
        buff[31 - i / 2] = (uint8_t)byte;
    }

    EVP_PKEY *pkey = EVP_PKEY_new_raw_private_key(EVP_PKEY_ED25519, NULL, buff, len);

    uint8_t pub[32];
    len = size_t(pub);
    EVP_PKEY_get_raw_public_key(pkey, pub, &len);

    // public key 4cc9cd70d755484327b5164fa8f3f080b12aea9cbcc7bf0d4e7d92f58d4ae990h
    try
    {
        ED25519 ed25519;

        std::string msg = "Hello Ed25519";

        auto signature = ed25519.sign((uint8_t *)msg.data(), msg.length());

        bool ret = ed25519.verify((uint8_t *)msg.data(), msg.length(), signature.data(), signature.size());

        assert(ret);
    }
    catch (const std::exception &e)
    {
        std::cerr << "error at funciton " << e.what() << '\n';
        assert(false);
    }
}