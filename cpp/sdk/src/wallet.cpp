//! Note further that the Key Derivation Function (KDF) chosen in the derivation of Child
//! Private Keys adheres to [HKDF RFC 5869](https://tools.ietf.org/html/rfc5869).

/**
 * @file wallet.cpp
 * @author Hunter (HunterSun2018@gmail.com)
 * @brief
 * @version 0.1
 * @date 2021-11-22
 *
 * @copyright Copyright (c) 2021
 *
 * the 11 bits of word index stores in two bytes
 * 10 9 8 7 6 5 4 3 2 1 0
 * [    byte 0    ] [ byte1 ]
 */

#include <iostream>
#include <sstream>
#include <iterator>
#include <bitset>
// Open SSL
#include <openssl/hmac.h>
#include <openssl/kdf.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

#include "mnemonic.hpp"
#include "wallet.hpp"

using namespace std;

namespace violas
{
    const string MNEMONIC_SALT_PREFIX = "DIEM WALLET: mnemonic salt prefix$"
                                        "Diem";
    const size_t iter_count = 10'000;

    Wallet::Wallet(/* args */)
    {
    }

    Wallet::~Wallet()
    {
    }

    void Wallet::generate_seed()
    {
        auto mnemoic = export_mnemonic();

        int ret = PKCS5_PBKDF2_HMAC((const char *)mnemoic.data(), mnemoic.length() - 1,
                          (const uint8_t *)MNEMONIC_SALT_PREFIX.data(), MNEMONIC_SALT_PREFIX.length(),
                          //(const uint8_t *)"mnemonic", 8,
                          2048,
                          EVP_sha3_256(),
                          seed.size(), seed.data());
    }

    bool verify(const Key &key)
    {
        uint8_t hash[32];

        uint8_t *r = SHA256(key.data(), 32, hash);

        return hash[0] == key[32];
    }

    Wallet Wallet::load_mnemonic(string_view mnemonic)
    {
        Key key = {0};

        istringstream oss(mnemonic.data());
        size_t count = distance(istream_iterator<string>(oss), {});

        vector<size_t> count_define = {24, 21, 18, 15, 12};
        if (end(count_define) == find(begin(count_define), end(count_define), count))
            __throw_runtime_error("Mnemonic must have a word count of the following lengths: 24, 21, 18, 15, 12");

        // search(begin(count_define), end(count_define), )
        bitset<264> bits;
        int bit_index = 0;

        oss.clear();
        oss.str(mnemonic.data());

        for_each(istream_iterator<string>(oss),
                 {},
                 [&bits, &bit_index](auto word)
                 {
                     auto found = lower_bound(begin(WORDS), end(WORDS), word);
                     if (found == end(WORDS))
                         __throw_runtime_error((string("Mnemonic contains a unkonwn word : ") + word).c_str());

                     size_t index = distance(begin(WORDS), found);
                     uint8_t len = (uint8_t)index;
                     // convert index to 11 bits to bitset
                     for (int i = 10; i >= 0; i--)
                     {
                         if (index & (1 << i))
                             bits.set(bit_index);

                         ++bit_index;
                     }
                 });

        for (size_t i = 0; i < key.size() * 8; i++)
        {
            size_t index = i / 8;
            size_t bit = 7 - i % 8;
            uint8_t byte = ((bits.test(i) ? 1 : 0) << bit);

            key[index] |= byte;
        }

        if (!verify(key))
            __throw_runtime_error("Failed to verify mnemonic.");

        Wallet wallet;

        wallet.key = key;
        wallet.generate_seed();

        return wallet;
    }

    string Wallet::export_mnemonic()
    {
        uint8_t hash[32];

        uint8_t *r = SHA256(key.data(), 32, hash);
        key[32] = hash[0];

        ostringstream oss;
        size_t word_index = 0;

        for (int i = 0; i < key.size() * 8; i++)
        {

            size_t byte_index = i / 8;
            size_t bit_index = 7 - i % 8;

            uint8_t byte = key[byte_index];
            if (byte & (1 << bit_index))
                word_index |= (1 << (10 - i % 11));

            if (i % 11 == 10)
            {
                oss << WORDS[word_index] << " ";
                word_index = 0;
            }
        }

        // oss.seekp(-1, ios::cur);

        return oss.str();
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
        //  "7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f",
        //  "legal winner thank year wave sausage worth useful legal winner thank year wave sausage worth useful legal winner thank year wave sausage worth title",
        //  "bc09fca1804f7e69da93c2f2028eb238c227f2e9dda30cd63699232578480a4021b146ad717fbb7e451ce9eb835f43620bf5c514db0f8add49f5d121449d3e87",
        //  "xprv9s21ZrQH143K3Y1sd2XVu9wtqxJRvybCfAetjUrMMco6r3v9qZTBeXiBZkS8JxWbcGJZyio8TrZtm6pkbzG8SYt1sxwNLh3Wx7to5pgiVFU"
        // [
        //     "68a79eaca2324873eacc50cb9c6eca8cc68ea5d936f98787c60c7ebc74e6ce7c",
        //     "hamster diagram private dutch cause delay private meat slide toddler razor book happy fancy gospel tennis maple dilemma loan word shrug inflict delay length",
        //     "64c87cde7e12ecf6704ab95bb1408bef047c22db4cc7491c4271d170a1b213d20b385bc1588d9c7b38f1b39d415665b8a9030c9ec653d75e65f847d8fc1fc440",
        //     "xprv9s21ZrQH143K2XTAhys3pMNcGn261Fi5Ta2Pw8PwaVPhg3D8DWkzWQwjTJfskj8ofb81i9NP2cUNKxwjueJHHMQAnxtivTA75uUFqPFeWzk"
        // ],
        //"7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f";
        Key key;
        key.fill(0x7f);

        // string mnemonic = export_mnemonic(key);

        load_mnemonic("legal winner thank year wave sausage worth useful legal winner thank year wave sausage worth useful legal winner thank year wave sausage worth title");
        load_mnemonic("hamster diagram private dutch cause delay private meat slide toddler razor book happy fancy gospel tennis maple dilemma loan word shrug inflict delay length");

        for (auto byte : key)
        {
            cout << hex << (int)byte;
        }
        cout << endl;
    }
}
