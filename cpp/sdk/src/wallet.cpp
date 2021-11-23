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
#include <memory>
// Open SSL
#include <openssl/hmac.h>
#include <openssl/kdf.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/err.h>

#include "mnemonic.hpp"
#include "wallet.hpp"

using namespace std;

namespace violas
{
    const string MNEMONIC_SALT_PREFIX = "DIEM WALLET: mnemonic salt prefix$";
    const string MAIN_KEY_SALT = "DIEM WALLET: main key salt$";
    const string INFO_PREFIX = "DIEM WALLET: derived key$";

    const size_t iter_count = 10'000;

    static void check_ret(int ret, const char *error)
    {
        std::ostringstream oss;

        oss << error << "failed, error detail : " << ERR_error_string(ERR_get_error(), NULL) << std::endl;

        if (ret != 1)
            std::__throw_runtime_error(oss.str().c_str());
    }

    Wallet::Wallet(/* args */)
    {
    }

    Wallet::~Wallet()
    {
    }

    void Wallet::generate_seed(string_view salt)
    {
        auto mnemoic = export_mnemonic();
        string salt_ex = MNEMONIC_SALT_PREFIX + salt.data();

        int ret = PKCS5_PBKDF2_HMAC((const char *)mnemoic.data(), mnemoic.length(),
                                    (const uint8_t *)salt_ex.data(), salt_ex.length(),
                                    2048,
                                    EVP_sha3_256(),
                                    seed.size(), seed.data());
    }
    // An implementation of HKDF, the [HMAC-based Extract-and-Expand Key Derivation Function][1].
    void Wallet::extract_main_key()
    {
        int ret = 0;
        using hmac_ctx_ptr = unique_ptr<HMAC_CTX, decltype(&HMAC_CTX_free)>;
        hmac_ctx_ptr ctx(HMAC_CTX_new(), &HMAC_CTX_free);

        ret = HMAC_Init_ex(ctx.get(), MAIN_KEY_SALT.data(), MAIN_KEY_SALT.length(), EVP_sha3_256(), nullptr);
        check_ret(ret, "HMAC_Init_ex");

        ret = HMAC_Update(ctx.get(), seed.data(), seed.size());
        check_ret(ret, "HMAC_Update");

        uint32_t len = main_key.size();
        ret = HMAC_Final(ctx.get(), main_key.data(), &len);
        check_ret(ret, "HMAC_Final");
    }

    Wallet::Key Wallet::extend_child_private_key(size_t index)
    {
        int ret = 0;
        std::array<uint8_t, 32> out;
        size_t outlen = out.size();
        using evp_pkey_ctx_ptr = unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)>;
        evp_pkey_ctx_ptr ctx(EVP_PKEY_CTX_new_id(EVP_PKEY_HKDF, NULL), &EVP_PKEY_CTX_free);

        ret = EVP_PKEY_derive_init(ctx.get());
        check_ret(ret, "EVP_PKEY_derive_init");

        ret = EVP_PKEY_CTX_set_hkdf_md(ctx.get(), EVP_sha3_256());
        check_ret(ret, "EVP_PKEY_CTX_set_hkdf_md");

        ret = EVP_PKEY_CTX_set1_hkdf_salt(ctx.get(), MAIN_KEY_SALT.data(), MAIN_KEY_SALT.length());
        check_ret(ret, "EVP_PKEY_CTX_set1_hkdf_salt");

        ret = EVP_PKEY_CTX_set1_hkdf_key(ctx.get(), seed.data(), seed.size());
        check_ret(ret, "EVP_PKEY_CTX_set1_hkdf_key");

        vector<uint8_t> info(INFO_PREFIX.size() + 8, 0);
        copy(begin(INFO_PREFIX), end(INFO_PREFIX), begin(info));

        ret = EVP_PKEY_CTX_add1_hkdf_info(ctx.get(), info.data(), info.size());
        check_ret(ret, "EVP_PKEY_CTX_add1_hkdf_info");

        ret = EVP_PKEY_derive(ctx.get(), out.data(), &outlen);
        check_ret(ret, "EVP_PKEY_derive");

        return out;
    }    

    Wallet Wallet::generate_from_mnemonic(string_view mnemonic)
    {
        std::array<uint8_t, 33> key = {0};

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

        // verify hash sum
        uint8_t hash[32];

        SHA256(key.data(), 32, hash);
        if (key[32] != hash[0])
            __throw_runtime_error("Failed to verify mnemonic.");

        Wallet wallet;

        wallet.key = key;
        wallet.generate_seed();

        wallet.extract_main_key();

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
                if (i != 10)
                    oss << " ";

                oss << WORDS[word_index];

                word_index = 0;
            }
        }

        oss.seekp(-1, ios::end);
        oss.flush();

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
        // string mnemonic = export_mnemonic(key);
        Wallet wallet = Wallet::generate_from_mnemonic("legal winner thank year wave sausage worth useful legal winner thank year wave sausage worth useful legal winner thank year wave sausage worth title");

        Key key = wallet.extend_child_private_key(0);

        generate_from_mnemonic("hamster diagram private dutch cause delay private meat slide toddler razor book happy fancy gospel tennis maple dilemma loan word shrug inflict delay length");

        for (auto byte : key)
        {
            cout << hex << (int)byte;
        }
        cout << endl;
    }
}
