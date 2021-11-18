//! Note further that the Key Derivation Function (KDF) chosen in the derivation of Child
//! Private Keys adheres to [HKDF RFC 5869](https://tools.ietf.org/html/rfc5869).

#include "wallet.hpp"
#include <openssl/hmac.h>
#include <openssl/kdf.h>
#include <openssl/evp.h>
#include <openssl/params.h>

using namespace std;

extern char *WORDS[2048]; // in mnemonic.cpp

namespace violas
{
    const string MNEMONIC_SALT_PREFIX = "DIEM WALLET: mnemonic salt prefix$";
    const size_t iter_count = 10'000;

    Wallet::Wallet(/* args */)
    {
    }

    Wallet::~Wallet()
    {
    }

    void generate()
    {
        Key key, root_key;

        PKCS5_PBKDF2_HMAC((const char *)root_key.data(), root_key.size(),
                          (const uint8_t *)MNEMONIC_SALT_PREFIX.data(), MNEMONIC_SALT_PREFIX.length(),
                          iter_count,
                          EVP_sha3_256(),
                          key.size(), key.data());
    }

    void Wallet::create_next_account()
    {        
        EVP_KDF *kdf;
        EVP_KDF_CTX *kctx;
        unsigned char out[10];
        OSSL_PARAM params[5], *p = params;

        kdf = EVP_KDF_fetch(NULL, "HKDF", NULL);
        kctx = EVP_KDF_CTX_new(kdf);
        EVP_KDF_free(kdf);

        *p++ = OSSL_PARAM_construct_utf8_string(OSSL_KDF_PARAM_DIGEST,
                                                SN_sha256, strlen(SN_sha256));
        *p++ = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_KEY,
                                                 "secret", (size_t)6);
        *p++ = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_INFO,
                                                 "label", (size_t)5);
        *p++ = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_SALT,
                                                 "salt", (size_t)4);
        *p = OSSL_PARAM_construct_end();
        if (EVP_KDF_derive(kctx, out, sizeof(out), params) <= 0)
        {
            error("EVP_KDF_derive");
        }

        EVP_KDF_CTX_free(kctx);
    }

    array<uint8_t, 16>
    Wallet::get_account_address(size_t index)
    {
        auto pubkey = ed25519::PrivateKey::generate().get_public_key().get_raw_key();
        array<uint8_t, 16> address;

        copy(begin(pubkey), begin(pubkey) + 16, begin(address));

        return address;
    }

    std::vector<std::array<uint8_t, 16>>
    Wallet::get_all_accounts()
    {
        return std::vector<std::array<uint8_t, 16>>();
    }
}
