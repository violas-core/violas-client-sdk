//! Note further that the Key Derivation Function (KDF) chosen in the derivation of Child
//! Private Keys adheres to [HKDF RFC 5869](https://tools.ietf.org/html/rfc5869).
#include <iostream>
#include <sstream>
#include <iterator>
#include <bitset>
// Open SSL
#include <openssl/hmac.h>
#include <openssl/kdf.h>
#include <openssl/evp.h>

#include "wallet.hpp"

using namespace std;

extern const char *WORDS[2048]; // in mnemonic.cpp

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

    Key load_mnemonic(string_view mnemonic)
    {
        Key key = {0};

        istringstream oss(mnemonic.data());
        size_t count = distance(istream_iterator<string>(oss), {});

        vector<size_t> count_define = {24, 21, 18, 15, 12};
        if (end(count_define) == find(begin(count_define), end(count_define), count))
            __throw_runtime_error("Mnemonic must have a word count of the following lengths: 24, 21, 18, 15, 12");

        // search(begin(count_define), end(count_define), )
        bitset<256> bits;
        int bit_index = 0;

        istringstream oss1(mnemonic.data());
        for_each(istream_iterator<string>(oss1),
                 {},
                 [&bits, &bit_index](auto word)
                 {
                     auto found = lower_bound(begin(WORDS), end(WORDS), word);
                     if (found == end(WORDS))
                         __throw_runtime_error((string("Mnemonic contains a unkonwn word : ") + word).c_str());

                     size_t index = distance(begin(WORDS), found);
                     for (int i = 0; i < 11; i++)
                     {
                         if (index & 0x1)
                             bits.set(bit_index);

                         ++bit_index;
                         index >>= 1;
                     }
                 });

        cout << bits.to_string() << endl;

        for (size_t i = 0; i < 256; i++)
        {
            size_t index = i / 8;
            size_t bit = i % 8;
            uint8_t byte = ((bits.test(i) ? 1 : 0) << bit);

            key[index] |= byte;
        }

        return key;
    }

    void Wallet::create_next_account()
    {
        // EVP_KDF *kdf;
        // EVP_KDF_CTX *kctx;
        // unsigned char out[10];
        // OSSL_PARAM params[5], *p = params;

        // kdf = EVP_KDF_fetch(NULL, "HKDF", NULL);
        // kctx = EVP_KDF_CTX_new(kdf);
        // EVP_KDF_free(kdf);

        // *p++ = OSSL_PARAM_construct_utf8_string(OSSL_KDF_PARAM_DIGEST,
        //                                         SN_sha256, strlen(SN_sha256));
        // *p++ = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_KEY,
        //                                          "secret", (size_t)6);
        // *p++ = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_INFO,
        //                                          "label", (size_t)5);
        // *p++ = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_SALT,
        //                                          "salt", (size_t)4);
        // *p = OSSL_PARAM_construct_end();
        // if (EVP_KDF_derive(kctx, out, sizeof(out), params) <= 0)
        // {
        //     error("EVP_KDF_derive");
        // }

        // EVP_KDF_CTX_free(kctx);
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

    void Wallet::run_test_case()
    {
        Key key = load_mnemonic("legal winner thank year wave sausage worth useful legal winner thank year wave sausage worth useful legal will");
        
        for(auto byte : key)
        {
            cout << hex << (int)byte;
        }
        cout << endl;
    }
}
